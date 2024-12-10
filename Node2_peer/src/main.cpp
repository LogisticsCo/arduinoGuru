#include "lorasetup.h"
#include <Arduino.h>
#include <ArduinoJson.h>


float tempRead;
float humdRead;

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
  testSerial();
  dht.begin();
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  Serial.println("LoRa Nodes-1");
}

void loop()
{
  DHT_config();
  if (runEvery(2500))
  {                            
    String message = "002,";   
    message += buffer;         
    message += "#";            
    LoRa_sendMessage(message); 
    Serial.println("Message sent: " + message);
  }
}
