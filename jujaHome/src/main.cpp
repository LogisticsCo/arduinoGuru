#include <Arduino.h>

#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display



const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns


//we create 2 arrays one with the password and the other, empty

int i=0;

#define pass_length  5

char passcode[pass_length] ="1234";

char confirmPasscode[pass_length]="" ;



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
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("hello world");
  lcd.setCursor(1,1);
  lcd.print("initializing ..");
  lcd.clear();
  Serial.begin(9600);
}



void reset(){
  while (i !=0){
    i =0;
  }
  return;
}
  
void passwordChecker(){
  lcd.setCursor(0,0);
       lcd.print("Enter passcode :");
   char customKey = customKeypad.getKey();
  
  if (customKey){
    
    lcd.setCursor(i,1);
       lcd.print("*");
    confirmPasscode[i]=customKey;
    
   i++;
  }
  if(i == pass_length-1)
  {
      if(!strcmp(passcode,confirmPasscode))
      {
       lcd.clear();
       lcd.print("correct");
       
       delay(5000);
       
       reset();

       }
       else{lcd.clear();
        lcd.print("incorrect");
        
        delay(3000); 
         reset();
       }
    }
}

void loop(){
passwordChecker();
}
   
  
