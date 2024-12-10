#include "lorasetup.h"
#include <Arduino.h>


const char *APN = "safaricom"; 
const char *HTTP_SERVER = "https://cklogistics-h9bxfpgsaqf3duab.canadacentral-01.azurewebsites.net/backend/coordinates/";  // Replace with your server's URL

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
  mySerial.println("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"");  // Set APN for the GPRS connection
  delay(1000);
  mySerial.println("AT+CGACT=1,1");  
  delay(2000);
  Serial.println("setup complete for gsm");
}

void sendDataToHTTP(const String &id, const String &data) {
  String postData = "id=" + id + "&data=" + data;
  
  mySerial.println("AT+HTTPINIT");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"CID\",1");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"URL\",\"" + String(HTTP_SERVER) + "\"");  
  delay(1000);
  mySerial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");  
  delay(1000);
  mySerial.println("AT+HTTPDATA=" + String(postData.length()) + ",10000");  
  delay(1000);
  mySerial.println(postData);  
  delay(1000);
  mySerial.println("AT+HTTPACTION=1"); 
  delay(1000);

  String response = "";
  while (mySerial.available()) {
    response += (char)mySerial.read();
  }
  
  Serial.print("HTTP response: ");
  Serial.println(response);  
}

void onReceive(int packetSize) {
  int len_ID, len_DT;
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  len_ID = message.indexOf(",");
  len_DT = message.indexOf("#");

  id = message.substring(0, len_ID);  
  Data = message.substring(len_ID + 1, len_DT); 

  if (id == "001") {
    Data1 = Data;
    Serial.print("001, ");
    Serial.println(Data1);
    sendDataToHTTP(id, Data1);  
  } else if (id == "002") {
    Data2 = Data;
    Serial.print("002, ");
    Serial.println(Data2);
    sendDataToHTTP(id, Data2);  
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

void setup() {
  setupSerial();
  setup_sim800l();  
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {
  
  if (runEvery(2500)) {
    Serial.println("LoRa-Gateway:01, Site->Cklogistics");
    Serial.print("01: ");
    Serial.println(Data1);
    Serial.print("02: ");
    Serial.println(Data2);
  }
}
