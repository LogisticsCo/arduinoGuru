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

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define PEER_1_ADDR 1
#define GATEWAY_ADDR 255
#define PEER_2_ADDR 2

// Shared variables for GPS data
volatile double latitude = 0.0;
volatile double longitude = 0.0;

// Mutex for accessing GPS data safely
SemaphoreHandle_t gpsMutex;

// Initialize the I2C LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
const int button1Pin = 3;
const int button2Pin = 4;
const int button3Pin = 5;

// Shared variables for button task
volatile bool systemInitialized = false;
volatile char selectedString[7] = "";
volatile int charIndex = 0;

// Character set for selection
const char charSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
volatile int charSetIndex = 0;

// Task to handle GPS data retrieval
void Task_GPS(void *pvParameters) {
  while (true) {
    while (ss.available() > 0) {
      gps.encode(ss.read());
      
      // Check if GPS data is valid
      if (gps.location.isUpdated()) {
        if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE) {
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          xSemaphoreGive(gpsMutex);
        }
        Serial.print("GPS Updated: Latitude = ");
        Serial.print(latitude, 6);
        Serial.print(", Longitude = ");
        Serial.println(longitude, 6);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Polling delay
  }
}

volatile bool stringConfirmed = false;

// Update the Task_Buttons to confirm the string
void Task_Buttons(void *pvParameters) {
  while (true) {
    if (digitalRead(button1Pin) == HIGH) {
      // Reset the system
      systemInitialized = true;
      charIndex = 0;
      stringConfirmed = false;
      memset((char *)selectedString, '\0', sizeof(selectedString));  // Clear string
      Serial.println("System Initialized.");
      vTaskDelay(200 / portTICK_PERIOD_MS);  // Debounce
    }

    if (systemInitialized && digitalRead(button2Pin) == HIGH) {
      charSetIndex = (charSetIndex + 1) % sizeof(charSet);
      selectedString[charIndex] = charSet[charSetIndex];
      Serial.print("Selected character: ");
      Serial.println(selectedString[charIndex]);
      vTaskDelay(200 / portTICK_PERIOD_MS);  // Debounce
    }

    if (systemInitialized && digitalRead(button3Pin) == HIGH) {
      charIndex++;
      if (charIndex >= 6) {
        charIndex = 0;  // Reset after 6 characters
      }
      selectedString[charIndex] = '\0';  // Null-terminate the string
      Serial.println("Character confirmed, moving to next.");

      // If the string is fully selected, confirm it
      if (charIndex == 0) {
        stringConfirmed = true;
        Serial.print("String confirmed: ");
        Serial.println((const char *)selectedString);  // Updated line
      }
      vTaskDelay(200 / portTICK_PERIOD_MS);  // Debounce
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);  // Polling delay
  }
}

// Update the Task_Peer_1 to send the confirmed string
void Task_Peer_1(void *pvParameters) {
  while (true) {
    if (stringConfirmed) {
      // Send the confirmed string to the gateway
      String message = String("Selected String: ") + (const char*)selectedString;


      LoRa.beginPacket();
      LoRa.write(GATEWAY_ADDR);
      LoRa.print(message);
      LoRa.endPacket();
      Serial.println("Peer 1 sent data to Gateway: " + message);

      // Check RSSI and forward to Peer 2 if necessary
      int rssi = LoRa.packetRssi();
      if (rssi < -100) {
        LoRa.beginPacket();
        LoRa.write(PEER_2_ADDR);
        LoRa.print("Forwarded data from Peer 1 to Peer 2: " + message);
        LoRa.endPacket();
        Serial.println("Peer 1 forwarded data to Peer 2 due to weak RSSI.");
      }

      vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay between transmissions
    } else {
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Check periodically
    }
  }
}

// Task to handle LCD display
void Task_LCD(void *pvParameters) {
  while (true) {
    lcd.clear();
    if (!systemInitialized) {
      lcd.print("Press Button 1");
    } else {
      lcd.print("String: ");
      lcd.setCursor(0, 1);
      lcd.print((const char *)selectedString);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Refresh every 500 ms
  }
}

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  lcd.init();  // Initialize the I2C LCD
  lcd.backlight();  // Turn on the LCD backlight

  // Initialize buttons
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);

  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Create a mutex for GPS data
  gpsMutex = xSemaphoreCreateMutex();
  if (gpsMutex == NULL) {
    Serial.println("Failed to create GPS mutex!");
    while (1);
  }

  // Create FreeRTOS tasks
  xTaskCreate(Task_GPS, "Task_GPS", 2048, NULL, 1, NULL);
  xTaskCreate(Task_Peer_1, "Task_Peer_1", 2048, NULL, 1, NULL);
  xTaskCreate(Task_LCD, "Task_LCD", 1024, NULL, 1, NULL);
  xTaskCreate(Task_Buttons, "Task_Buttons", 1024, NULL, 1, NULL);
}

void loop() {
  // Nothing to do here, tasks handle the workload
}
