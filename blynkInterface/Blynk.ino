//Blynk auth credentials: 
//Should be placed before headers
#define BLYNK_TEMPLATE_ID           "TMPLBotx3_ZT"
#define BLYNK_DEVICE_NAME           "Car controls"
#define BLYNK_AUTH_TOKEN            "f1cGTooC6I4YG9aXzC_tDHCHaF_oC-zW"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

//Networking credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "SSID";       //SHOULD BE CHANGED 
char pass[] = "PASSWORD";   //SAME HER TOO 

//Pinouts 
#define LED_BUILTIN 2
#define tempPIN 33
#define dhtPIN 25
#define offsetValue 2  

BlynkTimer timer;
DHT dht(dhtPIN, DHT11); 

// Updates every 1S
void myTimerEvent(){
  Blynk.virtualWrite(V0, digitalRead(LED_BUILTIN));               //(INT) inbuild Led state. 
 // Blynk.virtualWrite(V1, xxx);                                    //(INT) Front Ultrasensor value on xxx
  //Blynk.virtualWrite(V2, xxx);                                    //(INT) Right Ultrasensor value    
  //Blynk.virtualWrite(V3, xxx);                                    //(INT) Left Ultrasenor value 
  Blynk.virtualWrite(V4, tempData());                                    //(Float) Battery temp
 //Blynk.virtualWrite(V5, xxx);                                    //(String) AI prediction 
  //Blynk.virtualWrite(V6, xxx);                                  //Right motor LED if needed 
 //Blynk.virtualWrite(V7, xxx);                                   //Left motor LED if needed 
 Blynk.virtualWrite(V8,dht.readTemperature() );
 
}

void setup(){
 
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  //Function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  dht.begin(); //DHT sensor maybe usless

}

void loop(){
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  digitalWrite(LED_BUILTIN, HIGH); 
}


float tempData(){
  
  int inputValue = analogRead(tempPIN);            //Reads the value of potentiometer
  float milliVolt = inputValue * (3300.0 / 4096.0);   //Calculates the corressponding milli Voltage range from 0 - 5V
  float temp = milliVolt / 10;                   //Datasheet specifies that LM35 reads 10mv/°C and adds to the value before.
  temp = temp *offsetValue; 

  /*Debuging thing for temp 
  Serial.println("......................");
  Serial.println(" "); 
  Serial.println("Reading: " + (String)inputValue); 
  Serial.println("volt: " + (String)milliVolt); 
  Serial.println("Temp: " + (String)temp); 
  Serial.println("Temp DHT: " + (String)dht.readTemperature());
  Serial.println(" "); 
  Serial.println("......................");
  */

  return temp; 
}
