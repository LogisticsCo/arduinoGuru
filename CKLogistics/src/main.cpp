#include "lorasetup.h"
#include <Arduino.h>

const char *APN = "safaricom"; 
const char *HTTP_SERVER = "https://cklogisticsback.onrender.com/backend/coordinates/"; 

HardwareSerial mySerial(1);  

String id, Data, Data1, Data2;

void setup_sim800l() {
  mySerial.begin(9600, SERIAL_8N1, 26, 27);  
  delay(1000);
  mySerial.println("AT");
  delay(1000);
  mySerial.println("AT+CSQ");  
  delay(1000);
  mySerial.println("AT+CGATT=1");  
  delay(1000);
  mySerial.println("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"");  
  delay(1000);
  mySerial.println("AT+CGACT=1,1");  
  delay(2000);
  Serial.println("setup complete for gsm");
}

void sendDataToHTTP(const String &od, const String &lat, const String &lon) {
  String jsonData = "{\"od\":\"" + od + "\",\"lat\":\"" + lat + "\",\"lon\":\"" + lon + "\"}";

  mySerial.println("AT+HTTPINIT");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"CID\",1");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"URL\",\"" + String(HTTP_SERVER) + "\"");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");  
  delay(1000);
  mySerial.println("AT+HTTPDATA=" + String(jsonData.length()) + ",10000");  
  delay(1000);
  mySerial.println(jsonData);  
  delay(1000);
  mySerial.println("AT+HTTPACTION=1");  
  delay(5000);  

  String response = "";
  while (mySerial.available()) {
    response += (char)mySerial.read();
  }
  
  Serial.print("HTTP response: ");
  Serial.println(response);  
}

void onReceive(int packetSize) {
  String message = "";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  int len_ID = message.indexOf(",");
  int len_OD = message.indexOf(",", len_ID + 1);
  int len_LAT = message.indexOf(",", len_OD + 1);
  int len_LON = message.indexOf("#");

  String id = message.substring(0, len_ID);  
  String od = message.substring(len_ID + 1, len_OD);  
  String lat = message.substring(len_OD + 1, len_LAT);  
  String lon = message.substring(len_LAT + 1, len_LON);  

  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Order ID (od): ");
  Serial.println(od);
  Serial.print("Latitude: ");
  Serial.println(lat);
  Serial.print("Longitude: ");
  Serial.println(lon);

  sendDataToHTTP(od, lat, lon);
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

void setup() {
  setupSerial();
  setup_sim800l();  
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {
  if (runEvery(2500)) {
    
    Serial.println("Sending data to API every 2500ms");
    
    
    sendDataToHTTP("IUG214", "12.3456", "-78.9012"); 
  }
}
