
//Ultrasonic connections
const int trigPin = 33;
const int echoPin = 35;


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

const byte LED_BUILTIN = 32;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  // Starts the serial communication

  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  // attachInterrupt(digitalPinToInterrupt(sensor_pin), movement_detection, RISING);

  // Turn off motors - Initial state(All LOW)
  initialStateMotors();
}

void loop() {
  // put your main code here, to run repeatedly:
  stop();
  

  
}

void stop(){
  int distance = sample_distance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  delay(100);

  

  while (distance >= 20 || distance == 0) {

    forward();

    distance = sample_distance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println("cm");
    delay(40);
  }


  //motors off
  initialStateMotors();
  //delay(3000);
}

int sample_distance() {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Serial.println("Duration: ");
  // Serial.println(duration);
  distance = int((duration / 2) / 29.155);
  return distance;
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
  analogWrite(enA, 140);
  analogWrite(enB, 140);
  aForward();
  bForward();
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
