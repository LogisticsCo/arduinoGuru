
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LoRa.h>
#include "lorasetup.h"

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define PEER_2_ADDR 2
#define PEER_1_ADDR 1
#define GATEWAY_ADDR 255

// Task to receive data from Peer 1 and forward to the Gateway if needed
void Task_Peer_2(void *pvParameters) {
  while (true) {
    if (LoRa.available()) {
      String data = LoRa.readString();
      byte senderAddress = LoRa.read();  // Read sender address from LoRa packet
      
      // If data is received from Peer 1, forward it to the gateway
      if (senderAddress == PEER_1_ADDR) {
        LoRa.beginPacket();
        LoRa.write(GATEWAY_ADDR);  // Forward to Gateway
        LoRa.print(data);
        LoRa.endPacket();
        Serial.println("Peer 2 forwarded data from Peer 1 to Gateway.");
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Task delay to allow other tasks to run
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

  
  xTaskCreate(Task_Peer_2, "Task_Peer_2", 2048, NULL, 1, NULL);
}

void loop() {
}
