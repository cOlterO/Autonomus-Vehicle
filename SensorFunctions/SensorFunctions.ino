


void SETUPultrasoundSensor() {
#include <afstandssensor.h>              // !! install library
  AfstandsSensor afstandssensor(10, 9);  // AfstandsSensor(triggerPin, echoPin);
}

void SETUPThingSpeak() {
#include <ESP8266WiFi.h>
#include <Blynk.h>

  


  // Replace with your network details
  const char* ssid = "POCO F3";
  const char* password = "alcornoque";
  WiFiClient client;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void LM35temp() {
  int sensorValue = analogRead(A0);    // Reads the voltage given by the sensor
  float voltage = (value – 500) / 10;  //scale from 0-1023 to between 0-5
  float mV = voltage * 1000;           // convert voltage to millivolt
  return (mV - 500) / 10;              // convert voltage to temperature and return it in celsius
}


void ultrasoundSensor() {
  return afstandssensor.afstandCM();
}


void uploadThingSpeak(distance) {  // we pass the distance measurment we took
  ThingSpeak.begin(client);
  if (client.connect(server, 80)) {
    ThingSpeak.setField(1, rssi);
    ThingSpeak.setField(2, distance);
    ThingSpeak.setField(3, LM35temp());
    ThingSpeak.writeFields(channelID, myWriteAPIKey);
  }
  client.stop();

  // wait and then post again
  // We need to implement this with millis()) delay(postingInterval);
