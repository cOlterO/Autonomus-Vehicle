#include <movingAvg.h>  // https://github.com/JChristensen/movingAvg

movingAvg HCSR04_Left(5);
movingAvg HCSR04_Front(5);
movingAvg HCSR04_Right(5);

// defines pins numbers
const int trigPin = 5;    //Common trigger
const int echoPin1 = 18;  //D2
const int echoPin2 = 19;
const int echoPin3 = 21;

// defines variables
long duration1;
long distance1;
long duration2;
long distance2;
long duration3;
long distance3;


void setup() {
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT);  // Sets the echoPin as an Input
  pinMode(echoPin2, INPUT);  // Sets the echoPin as an Input
  pinMode(echoPin3, INPUT);  // Sets the echoPin as an Input

  Serial.begin(115200);  // Starts the serial communication

  HCSR04_Left.begin();
  HCSR04_Front.begin();
  HCSR04_Right.begin();
}

void loop() {
  sample_distance_3sensros();

  HCSR04_Left.reading(distance1);
  HCSR04_Front.reading(distance2);
  HCSR04_Right.reading(distance3);

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
  delay(300);

  /*Serial.print("AVG left: ");
  Serial.println(HCSR04_Left.getAvg());

  Serial.print("AVG front: ");
  Serial.println(HCSR04_Front.getAvg());

  Serial.print("AVG: right: ");
  Serial.println(HCSR04_Right.getAvg());
  Serial.println("____________");
  */
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

void sample_distance_3sensros() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(trigPin, LOW);

  float duration1 = pulseIn(echoPin1, HIGH, 2000); // we use a timeout because the wall won't be that far
  float duration2 = pulseIn(echoPin2, HIGH); 
  float duration3 = pulseIn(echoPin3, HIGH, 2000); // we use a timeout because the wall won't be that far

  distance1 = int((duration1 / 2) / 29.155);
  distance2 = int((duration2 / 2) / 29.155);
  distance3 = int((duration3 / 2) / 29.155);
  
}