#include <Arduino.h>
#include <SoftwareSerial.h>

#include <ArduinoJson.h>
#include "comms.h"
#include "lorasetup.h"
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 10000; 

void setup_sim800l() {
  mySerial.begin(9600);  
  delay(1000);

  // Test communication with SIM800L
  mySerial.println("AT");
  delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  
  // Check signal quality
  mySerial.println("AT+CSQ");
  delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  // Check network registration status
  mySerial.println("AT+CREG?");
  delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  // Attach to GPRS network
  mySerial.println("AT+CGATT?");
  delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  // Configure the APN (replace with your provider's APN if needed)
  mySerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  mySerial.println("AT+CSTT=\"internet\",\"\",\"\"");
  delay(2000);

  // Activate GPRS bearer
  mySerial.println("AT+SAPBR=1,1");
  delay(2000);

  // Check IP address
  mySerial.println("AT+SAPBR=2,1");
  delay(2000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  Serial.println("SIM800L GPRS setup complete.");
}




void setup() {
  Serial.begin(9600);  // Initialize Serial monitor
  setup_sim800l();     // Set up SIM800L for GPRS communication

  SerialAT.begin(GSM_BAUD);        
  
  delay(3000); 
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
}

void loop() { 
  if (!mqttClient.connected()) {
    reconnect();
  }
  
  mqttClient.loop();

  String od = "IUG214";
  String lat = "-1.203010";
  String lon = "36.917643";

  if (millis() - lastPublishTime >= publishInterval) {
    lastPublishTime = millis();
    String status = gsmMqtt(od, lat, lon);  // Pass keys and values
    Serial.println("Publish status: " + status);
  }
}

//// gateway
#include <LoRa.h>
#include <FreeRTOS.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define GATEWAY_ADDR 255
#define PEER_1_ADDR 1
#define PEER_2_ADDR 2

// Task to receive data from peers and forward it to the gateway
void Task_Gateway(void *pvParameters) {
  while (true) {
    if (LoRa.available()) {
      String data = LoRa.readString();
      byte senderAddress = LoRa.read();  // Read the sender's address from LoRa packet

      // Process the data based on sender address
      if (senderAddress == PEER_1_ADDR) {
        Serial.println("Gateway received data from Peer 1: " + data);
      } else if (senderAddress == PEER_2_ADDR) {
        Serial.println("Gateway received data from Peer 2: " + data);
      } else {
        Serial.println("Gateway received data from an unknown peer");
      }

      // Perform additional processing or forwarding logic as needed
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Task delay to allow other tasks to run
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);  // Loop forever if LoRa fails to initialize
  }

  // Create FreeRTOS task for receiving data from peers
  xTaskCreate(Task_Gateway, "Task_Gateway", 2048, NULL, 1, NULL);
}

void loop() {
  // Main loop is empty because FreeRTOS handles tasks asynchronously
}
/////node 1

#include <LoRa.h>
#include <FreeRTOS.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define PEER_1_ADDR 1
#define GATEWAY_ADDR 255
#define PEER_2_ADDR 2

// Task to send data to the gateway or forward data to Peer 2 if out of range
void Task_Peer_1(void *pvParameters) {
  while (true) {
    // Send data to the gateway if in range
    LoRa.beginPacket();
    LoRa.write(GATEWAY_ADDR);  // Set the destination address as Gateway
    LoRa.print("Data from Peer 1");
    LoRa.endPacket();
    Serial.println("Peer 1 sent data to Gateway.");

    // Alternatively, you can forward data if needed based on RSSI or distance
    int rssi = LoRa.packetRssi();
    if (rssi < -100) {  // Check if the signal is weak (threshold for out of range)
      LoRa.beginPacket();
      LoRa.write(PEER_2_ADDR);  // Forward data to Peer 2
      LoRa.print("Forwarded data from Peer 1 to Peer 2");
      LoRa.endPacket();
      Serial.println("Peer 1 forwarded data to Peer 2 due to weak RSSI.");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Task delay to simulate periodic sending
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);  // Loop forever if LoRa fails to initialize
  }

  // Create FreeRTOS task for sending data to the gateway or Peer 2
  xTaskCreate(Task_Peer_1, "Task_Peer_1", 2048, NULL, 1, NULL);
}

void loop() {
  // Main loop is empty because FreeRTOS handles tasks asynchronously
}
//// node 2

#include <LoRa.h>
#include <FreeRTOS.h>

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
  Serial.begin(115200);
  
  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);  // Loop forever if LoRa fails to initialize
  }

  // Create FreeRTOS task for receiving data from Peer 1 and forwarding it to the Gateway
  xTaskCreate(Task_Peer_2, "Task_Peer_2", 2048, NULL, 1, NULL);
}

void loop() {
  // Main loop is empty because FreeRTOS handles tasks asynchronously
}
