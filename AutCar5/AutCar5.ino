/*
  ___        _                                                _____            
 / _ \      | |                                              /  __ \           
/ /_\ \_   _| |_ ___  _ __   ___  _ __ ___   ___  _   _ ___  | /  \/ __ _ _ __ 
|  _  | | | | __/ _ \| '_ \ / _ \| '_ ` _ \ / _ \| | | / __| | |    / _` | '__|
| | | | |_| | || (_) | | | | (_) | | | | | | (_) | |_| \__ \ | \__/\ (_| | |   
\_| |_/\__,_|\__\___/|_| |_|\___/|_| |_| |_|\___/ \__,_|___/  \____/\__,_|_|   
                                                                               
*/
#include <string> //string manpiulation library
#include <NewPing.h>  // Sonar library
#include <ESP_LM35.h> // temperature library
#include "pitches.h"//buzzer musical pitches
#include <WiFi.h> //wifi library
#include <WiFiClient.h> //wifi library
#include <BlynkSimpleEsp32.h> //blyink iot library



#define SONAR_NUM 3       // Number of sensors.
#define MAX_DISTANCE 300  // Maximum distance (in cm) to ping.

NewPing sonar[SONAR_NUM] = {      // Sensor object array.
  NewPing(21, 22, MAX_DISTANCE),  // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(33, 35, MAX_DISTANCE),
  NewPing(26, 27, MAX_DISTANCE)
};

unsigned long myTime;
unsigned long prevTime;

// LM 35 temp sensor
ESP_LM35 temp(34);
const int tempPIN = 34;
const int offsetValue = 2;

// defines variables for distance
long duration;
long distance;


// Motor A(left) connections
const int enA = 14;
const int in1 = 13;
const int in2 = 32;
const int PWM_left = 142;

// Motor B(right) connections
const int enB = 18;  //brown
const int in3 = 23;  //red
const int in4 = 5;   //orange
const int PWM_right = 140;

int pwmEnA; // pwm values for Motor A 
int pwmEnB; //pwm values for Motor B


//camera
#define RXD2 16
#define TXD2 17


// Buzzer pin
const int BUZZER_PIN = 4;
// Nokia melody pitches
int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4,
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4,
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};
// Nokia melody duration lenght of pitches
int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};


// Variables for distance of the ultrasound sensors
int d1;  // Front
int d2;  // Left
int d3;  // Right

const int distance_to_walls = 30;  // How many cm appart should the sensors read to be centered
// |                   |
// |       |Car|       |
// |-------|   |-------|
// | 22cm   14cm   22cm| = 58cm,   1 cm to each side of tolerance

// Variable where prediction string will be stored
String prediction = "";
String lastPred = "";

//Blynk auth credentials:
//Should be placed before headers
#define BLYNK_TEMPLATE_ID "TMPLBotx3_ZT"
#define BLYNK_DEVICE_NAME "Car controls"
#define BLYNK_AUTH_TOKEN "f1cGTooC6I4YG9aXzC_tDHCHaF_oC-zW"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

//Networking credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "POCO F3";     //SHOULD BE CHANGED
char pass[] = "alcornoque";  //SHOULD BE CHANGED

//Pinouts
#define tempPIN 32
#define offsetValue 2


String direction = " ";

BlynkTimer timer;

BLYNK_WRITE(V10) {

  pwmEnA = param.asInt();
}
BLYNK_WRITE(V11) {

  pwmEnB = param.asInt();
}

// Updates every 1S
void myTimerEvent() {
  // Blynk.virtualWrite(V0, digitalRead(LED_BUILTIN));               //(INT) inbuild Led state.
  Blynk.virtualWrite(V1, d1);             //(INT) Front Ultrasensor value on xxx
  Blynk.virtualWrite(V3, d2);             //(INT) Left Ultrasensor value
  Blynk.virtualWrite(V2, d3);             //(INT) Right Ultrasenor value
  Blynk.virtualWrite(V4, temp.tempC());   //(Float) Battery temp
  Blynk.virtualWrite(V5, showNewData());  //(String) AI prediction
  Blynk.virtualWrite(V12, direction);
}

