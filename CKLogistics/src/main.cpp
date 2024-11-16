#include "lorasetup.h"
#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>

const char *SSID = "KASRI";
const char *PASS = "@Freezy#254";
const char *MQTT_SERVER = "cklogistics.cloud.shiftr.io";
const char *MQTT_TOPIC = "Logistics/lorawan/gps";

const char *MQTT_CLIENT_ID = "esp32logistics";
const char *MQTT_USERNAME = "cklogistics";
const char *MQTT_PASSWORD = "public";
 
String id, Data, Data1, Data2;

WiFiClient client;
MQTTPubSubClient mqtt;

void setup_wifi() {
  connect_to_wifi:
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  connect_to_host:
  Serial.print("Connecting to host...");
  client.stop();
  while (!client.connect(MQTT_SERVER, 1883)) {
      Serial.print(".");
      delay(1000);
      if (WiFi.status() != WL_CONNECTED) {
          Serial.println("WiFi disconnected");
          goto connect_to_wifi;
      }
  }
  Serial.println(" connected!");

  Serial.print("Connecting to MQTT broker...");
  mqtt.disconnect();
  while (!mqtt.connect("arduino", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.print(".");
      delay(1000);
      if (WiFi.status() != WL_CONNECTED) {
          Serial.println("WiFi disconnected");
          goto connect_to_wifi;
      }
      if (client.connected() != 1) {
          Serial.println("WiFiClient disconnected");
          goto connect_to_host;
      }
  }
  Serial.println(" connected!");
}

// Function to publish received data to MQTT
void sendToMQTT(const String &id, const String &data) {
  String topic = String(MQTT_TOPIC) + "/" + id;
  if (mqtt.publish(topic.c_str(), data.c_str())) {
      Serial.print("Data sent to MQTT: ");
      Serial.print(topic);
      Serial.print(" -> ");
      Serial.println(data);
  } else {
      Serial.println("Failed to send data to MQTT.");
  }
}

void onReceive(int packetSize) {
  int len_ID, len_DT;
  String message = "";

  while (LoRa.available()) {
      message += (char)LoRa.read();
  }

  len_ID = message.indexOf(",");
  len_DT = message.indexOf("#");

  id = message.substring(0, len_ID);          // parsing id
  Data = message.substring(len_ID + 1, len_DT); // parsing data

  if (id == "001") {
      Data1 = Data;
      Serial.print("001, ");
      Serial.println(Data1);
      sendToMQTT(id, Data1); // Send to MQTT
      Serial.print("RSSI: ");
      Serial.println(LoRa.packetRssi());
  } else if (id == "002") {
      Data2 = Data;
      Serial.print("002, ");
      Serial.println(Data2);
      sendToMQTT(id, Data2); // Send to MQTT
      Serial.print("RSSI: ");
      Serial.println(LoRa.packetRssi());
  }
}

boolean runEvery(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      return true;
  }
  return false;
}

void reconnect() {
  WiFi.begin(SSID, PASS);
  mqtt.begin(client);
  setup_wifi();
}

void setup() {
  setupSerial();
  WiFi.begin(SSID, PASS);
  mqtt.begin(client);
  setup_wifi();

  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
      else // U_SPIFFS
          type = "filesystem";

      Serial.println("Start updating " + type);
  }).onEnd([]() {
      Serial.println("\nEnd");
  }).onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  }).onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  client.connect(MQTT_SERVER, 1883);
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connect(MQTT_SERVER, 1883)) {
      reconnect();
  }
  if (runEvery(2500)) {
      Serial.println("LoRa-Gateway:01, Site->Cklogistics");
      Serial.print("01: ");
      Serial.println(Data1);
      Serial.print("02: ");
      Serial.println(Data2);
  }
}
