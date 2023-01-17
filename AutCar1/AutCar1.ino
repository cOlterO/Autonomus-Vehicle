/*
  ___        _                                                _____            
 / _ \      | |                                              /  __ \           
/ /_\ \_   _| |_ ___  _ __   ___  _ __ ___   ___  _   _ ___  | /  \/ __ _ _ __ 
|  _  | | | | __/ _ \| '_ \ / _ \| '_ ` _ \ / _ \| | | / __| | |    / _` | '__|
| | | | |_| | || (_) | | | | (_) | | | | | | (_) | |_| \__ \ | \__/\ (_| | |   
\_| |_/\__,_|\__\___/|_| |_|\___/|_| |_| |_|\___/ \__,_|___/  \____/\__,_|_|   
                                                                               
*/
#include <NewPing.h>  // Sonar library

#define SONAR_NUM 3       // Number of sensors.
#define MAX_DISTANCE 250  // Maximum distance (in cm) to ping.

NewPing sonar[SONAR_NUM] = {      // Sensor object array.
  NewPing(33, 35, MAX_DISTANCE),  // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(22, 23, MAX_DISTANCE),
  NewPing(18, 19, MAX_DISTANCE)
};



// defines variables for distance
long duration;
long distance;


// Motor A(left) connections
const int enA = 13;
const int in1 = 12;
const int in2 = 14;
// Motor B(right) connections
const int enB = 25;
const int in3 = 26;
const int in4 = 27;

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
char pass[] = "1234567a";  //SAME HER TOO

//Pinouts
// #define LED_BUILTIN 2
#define tempPIN 33
#define offsetValue 2

BlynkTimer timer;


// Updates every 1S
void myTimerEvent() {
  // Blynk.virtualWrite(V0, digitalRead(LED_BUILTIN));               //(INT) inbuild Led state.
  Blynk.virtualWrite(V1, d1);          //(INT) Front Ultrasensor value on xxx
  Blynk.virtualWrite(V2, d2);          //(INT) Left Ultrasensor value
  Blynk.virtualWrite(V3, d3);          //(INT) Right Ultrasenor value
  Blynk.virtualWrite(V4, tempData());  //(Float) Battery temp
  Blynk.virtualWrite(V5, lastPred);  //(String) AI prediction
                                       //Blynk.virtualWrite(V6, xxx);                                  //Right motor LED if needed
  //Blynk.virtualWrite(V7, xxx);                                   //Left motor LED if needed
}



const byte LED_BUILTIN = 32;



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

  dMeasure();
  //fetches prediction
  serial2Serial();

  //forward if car isnt too close to wall
  if (d1 > 40) {
    forward();
    dMeasure();
  } else {
    Serial.println("ENTERS ELSE");
    //Stops car
    initialStateMotors();
    //performs movement based on prediction
    movement(prediction);

    //stores latest prediction so that it doesnt get cleared(For IOT)
    lastPred = prediction;

    //resets buffer
    prediction = "";
    Serial2.end();
    Serial2.begin(115200);
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

//gets Serial reading from nano microcontroller. (The prediction)
void serial2Serial() {
  while (Serial2.available() >= 1) {
    delay(2);

    char data = Serial2.read();
    prediction += data;
  }
  if (prediction.length() > 1) {
    prediction = prediction.substring(1);
  } else {
    Serial.println("NoPrediction");
  }
}

//switch case for predictions
void movement(String prediction) {

  switch (tolower(prediction[0])) {

    case 'u':
      Serial.println("UNKNOWNWNW");
      break;

    case 's':
      nokia();
      delay(5000);
      break;

    case 'r':
      right1motor(640);
      Serial.println("Turning Right");
      delay(5000);
      break;

    case 'l':
      left1motor(640);
      delay(5000);
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
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void bForward() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void forward() {
  analogWrite(enA, 134);
  analogWrite(enB, 142);
  aForward();
  bForward();
}

void backward() {
  analogWrite(enA, 134);
  analogWrite(enB, 140);
  aBackward();
  bBackward();
}


void aBackward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void bBackward() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void left1motor(int time) {  // makes a left turn using 1 motor lasting the time you specified
  analogWrite(enA, 160);
  analogWrite(enB, 160);

  //left motor OFF
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  //right motor spin
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

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
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  delay(time);
  initialStateMotors();
}