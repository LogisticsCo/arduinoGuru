#include "lorasetup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 14, TXPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
float latitude;
float longitude;
float orderNo;

char buffer[256];

char latBuffer[15];
char lngBuffer[15];

void onReceive(int packetSize)
{
  String message = "";
  while (LoRa.available())
  {
    message += (char)LoRa.read();
  }
  Serial.print("Node Receive: ");
  Serial.println(message);
}
boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}


void setup()
{
  setupSerial();
  ss.begin(GPSBaud);
  testSerial();
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  Serial.println("LoRa Nodes-1");
}

void loop()
{
  
  if (runEvery(5000))
	 {                            
	   String message = "001,";  
     if (gps.encode(ss.read())) {
			latitude = gps.location.lat();
      Serial.println(latitude);
      longitude = gps.location.lng();
      
    
    

		} 
     
      dtostrf(latitude, 10, 6, latBuffer);
      dtostrf(longitude, 10, 6, lngBuffer);

	   message += "OrderNo: ";
     message += "lat:";
     message += latBuffer;
     message += "long:";
     message += lngBuffer;         
	   message += "#";           
	   LoRa_sendMessage(message);
	   Serial.println("Message sent: " + message);
    
	 }
}
