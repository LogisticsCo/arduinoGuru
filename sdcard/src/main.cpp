#include <Arduino.h>

#include <FS.h>
#include <SD.h>
#include <SPI.h>

int CS_PIN = 5;  // Define the correct CS pin

File file;


int createFile(const char* filename)
{
  file = SD.open(filename, FILE_WRITE);
  if (file) {
    Serial.println("File created successfully.");
    return 1;
  } else {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(const char* text)
{
  if (file) {
    file.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile()
{
  if (file) {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(const char* filename)
{
  file = SD.open(filename);
  if (file) {
    Serial.println("File opened with success!");
    return 1;
  } else {
    Serial.println("Error opening file...");
    return 0;
  }
}

String readLine()
{
  String received = "";
  char ch;
  while (file.available()) {
    ch = file.read();
    if (ch == '\n') {
      return received;
    } else {
      received += ch;
    }
  }
  return "";
}
void setup()
{
  Serial.begin(9600);

  if (!SD.begin(CS_PIN)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  createFile("/test.txt");
  writeToFile("This is sample text!");
  closeFile();

  openFile("/prefs.txt");
  Serial.println(readLine());
  Serial.println(readLine());
  closeFile();
}

void loop() {
}
