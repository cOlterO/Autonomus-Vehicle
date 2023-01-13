#define nanoPin1 36
#define nanoPin2 39

char prediction;

void setup() {
  // Prediction from Nano
  pinMode(nanoPin1,INPUT);
  pinMode(nanoPin2,INPUT);
  // Prediction from Nano
  Serial.begin(9600);
}

void loop() {
  getPrediction();
  Serial.print("Prediction is: ");
  Serial.println((char)prediction);
  delay(1000);

}

void getPrediction() {
  if (digitalRead(nanoPin1) == LOW && digitalRead(nanoPin2) == LOW) {
    prediction = 'u';
  }
  else if (digitalRead(nanoPin1) == HIGH && digitalRead(nanoPin2) == HIGH) {
    prediction = 's';
  }
  else if (digitalRead(nanoPin1) == HIGH && digitalRead(nanoPin2) == LOW) {
    prediction = 'r';
  }
  else if (digitalRead(nanoPin1) == LOW && digitalRead(nanoPin2) == HIGH) {
    prediction = 'l';
  }
}
