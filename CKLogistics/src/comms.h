#define TINY_GSM_MODEM_SIM800     
#define GSM_BAUD 9600   
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#define Serial Serial 

SoftwareSerial mySerial(14,13);           
#define SerialAT mySerial           


const char* mqttServer = "cklogistics.cloud.shiftr.io";
const int mqttPort = 1883;
const char* mqttUser = "logistics";        
const char* mqttPassword = "uZlp3Dbsk6wYqWGe";    

const char* topic = "cklogistics/data";


TinyGsm modem(SerialAT);           
TinyGsmClient client(modem);
PubSubClient mqttClient(client);


const char* apn = "safaricom";     
const char* gprsUser = "";          
const char* gprsPass = "";          


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    
    
    if (mqttClient.connect("arduino", "cklogistics", "public")) {
      Serial.println("Connected to MQTT");
      mqttClient.subscribe(topic);  
    } else {
      // If connection fails, print the error and retry
      Serial.print("Failed to connect, return code: ");
      Serial.println(mqttClient.state());  
      delay(5000);  
    }
  }
}

String gsmMqtt(String jsonData) {
  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();

  // Publish the JSON-formatted string
  if (mqttClient.publish(topic, jsonData.c_str())) {
    Serial.println("JSON message published successfully: " + jsonData);
    return "Success";
  } else {
    Serial.println("Failed to publish JSON message: " + jsonData);
    return "Fail";
  }
}
