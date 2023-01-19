/*
  ___        _                                                _____            
 / _ \      | |                                              /  __ \           
/ /_\ \_   _| |_ ___  _ __   ___  _ __ ___   ___  _   _ ___  | /  \/ __ _ _ __ 
|  _  | | | | __/ _ \| '_ \ / _ \| '_ ` _ \ / _ \| | | / __| | |    / _` | '__|
| | | | |_| | || (_) | | | | (_) | | | | | | (_) | |_| \__ \ | \__/\ (_| | |   
\_| |_/\__,_|\__\___/|_| |_|\___/|_| |_| |_|\___/ \__,_|___/  \____/\__,_|_|   
                                                                               
*/
#include <string>
#include <NewPing.h>  // Sonar library

#define SONAR_NUM 3       // Number of sensors.
#define MAX_DISTANCE 300  // Maximum distance (in cm) to ping.

NewPing sonar[SONAR_NUM] = {      // Sensor object array.
  NewPing(21, 22, MAX_DISTANCE),  // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(33, 35, MAX_DISTANCE),
  NewPing(26, 27, MAX_DISTANCE)
};



// defines variables for distance
long duration;
long distance;


// Motor A(left) connections
const int enA = 14;
const int in1 = 13;
const int in2 = 32;
// Motor B(right) connections
const int enB = 18;  //brown
const int in3 = 23;  //red
const int in4 = 5;   //orange

//camera
#define RXD2 16
#define TXD2 17



//buzzer musical pitches
#include "pitches.h"

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

int durations2[] = {
  4, 2, 8, 8,
  4, 4, 3, 6,
  5, 2, 3, 3,
  2
};

// Variables for distance of the ultrasound sensors
int d1 = 0;  // Front
int d2 = 0;  // Left
int d3 = 0;  // Right

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


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


//Networking credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Whax";      //SHOULD BE CHANGED
char pass[] = "1234567a";  //SHOULD BE CHANGED

//Pinouts
// #define LED_BUILTIN 2
#define tempPIN 33
#define offsetValue 2

int pwmEnA;
int pwmEnB;

BlynkTimer timer;

BLYNK_WRITE(V10){

 pwmEnA =param.asInt();

}
BLYNK_WRITE(V11){

 pwmEnB =param.asInt();

}

// Updates every 1S
void myTimerEvent() {
  // Blynk.virtualWrite(V0, digitalRead(LED_BUILTIN));               //(INT) inbuild Led state.
  Blynk.virtualWrite(V1, d1);             //(INT) Front Ultrasensor value on xxx
  Blynk.virtualWrite(V3, d2);             //(INT) Left Ultrasensor value
  Blynk.virtualWrite(V2, d3);             //(INT) Right Ultrasenor value
  //Blynk.virtualWrite(V4, tempData());     //(Float) Battery temp
  Blynk.virtualWrite(V5, showNewData());  //(String) AI prediction
                                          //Blynk.virtualWrite(V6, xxx);                                  //Right motor LED if needed
  //Blynk.virtualWrite(V7, xxx);                                   //Left motor LED if needed
}



const byte numChars = 32;
char receivedChars[numChars];  // an array to store the received data

boolean newData = false;


// int initialSpeed = 140;  // pwm that forward() starts with


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);


  Blynk.begin(auth, ssid, pass);
  //Function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  // Turn off motors - Initial state(All LOW)
  initialStateMotors();
  while (!Serial)
    ;
  Serial.println("Serial2Serial Arduino Nano 33 BLE to ESP32 1-wire");
}


void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer

  dMeasure();  //measures and stores distance

  recvWithEndMarker();  //Serial to serial communication
  showNewData();        //returns string prediction from serial 2


  if (d1 > 40) {
    Serial.println("FORWARD");
    forward(pwmEnA,pwmEnB);
    Serial.println("left pwm value is: " + (String)pwmEnA);
    Serial.println("left pwm value is: " + (String)pwmEnB);
    dMeasure();
  } else {
    Serial.println("MOVEMENT");
    initialStateMotors();
    //   movement(prediction);
    // }
  }
}


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

String showNewData() {
  if (newData == true) {
    Serial.print("Prediction is: ");
    Serial.println(receivedChars);
    prediction = receivedChars;
    newData = false;
    return prediction;
  }
}

float tempData() {

  int inputValue = analogRead(tempPIN);              //Reads the value of potentiometer
  float milliVolt = inputValue * (3300.0 / 4096.0);  //Calculates the corressponding milli Voltage range from 0 - 5V
  float temp = milliVolt / 10;                       //Datasheet specifies that LM35 reads 10mv/°C and adds to the value before.
  temp = temp * offsetValue;

  /*Debuging thing for temp 
  Serial.println("......................");
  Serial.println(" "); 
  Serial.println("Reading: " + (String)inputValue); 
  Serial.println("volt: " + (String)milliVolt); 
  Serial.println("Temp: " + (String)temp); 
  Serial.println("Temp DHT: " + (String)dht.readTemperature());
  Serial.println(" "); 
  Serial.println("......................");
  */

  return temp;
}

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

void dMeasure() {
  d1 = sonar[0].ping_cm();
  delay(30);
  d2 = sonar[1].ping_cm();
  delay(30);
  d3 = sonar[2].ping_cm();
  delay(30);

  Serial.print("Front Sensor: ");
  Serial.println(sonar[0].ping_cm());
  Serial.print("Left Sensor: ");
  Serial.println(sonar[1].ping_cm());
  Serial.print("Right Sensor: ");
  Serial.println(sonar[2].ping_cm());
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
      right1motor(640);
      Serial.println("Turning Right");
      // delay(3000);
      break;

    case 'l':
      left1motor(640);
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
void aForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void bForward() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void forward(int pwmEnA, int pwmEnB) {
  analogWrite(enA, pwmEnA);
  analogWrite(enB, pwmEnB);
  aForward();
  bForward();
  // for (int initialSpeed; initialSpeed < 180; initialSpeed++) {
  //   analogWrite(enA, initialSpeed);
  //   analogWrite(enB, initialSpeed);
  //   delay(20);
  // }
}

void backward() {
  analogWrite(enA, 134);
  analogWrite(enB, 140);
  aBackward();
  bBackward();
}


void aBackward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void bBackward() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void left1motor(int time) {  // makes a left turn using 1 motor lasting the time you specified
  analogWrite(enA, 160);
  analogWrite(enB, 160);

  //left motor OFF
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  //right motor spin
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(time);
  initialStateMotors();
}

void right1motor(int time) {
  analogWrite(enA, 160);
  analogWrite(enB, 160);

  //right motor OFF
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //left motor spin
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  delay(time);
  initialStateMotors();
}