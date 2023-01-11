// defines pins numbers

const int trigPin = 5;    //Common trigger
const int echoPin1 = 18;  //D2
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
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT);  // Sets the echoPin as an Input
  pinMode(echoPin2, INPUT);  // Sets the echoPin as an Input
  pinMode(echoPin3, INPUT);  // Sets the echoPin as an Input

  Serial.begin(115200);  // Starts the serial communication
}

void loop() {
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
  delay(800);
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