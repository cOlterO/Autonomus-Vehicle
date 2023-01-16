/*
  Blynk console for car control

  NOTE:BlynkTimer provides SimpleTimer functionality:
  http://playground.arduino.cc/Code/SimpleTimer

*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"


//Blynk auth details: 

#define BLYNK_TEMPLATE_ID "TMPLBotx3_ZT"
#define BLYNK_DEVICE_NAME "Car controls"
#define BLYNK_AUTH_TOKEN  "f1cGTooC6I4YG9aXzC_tDHCHaF_oC-zW"

#define BLYNK_PRINT Serial                        // Comment this out to disable prints and save space

char auth[] = BLYNK_AUTH_TOKEN;                   
char ssid[] = "Dontaskmypassword";                  //your SSID              
char pass[] = "ben168001";                     // Password

//Pinouts 
#define LED_BUILTIN 32
#define tempPIN 33
#define dhtPIN 25
#define offsetValue 2  

BlynkTimer timer;
DHT dht(dhtPIN, DHT11); 


void setup(){

  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, blynkData);    //Setup blynkData to be called every 1S

    dht.begin();


}

void loop(){


  digitalWrite(LED_BUILTIN, LOW); 

  Blynk.run();                          // Init Blynk 
  timer.run();                          // Initiates BlynkTimer

}


 //Function will be called every 1s and sends value to the blynk console. 
void blynkData(){
 
  int inputValue = analogRead(tempPIN);            //Reads the value of potentiometer
  float milliVolt = inputValue * (3300.0 / 4096.0);   //Calculates the corressponding milli Voltage range from 0 - 5V
  float temp = milliVolt / 10;                   //Datasheet specifies that LM35 reads 10mv/°C and adds to the value before.
  temp = temp *offsetValue; 

  Serial.println("......................");
  Serial.println(" "); 
  Serial.println("Reading: " + (String)inputValue); 
  Serial.println("volt: " + (String)milliVolt); 
  Serial.println("Temp: " + (String)temp); 
  Serial.println("Temp DHT: " + (String)dht.readTemperature());
  Serial.println(" "); 
  Serial.println("......................");


  Blynk.virtualWrite(V0, digitalRead(LED_BUILTIN));               //(INT) inbuild Led state. 
 // Blynk.virtualWrite(V1, xxx);                                    //(INT) Front Ultrasensor value on xxx
  //Blynk.virtualWrite(V2, xxx);                                    //(INT) Right Ultrasensor value    
  //Blynk.virtualWrite(V3, xxx);                                    //(INT) Left Ultrasenor value 
  Blynk.virtualWrite(V4, temp);                                    //(Float) Battery temp
 //Blynk.virtualWrite(V5, xxx);                                    //(String) AI prediction 
  //Blynk.virtualWrite(V6, xxx);                                  //Right motor LED if needed 
 //Blynk.virtualWrite(V7, xxx);                                   //Left motor LED if needed 

}
