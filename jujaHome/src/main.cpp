#include <Arduino.h>

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','f'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};
byte rowPins[ROWS] = {53, 52, 51, 50}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {49, 48, 47, 46}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
}