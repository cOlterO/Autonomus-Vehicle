
#include <NewPing.h>

#define SONAR_NUM 3       // Number of sensors.
#define MAX_DISTANCE 100  // Maximum distance (in cm) to ping.

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



//buzzer stuff
#include "pitches.h"

const int BUZZER_PIN = 4;

int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4,
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4,
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};


int d1 = 0;
int d2 = 0;
int d3 = 0;



const byte LED_BUILTIN = 32;

String prediction = "";

//Initialize distance for Ultrasound sensors



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

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  // attachInterrupt(digitalPinToInterrupt(sensor_pin), movement_detection, RISING);

  // Turn off motors - Initial state(All LOW)
  initialStateMotors();
  while (!Serial)
    ;
  Serial.println("Serial2Serial Arduino Nano 33 BLE to ESP32 1-wire");
}

void loop() {

  takeMeasurements();

  //checks car isnt too close to side walls
  while ((d2 > 5) && (d3 > 5)) {
    takeMeasurements();
    Serial.println("HERE");

    //car stops in front of wall
    if (d1 > 20) {
      takeMeasurements();
      Serial.println("FORWARD!");
      forward();

    } else {
      initialStateMotors();
      Serial.println("Reading sign");
      serial2Serial();
      delay(3000);
      movement(prediction);
      prediction = "";
      Serial2.end();
      Serial2.begin(115200);
    }
  }
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

void takeMeasurements() {
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

    Serial.println(prediction.substring(1));
    serial
  }else{
    Serial.println("NoPrediction");
  }
}

void movement(String prediction) {

  switch (tolower(prediction[0])) {

    case 'u':
      Serial.println("UNKNOWNWNW");

    case 's':
      nokia();

    case 'r':
      right1motor(500);
      Serial.println("Turning Right");

    case 'l':
      left1motor(500);
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
  analogWrite(enB, 140);
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
  analogWrite(enA, 140);
  analogWrite(enB, 140);

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
  analogWrite(enA, 140);
  analogWrite(enB, 140);

  //right motor OFF
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //left motor spin
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  delay(time);
  initialStateMotors();
}