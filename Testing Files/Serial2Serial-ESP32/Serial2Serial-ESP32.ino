/*
Serial2Serial Arduino Nano 33 BLE to ESP32 1-wire
Nano 33: TX:D11 RX:D7 (Not connected), UART mySerial(TX3Pin, RX3Pin, NC, NC);
ESP32: TX:D11/GPIO35 RX:D10/GPIO34 (Not connected), NanoSerial.begin(9600,SERIAL_8N1,RX,TX);
*/
#define RXD2 16
#define TXD2 17

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  while (!Serial);
  Serial.println("Serial2Serial Arduino Nano 33 BLE to ESP32 1-wire");
}


void loop() {

   while (Serial2.available() >= 0) {
    Serial.println(Serial2.readString());
  }

  delay(1000);  // delay in between reads for stability
}
