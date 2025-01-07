#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "comms.h"
#include "lorasetup.h"
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define GATEWAY_ADDR 255
#define PEER_1_ADDR 1
#define PEER_2_ADDR 2

// Queue to store received data from LoRa
QueueHandle_t loraQueue;

void Task_Gateway(void *pvParameters) {
  while (true) {
    if (LoRa.available()) {
      String data = LoRa.readString();
      byte senderAddress = LoRa.read();

      // Prepare message for the queue
      String message;
      if (senderAddress == PEER_1_ADDR) {
        message = "Peer 1: " + data;
        Serial.println("Gateway received data from Peer 1: " + data);
      } else if (senderAddress == PEER_2_ADDR) {
        message = "Peer 2: " + data;
        Serial.println("Gateway received data from Peer 2: " + data);
      } else {
        message = "Unknown Peer: " + data;
        Serial.println("Gateway received data from an unknown peer");
      }

      // Send the message to the queue
      if (xQueueSend(loraQueue, message.c_str(), pdMS_TO_TICKS(100)) != pdPASS) {
        Serial.println("Queue full, message dropped");
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Prevent task from hogging the CPU
  }
}

void Task_MQTT(void *pvParameters) {
  char message[256];

  while (true) {
    // Wait for data from the queue
    if (xQueueReceive(loraQueue, &message, portMAX_DELAY)) {
      Serial.println("MQTT Task: Sending data via MQTT -> " + String(message));

      // Publish the message via MQTT
      if (!mqttClient.connected()) {
        reconnect();
      }

      mqttClient.loop();

      if (mqttClient.publish(topic, message)) {
        Serial.println("MQTT: Message published successfully -> " + String(message));
      } else {
        Serial.println("MQTT: Failed to publish message -> " + String(message));
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Initialize GSM and MQTT
  SerialAT.begin(GSM_BAUD);
  delay(1000);
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info: " + modemInfo);
  Serial.println("Connecting to the GSM network...");
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("Failed to connect to GPRS.");
    return;
  }
  Serial.println("Connected to GPRS.");

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  loraQueue = xQueueCreate(10, 256);  
  if (loraQueue == NULL) {
    Serial.println("Error creating the queue!");
    while (1);
  }

  
  xTaskCreate(Task_Gateway, "Task_Gateway", 2048, NULL, 1, NULL);
  xTaskCreate(Task_MQTT, "Task_MQTT", 2048, NULL, 1, NULL);
}

void loop() {
  // The loop is not used since tasks handle the workload
}
