// Motor A(left) connections____________________________
const int enA = D1;  // speed
const int in1 = D2;
const int in2 = D3;

// Motor B(right) connections
const int enB = D8;  // speed
const int in3 = D6;
const int in4 = D7;


// pins distance sensor___________________________________
const int trigPin = 5;  //Common trigger
const int echoPin1 = 18;
const int echoPin2 = 19;
const int echoPin3 = 20;

// defines variables
long duration1;
long distance1;
long duration2;
long distance2;
long duration3;
long distance3;


void setup() {
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Turn off motors - Initial state
  initialStateMotors();

  Serial.begin(115200);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);  // LOW means ON in ESP8266 LED_BUILTIN
  distanceSensors();               // Take measurements from the 3 sensors

  if (distance2 < 10) {    // If the front sensor is close to a wall
    initialStateMotors();  // Stop the car
                           //send signal to Arduino Nano 33
                           // wait for response
                           /*
    switch (arduinoNanoResponse) {
      case "pedestrian":
        initialStateMotors();

      case "stop":
        initialStateMotors();  // Stop again
        delay(1500);           // wait
        forward();             // slowly approach until distance is =< 3

      case "left":
        left1motor(900);

      case "right":
        right1motor(900);
    }*/

  } else if (distance1 < 5) {  // approaching left wall
    initialStateMotors();      //stop
    right1motor(120);          //turn left a bit
    forward();                 // keep going forward

  } else if (distance3 < 5) {  // approaching right wall
    initialStateMotors();      //stop
    left1motor(120);           //turn left a bit
    forward();                 // keep going forward

  } else {  // everything is OK, keep going forward
    forward();
  }
}

void right1motor(int time) {  // makes a right turn using 1 motor lasting the time you specified
  analogWrite(enA, 130);
  analogWrite(enB, 130);

  //left motor spins
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  //right motor OFF
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  delay(time);
  initialStateMotors();
}

void left1motor(int time) {  // makes a left turn using 1 motor lasting the time you specified
  analogWrite(enA, 130);
  analogWrite(enB, 130);

  //left motor OFF
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  //right motor spin
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  delay(time);
  initialStateMotors();
}

void right2motor() {
  analogWrite(enA, 130);
  analogWrite(enB, 130);

  //left motor
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  //right motor
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800);
  initialStateMotors();
}

void forward() {
  analogWrite(enA, 130);
  analogWrite(enB, 130);

  //left motor
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  //right motor
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void initialStateMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

/* //Calibration
  for (int i = 120; i < 150; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);

    //left motor
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    //right motor
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    Serial.println(i);
    delay(1000);
  }
  //*/

void distanceSensors() {
  distance1 = sample_distance(duration1, distance1, echoPin1);
  distance2 = sample_distance(duration2, distance2, echoPin2);
  distance3 = sample_distance(duration3, distance3, echoPin3);

  Serial.println("");
  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.println("cm");

  Serial.print("Distance 2: ");
  Serial.print(distance2);
  Serial.println("cm");

  Serial.print("Distance 3: ");
  Serial.print(distance3);
  Serial.println("cm");
}

int sample_distance(int duration, int distance, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = int((duration / 2) / 29.155);
  return distance;
}