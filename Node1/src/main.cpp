#include "lorasetup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 14, TXPin = 13;
static const uint32_t GPSBaud = 9600;


TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);



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
  {                            // repeat every 1000 millis
    String message = "001,";   // id
    message += buffer;         // kirim data millis,
    message += "#";            // tanda akhir data
    LoRa_sendMessage(message); // send a message
    Serial.println("Message sent: " + message);
  }
}
void displayInfo() {
	Serial.print(F("Location: ")); 
	if (gps.location.isValid()) {
		Serial.print(gps.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps.location.lng(), 6);
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gps.date.isValid()) {
		Serial.print(gps.date.month());
		Serial.print(F("/"));
		Serial.print(gps.date.day());
		Serial.print(F("/"));
		Serial.print(gps.date.year());
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gps.time.isValid()) {
		if (gps.time.hour() < 10) Serial.print(F("0"));
		Serial.print(gps.time.hour());
		Serial.print(F(":"));
		if (gps.time.minute() < 10) Serial.print(F("0"));
		Serial.print(gps.time.minute());
		Serial.print(F(":"));
		if (gps.time.second() < 10) Serial.print(F("0"));
		Serial.print(gps.time.second());
		Serial.print(F("."));
		if (gps.time.centisecond() < 10) Serial.print(F("0"));
		Serial.print(gps.time.centisecond());
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.println();
}