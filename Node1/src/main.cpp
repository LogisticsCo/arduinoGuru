#include "lorasetup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>

static const int RXPin = 14, TXPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

#define LORA_SS 15
#define LORA_RST 12
#define LORA_DIO0 27

#define PEER_1_ADDR 1
#define GATEWAY_ADDR 255
#define PEER_2_ADDR 2

// Shared variables for GPS data
double latitude = 0.0;
double longitude = 0.0;

// Initialize the I2C LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
const int button1Pin = 4;
const int button2Pin = 38;
const int button3Pin = 5;

// Shared variables for button task
bool systemInitialized = false;
char selectedString[7] = "";  // 6 characters + null terminator
int charIndex = 0;

// Character set for selection
const char charSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
int charSetIndex = 0;

// Declare stringConfirmed
bool stringConfirmed = false;  // Variable to track if the string is confirmed

// Timing variables
unsigned long lastGPSUpdate = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastPeerTransmission = 0;
const unsigned long gpsPollingInterval = 100;  // 100 ms
const unsigned long lcdRefreshInterval = 1000; 
const unsigned long peerTransmissionInterval = 7000;  

void setup() {
    Serial.begin(9600);
    ss.begin(GPSBaud);

    lcd.init();
    lcd.backlight();

    // Initialize buttons
    pinMode(button1Pin, INPUT_PULLUP);
    pinMode(button2Pin, INPUT_PULLUP);
    pinMode(button3Pin, INPUT_PULLUP);

    // Initialize LoRa
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    LoRa.setSyncWord(0x34); // Set a unique sync word to avoid interference
    LoRa.enableCrc();       // Enable CRC for packet integrity

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    Serial.println("LoRa initialized");
}

void loop() {
    // Handle GPS data retrieval
    if (millis() - lastGPSUpdate >= gpsPollingInterval) {
        while (ss.available() > 0) {
            gps.encode(ss.read());

            // Check if GPS data is valid
            if (gps.location.isUpdated()) {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
                Serial.printf("GPS Updated: Latitude = %.6f, Longitude = %.6f\n", latitude, longitude);
            }
        }
        lastGPSUpdate = millis();
    }

    // Handle button inputs for string selection
    if (!stringConfirmed) {  // Only allow selection if the string is not confirmed
        if (digitalRead(button1Pin) == LOW) {  // Active LOW for button press
            // Reset the system
            systemInitialized = true;
            charIndex = 0;
            memset(selectedString, '\0', sizeof(selectedString));  // Clear string
            Serial.println("System Initialized.");

            // Update the LCD immediately
            lcd.clear();
            lcd.print("String:");
            lcd.setCursor(0, 1);
            lcd.print(selectedString);
            delay(200);  // Debounce
        }

        if (systemInitialized) {
            if (digitalRead(button2Pin) == LOW) {
                charSetIndex = (charSetIndex + 1) % (sizeof(charSet) - 1);  // Cycle through the character set
                selectedString[charIndex] = charSet[charSetIndex];
                Serial.printf("Selected character: %c\n", selectedString[charIndex]);

                // Immediately update the LCD with the selected character
                lcd.clear();
                lcd.print("Order:");
                lcd.setCursor(0, 1);
                lcd.print(selectedString);

                delay(1200);  // Debounce
            }

            if (digitalRead(button3Pin) == LOW) {
                charIndex++;
                if (charIndex == 6) {  // String is fully selected
                    selectedString[6] = '\0';  // Null-terminate the string
                    Serial.printf("String confirmed: %s\n", selectedString);
                    stringConfirmed = true;  // Lock the string
                } else {
                    selectedString[charIndex] = '\0';  // Null-terminate at the current index
                    Serial.println("Character confirmed, moving to next.");

                    
                    lcd.clear();
                    lcd.print("Order:");
                    lcd.setCursor(0, 1);
                    lcd.print(selectedString);
                }
                delay(200); 
            }
        }
    }
    
    // Handle LCD display
    if (millis() - lastLCDUpdate >= lcdRefreshInterval) {
        lcd.clear();
        if (stringConfirmed) {
            lcd.print("Confirmed Order");
            lcd.setCursor(0, 1);
            lcd.print(selectedString);
        } else {
            lcd.print(systemInitialized ? "Order: " : "Press Button 1");
            lcd.setCursor(0, 1);
            if (systemInitialized) {
                lcd.print(selectedString);
            }
        }
        lastLCDUpdate = millis();
    }

    
    if (stringConfirmed && (millis() - lastPeerTransmission >= peerTransmissionInterval)) {
        // Prepare latitude and longitude for sending
        String latStr = gps.location.isValid() ? String(latitude, 6) : "0.000000";
        String lonStr = gps.location.isValid() ? String(longitude, 6) : "0.000000";

        String message = "Selected Order: " + String(selectedString) + ", Lat: " + latStr + ", Lon: " + lonStr;


        LoRa.beginPacket();
        LoRa.write(PEER_2_ADDR); 
        LoRa.write(PEER_1_ADDR); 
        LoRa.print( message);
        
        LoRa.endPacket();

        LoRa.beginPacket();
        LoRa.write(PEER_1_ADDR); 
        LoRa.write(GATEWAY_ADDR); 
        LoRa.print( message);
        
        LoRa.endPacket();
            
    }

    // LoRa packet reception
    int packetSize = LoRa.parsePacket();
    if (packetSize) {     
        int recipient = LoRa.read();
        int sender = LoRa.read();
        String incomingMessage = "";
        while (LoRa.available()) {
            incomingMessage += (char)LoRa.read();
        }

        
        int rssi = LoRa.packetRssi();
        
        
        if (recipient == PEER_1_ADDR || recipient == GATEWAY_ADDR) {
            Serial.printf("Received from address %d: %s\n", sender, incomingMessage.c_str());
            Serial.println("");
            Serial.printf("RSSI: %d\n", rssi);
        } else {
            Serial.printf("Received packet for unknown recipient %d, ignoring.\n", recipient);
        }
    }
}

void get_rssigw(){
    String checker = "hello";
    LoRa.beginPacket();
    LoRa.write(PEER_2_ADDR); 
    LoRa.write(GATEWAY_ADDR); 
    LoRa.print(checker);
    LoRa.print( checker.length());
    LoRa.endPacket();
}
void send_data(){
    String checker = "hello";
    LoRa.beginPacket();
    LoRa.write(PEER_2_ADDR); 
    LoRa.write(GATEWAY_ADDR); 
    LoRa.print(checker);
    LoRa.print( checker.length());
    LoRa.endPacket();
}
void receive_rssi(){

}
//LoRa.write(PEER_1_ADDR); 