#include <Arduino.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "comms.h"
#include "lorasetup.h"
#include <LoRa.h>

// LoRa module configuration
#define LORA_SS 15
#define LORA_RST 12
#define LORA_DIO0 4

#define PEER_1_ADDR 2
#define PEER_2_ADDR 1
#define GATEWAY_ADDR 255

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 10000;  // Publish interval in milliseconds

String node = "";  // Variable to identify the sender node
String od = "";    // Variable to store the selected string
String lat = "0.000000";  // Default latitude
String lon = "0.000000";  // Default longitude
void parseMessage(int sender, const String& message, int rssi) {
    int selectedStringIndex = message.indexOf("Selected String: ");
    int latIndex = message.indexOf(", Lat: ");
    int lonIndex = message.indexOf(", Lon: ");

    // Validate and extract data from the message
    if (selectedStringIndex != -1 && latIndex != -1 && lonIndex != -1) {
        node = String(sender);  // Assign the sender's address as the node identifier
        od = message.substring(selectedStringIndex + 17, latIndex);
        lat = message.substring(latIndex + 7, lonIndex);
        lon = message.substring(lonIndex + 7);

        Serial.printf("Parsed values - Node: %s, od: %s, lat: %s, lon: %s\n", node.c_str(), od.c_str(), lat.c_str(), lon.c_str());
    } else {
        Serial.println("Failed to parse message.");
    }
}

void publishToMQTT() {
    // Create a JSON object
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["node"] = node;
    jsonDoc["od"] = od;
    jsonDoc["lat"] = lat;
    jsonDoc["lon"] = lon;

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    String status = gsmMqtt(jsonData);  
    Serial.println("Publish status: " + status);
}
void setup() {
    Serial.begin(9600);  // Initialize Serial monitor

    // Initialize LoRa
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    Serial.println("LoRa initialized successfully!");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        int sender = LoRa.read();
        byte receiver = LoRa.read();
        String incomingMessage = "";

        // Read the incoming message
        while (LoRa.available()) {
            incomingMessage += (char)LoRa.read();
        }

        int rssi = abs(LoRa.packetRssi());  
        Serial.printf("Received from address %d (RSSI: %d): %s\n", sender, rssi, incomingMessage.c_str());

        // Process message only if it is addressed to this gateway
        if (receiver == GATEWAY_ADDR) {
            parseMessage(sender, incomingMessage, rssi);
        }
    }

    // Publish data to MQTT at regular intervals
    if (millis() - lastPublishTime >= publishInterval) {
        lastPublishTime = millis();
        publishToMQTT();
    }
}

