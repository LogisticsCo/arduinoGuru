#include "lorasetup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 14, TXPin = 13;
static const uint32_t GPSBaud = 9600;

float tempRead = 22;
float  humdRead = 43;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);


struct Coordinates {
  float latitude;
  float longitude;
};

Coordinates displayInfo() {
  Coordinates coords = {0.0, 0.0}; // Initialize to 0.0
  
  if (gps.location.isValid()) {
    coords.latitude = gps.location.lat();
    coords.longitude = gps.location.lng();
    
    char latBuffer[15];
    char lngBuffer[15];
    
    
    dtostrf(coords.latitude, 10, 6, latBuffer);
    dtostrf(coords.longitude, 10, 6, lngBuffer);
    
    
    Serial.print(F("Latitude: "));
    Serial.println(latBuffer);
    Serial.print(F("Longitude: "));
    Serial.println(lngBuffer);
  } else {
    Serial.println(F("Location: INVALID"));
  }
  
  return coords; 
}

char buffer[256];

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

void DHT_config()
{
  tempRead = 22;
  humdRead = 43;

  if (isnan(tempRead) || isnan(humdRead))
  {
    return;
  }
  JsonDocument doc;
  JsonArray data = doc["DHT"].to<JsonArray>();
  data.add(tempRead);
  data.add(humdRead);

  serializeJson(doc, buffer);
  serializeJsonPretty(doc, Serial);
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
  if (runEvery(2500))
	 {                            
	   String message = "001,";   
	   message += Coordinates;         
	   message += "#";           
	   LoRa_sendMessage(message);
	   Serial.println("Message sent: " + message);
	 }
}


void loop() {
	// This sketch displays information every time a new sentence is correctly encoded.
	while (ss.available() > 0) {
		if (gps.encode(ss.read())) {
			displayInfo();

		}
	}
	
	if (millis() > 5000 && gps.charsProcessed() < 10) {
		Serial.println(F("No GPS detected: check wiring."));
		while(true);
	}
}
