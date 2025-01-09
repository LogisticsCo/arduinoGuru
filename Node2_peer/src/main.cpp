#include <Arduino.h>
#include <LoRa.h>

#define LORA_SS 15
#define LORA_RST 12
#define LORA_DIO0 27

#define PEER_1_ADDR 2
#define PEER_2_ADDR 1
#define GATEWAY_ADDR 255

// Function to validate the address
bool isValidAddress(byte senderAddress) {
  return (senderAddress == PEER_1_ADDR|| senderAddress == GATEWAY_ADDR);
}

// Task to receive data from Peer 1 and forward to the Gateway
void Task_Peer_2(void *pvParameters) {
  while (true) {
    if (LoRa.parsePacket()) {  // Check if a packet is available
      byte senderAddress = LoRa.read();  // First byte is the sender address
      
      // Validate the sender address
      if (!isValidAddress(senderAddress)) {
        Serial.print("Invalid sender address: ");
        Serial.println(senderAddress);
        while (LoRa.available()) LoRa.read();  // Clear the rest of the packet
        continue;
      }

      String data = "";  // Initialize payload string

      // Read the payload from the LoRa packet
      while (LoRa.available()) {
        data += (char)LoRa.read();
      }
        
      int rsi = LoRa.packetRssi();
      Serial.println(rsi);
      Serial.print("Received from address ");
      Serial.print(senderAddress);
      Serial.print(": ");
      Serial.println(data);
      rsi=abs(rsi);
      

      if (senderAddress == PEER_1_ADDR && rsi>=80) {
        LoRa.beginPacket();
        LoRa.write(GATEWAY_ADDR);  
        LoRa.write(PEER_2_ADDR);  
        LoRa.print(data);          
        LoRa.endPacket();
        Serial.println("Forwarded to Gateway.");
      } else {
        Serial.println("Packet received but not forwarded ).");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Task delay for stability
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

  Serial.println("LoRa initialized.");
  xTaskCreate(Task_Peer_2, "Task_Peer_2", 2048, NULL, 1, NULL);  // Create the LoRa handling task
}

void loop() {

}