const byte numChars = 32;
char receivedChars[numChars];  // an array to store the received data

boolean newData = false; //flag for serial communication

void setup() {

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  prevTime = millis();
  myTime = millis();
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(tempPIN, INPUT);

  Blynk.begin(auth, ssid, pass);
  //Function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  // Turn off motors - Initial state(All LOW)
  initialStateMotors();
  while (!Serial)
    ;
  Serial.println("Serial2Serial Arduino Nano 33 BLE to ESP32 1-wire");
}


const int h = 3;  // hyrestsis

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer

  dMeasure();  //measures and stores distance

  recvWithEndMarker();  //Serial to serial communication
  showNewData();        //returns string prediction from serial 2

  if (d1 > 27) {

    direction = "Straight"; //variable for blynk 

    forward(PWM_left, PWM_right);
    delay(250);

    initialStateMotors();
    dMeasure();

    const int ideal_distance_to_wall = 20;
    const int corridor_distance = 40;

    //ensures car stays in the middle
    if (((d2 > (d3 + h)) && (d2 < corridor_distance)) || ((d3 > corridor_distance) && (d2 > ideal_distance_to_wall)) || ((d2 > corridor_distance) && (d3 < ideal_distance_to_wall))) {
      direction = "Left";
      correctLeft(240);  // Goes to left


    } 
    //ensures car stays in the middle
    else if (((d3 > (d2 + h)) && (d3 < corridor_distance)) || ((d3 > corridor_distance) && (d2 < ideal_distance_to_wall)) || ((d2 > corridor_distance) && (d3 > ideal_distance_to_wall))) {
      direction = "Right";
      correctRight(180);  // Goes to right
    }

  } else {
    initialStateMotors();
    direction = "Init";
    movement(prediction);
  }

}

//reads in String prediction from serial 2
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial2.available() > 0 && newData == false) {
    rc = Serial2.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

//returns string prediction from recvWithEndMarker()
String showNewData() {
  if (newData == true) {
    //Serial.print("Prediction is: ");
    //Serial.println(receivedChars);
    prediction = receivedChars;
    newData = false;
    return prediction;
  }
}

//buzzer sound for reaching stop sign
void nokia() {
  int size = sizeof(durations) / sizeof(int);

  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);

    //stop the tone playing:
    noTone(BUZZER_PIN);
  }
}

//measures and stores ultrasonic sensor distances
void dMeasure() {
  d1 = (sonar[0].ping_median(4, 200) / 29) / 2;
  Serial.print("Front Sensor: ");
  Serial.println(d1);
  delay(30);
  d2 = (sonar[1].ping_median(4, 200) / 29) / 2;
  Serial.print("Left Sensor: ");
  Serial.println(d2);
  delay(30);
  d3 = (sonar[2].ping_median(4, 200) / 29) / 2;

  Serial.print("Right Sensor: ");
  Serial.println(d3);
  Serial.println("");
}


//switch case for predictions
void movement(String prediction) {

  switch (tolower(prediction[0])) {

    case 'u':
      Serial.println("UNKNOWNWNW");
      break;

    case 's':
      nokia();
      // delay(3000);
      break;

    case 'r':
      correctRight(1000);
      Serial.println("Turning Right");
      // delay(3000);
      break;

    case 'l':
      correctLeft(1100);
      // delay(3000);
      break;

    default:
      break;
  }
}

// Turn off motors - Initial state(All LOW)
void initialStateMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

//set motor a forward
void aForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}
//set motor b forward
void bForward() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

//car drives forward
void forward(int pwmEnA, int pwmEnB) {
  analogWrite(enA, pwmEnA);
  analogWrite(enB, pwmEnB);
  aForward();
  bForward();
}

//makes car turn slightly left
void correctLeft(int time) {  // makes a left turn using 1 motor lasting the time you specified
  analogWrite(enA, 150);
  analogWrite(enB, 170);

  //left stop
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  //right motor spin
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(time);
  initialStateMotors();
}


//Make car turn slightly right
void correctRight(int time) {
  analogWrite(enA, 170);
  analogWrite(enB, 150);


  //right motor OFF
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //left motor spin
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  delay(time);
  initialStateMotors();
}