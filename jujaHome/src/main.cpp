#include <Arduino.h>

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns


//we create 2 arrays one with the password and the other, empty

int i=0;

#define pass_length  5

char passcode[pass_length] ="1234";

char confirmPasscode[pass_length] ;



//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};

//definition of pins
byte rowPins[ROWS] = {53, 52, 51, 50}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {49, 48, 47, 46}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad, this comes with the library
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
   char customKey = customKeypad.getKey();
  
  if (customKey){
    confirmPasscode[i]=customKey;
    i++;
   
  }
  if(i == pass_length-1)
  {
      if(!strcmp(passcode,confirmPasscode))
      {
  
      Serial.println("correct");
       Serial.println(confirmPasscode);
       }
    }
}
  
   
  
