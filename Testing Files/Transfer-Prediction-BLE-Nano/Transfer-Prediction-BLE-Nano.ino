/*
  This example creates a BLE peripheral with a service that contains two float characteristics.

  The circuit:
  - Arduino Nano 33 BLE Sense board.

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

//----------------------------------------------------------------------------------------------------------------------
// BLE UUIDs
//----------------------------------------------------------------------------------------------------------------------

#define BLE_UUID_PREDICTION_SERVICE "8AF266CF-D302-4B84-8786-69EC57F92379"
#define BLE_UUID_UNKNOWN "83374BB5-D313-4DD5-8F8A-F1DC7848801F"
#define BLE_UUID_STOP "98D096F0-445C-4376-A43C-809AC6BC8D3D"
#define BLE_UUID_RIGHT "47791AB8-AE7A-4758-B54C-446551C6D5F3"
#define BLE_UUID_LEFT "B1E80FE4-0521-4E0D-8D88-2A94B40FD668"
#define BLE_UUID_PEDESTRIAN "E25AC9F4-88D0-4C43-A599-9F9516B226BA"
BLEDescriptor Descriptor("ADC93032-99AA-43FC-B6AE-8688E064D866","Descriptor");

//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------

BLEService predictionService(BLE_UUID_PREDICTION_SERVICE);
BLEFloatCharacteristic UnknownCharacteristic(BLE_UUID_UNKNOWN, BLERead | BLENotify);
BLEFloatCharacteristic StopCharacteristic(BLE_UUID_STOP, BLERead | BLENotify);
BLEFloatCharacteristic RightCharacteristic(BLE_UUID_RIGHT, BLERead | BLENotify);
BLEFloatCharacteristic LeftCharacteristic(BLE_UUID_LEFT, BLERead | BLENotify);
BLEFloatCharacteristic PedestrianCharacteristic(BLE_UUID_PEDESTRIAN, BLERead | BLENotify);


float pred[5];

void setup() {
  Serial.begin(9600);
  while (!Serial);

  BLE.begin();

  // set advertised local name and service
  BLE.setDeviceName("Arduino Nano 33 BLE");
  BLE.setLocalName("Arduino Nano 33 BLE");
  BLE.setAdvertisedService(predictionService);

  // ADD BLE Descriptors for notifications
  UnknownCharacteristic.addDescriptor(Descriptor);
  StopCharacteristic.addDescriptor(Descriptor);
  RightCharacteristic.addDescriptor(Descriptor);
  LeftCharacteristic.addDescriptor(Descriptor);
  PedestrianCharacteristic.addDescriptor(Descriptor);


  // BLE add characteristics
  predictionService.addCharacteristic(UnknownCharacteristic);
  predictionService.addCharacteristic(StopCharacteristic);
  predictionService.addCharacteristic(RightCharacteristic);
  predictionService.addCharacteristic(LeftCharacteristic);
  predictionService.addCharacteristic(PedestrianCharacteristic);


  // add service
  BLE.addService(predictionService);

  // set the initial value for characeristics
  // floatValueCharacteristic.writeValue(floatValue);
  // amplitudeCharacteristic.writeValue(amplitude);
  UnknownCharacteristic.writeValue(pred[0]);
  StopCharacteristic.writeValue(pred[1]);
  RightCharacteristic.writeValue(pred[2]);
  LeftCharacteristic.writeValue(pred[3]);
  PedestrianCharacteristic.writeValue(pred[4]);


  BLE.advertise();
}

void loop() {
  static long previousMillis = 0;
  pred[0] = 20.3;
  pred[1] = 12.3;
  pred[2] = 10.3;
  pred[3] = 77.3;
  pred[4] = 42.3;
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {

      long interval = 3000;
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;

        if (central.rssi() != 0) {
          UnknownCharacteristic.writeValue(pred[0]);
          StopCharacteristic.writeValue(pred[1]);
          RightCharacteristic.writeValue(pred[2]);
          LeftCharacteristic.writeValue(pred[3]);
          PedestrianCharacteristic.writeValue(pred[4]);
        }
        // delay(1000);
      }
    }
  }
}
// delay(2000);
