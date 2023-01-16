/*********
  ESP32 BLE Client for getting predictions from Arduino Nano 33 BLE
*********/

#include "BLEDevice.h"

//BLE Server name (the other ESP32 name running the server sketch)
#define bleServerName "Arduino Nano 33 BLE"


// BLE Service
static BLEUUID BLE_UUID_PREDICTION_SERVICE("8AF266CF-D302-4B84-8786-69EC57F92379");

// BLE Characteristics
static BLEUUID BLE_UUID_UNKNOWN("83374BB5-D313-4DD5-8F8A-F1DC7848801F");
static BLEUUID BLE_UUID_STOP("98D096F0-445C-4376-A43C-809AC6BC8D3D");
static BLEUUID BLE_UUID_RIGHT("47791AB8-AE7A-4758-B54C-446551C6D5F3");
static BLEUUID BLE_UUID_LEFT("B1E80FE4-0521-4E0D-8D88-2A94B40FD668");
static BLEUUID BLE_UUID_PEDESTRIAN("E25AC9F4-88D0-4C43-A599-9F9516B226BA");


//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress* pServerAddress;

//Characteristicd that we want to read
static BLERemoteCharacteristic* UnknownCharacteristic;
static BLERemoteCharacteristic* StopCharacteristic;
static BLERemoteCharacteristic* RightCharacteristic;
static BLERemoteCharacteristic* LeftCharacteristic;
static BLERemoteCharacteristic* PedestrianCharacteristic;

//Activate notify
const uint8_t notificationOn[] = { 0x1, 0x0 };
const uint8_t notificationOff[] = { 0x0, 0x0 };

//Variables to store temperature and humidity
float UnknownPrediction;
char* StopPrediction;
char* RightPrediction;
char* LeftPrediction;
char* PedestrianPrediction;

//Flags to check whether new temperature and humidity readings are available
boolean newUnknownPrediction = false;
boolean newStopPrediction = false;
boolean newRightPrediction = false;
boolean newLeftPrediction = false;
boolean newPedestrianPrediction = false;

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();

  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(BLE_UUID_PREDICTION_SERVICE);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(BLE_UUID_PREDICTION_SERVICE.toString().c_str());
    return (false);
  }

  // Obtain a reference to the characteristics in the service of the remote BLE server.
  UnknownCharacteristic = pRemoteService->getCharacteristic(BLE_UUID_UNKNOWN);
  StopCharacteristic = pRemoteService->getCharacteristic(BLE_UUID_STOP);
  RightCharacteristic = pRemoteService->getCharacteristic(BLE_UUID_RIGHT);
  LeftCharacteristic = pRemoteService->getCharacteristic(BLE_UUID_LEFT);
  PedestrianCharacteristic = pRemoteService->getCharacteristic(BLE_UUID_PEDESTRIAN);

  if (UnknownCharacteristic == nullptr || StopCharacteristic == nullptr || RightCharacteristic == nullptr || LeftCharacteristic == nullptr || PedestrianCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");

  //Assign callback functions for the Characteristics
  UnknownCharacteristic->registerForNotify(UnknownNotifyCallback);
  StopCharacteristic->registerForNotify(StopNotifyCallback);
  RightCharacteristic->registerForNotify(RightNotifyCallback);
  LeftCharacteristic->registerForNotify(LeftNotifyCallback);
  PedestrianCharacteristic->registerForNotify(PedestrianNotifyCallback);
  return true;
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {                 //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop();                              //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());  //Address of advertiser is the one we need
      doConnect = true;                                                //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};

//When the BLE Server sends a new prediction with the notify property
static void UnknownNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  // UnknownPrediction = (char*)pData;
  // newUnknownPrediction = true;
  UnknownCharacteristic->readValue( &UnknownPrediction, 4 );
}

static void StopNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  StopPrediction = (char*)pData;
  newStopPrediction = true;
}

static void RightNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  RightPrediction = (char*)pData;
  newRightPrediction = true;
}

static void LeftNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  LeftPrediction = (char*)pData;
  newLeftPrediction = true;
}

static void PedestrianNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  PedestrianPrediction = (char*)pData;
  newPedestrianPrediction = true;
}

//function that prints the latest predictions
void printReadings() {
  Serial.print("UnknownPrediction: ");
  Serial.println((UnknownPrediction));
  Serial.print("StopPrediction: ");
  Serial.println(StopPrediction);
  Serial.print("RightPrediction: ");
  Serial.println(RightPrediction);
  Serial.print("LeftPrediction: ");
  Serial.println(LeftPrediction);
  Serial.print("PedestrianPrediction: ");
  Serial.println(PedestrianPrediction);
}

void setup() {

  //Start serial communication
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");

  //Init BLE device
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      UnknownCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      StopCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      RightCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      LeftCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      PedestrianCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);

      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  //if new temperature readings are available, print in the OLED
  if (newUnknownPrediction && newStopPrediction && newRightPrediction && newLeftPrediction && newPedestrianPrediction) {
    newUnknownPrediction = false;
    newStopPrediction = false;
    newRightPrediction = false;
    newLeftPrediction = false;
    newPedestrianPrediction = false;
    printReadings();
  }
  delay(1000);  // Delay a second between loops.
}
