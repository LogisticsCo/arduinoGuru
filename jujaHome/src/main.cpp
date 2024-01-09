//importation of all our libraries 
#include <Arduino.h>
#include <Keypad.h>
 #include <Wire.h>

//declaration of variables
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
int i=0;
int f=0;  // for shutdown function
#define pass_length  5
#define leng 2  //length to establish off state
char passcode[pass_length] ="1234";
char confirmPasscode[pass_length]="" ;
int resetpin =12;
int fan =22;
int bedSide = 23;
int tv = 24;
int officeUp = 25;  //officeDown
int lappy = 26;
int doorUp = 27;   // bedUnder
int irchair=28;
int tvState=0;
int lappystate=0;
int bedstate =0;
int fanstate=0;
int officestate=0;
int doorstate=0;

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
  
  digitalWrite(resetpin,HIGH);
  pinMode(fan,OUTPUT);
  pinMode(bedSide,OUTPUT);
  pinMode(tv,OUTPUT);
  pinMode(lappy,OUTPUT);
  pinMode(doorUp,OUTPUT);
  pinMode(officeUp,OUTPUT);
  pinMode(irchair,INPUT);
  digitalWrite(fan,HIGH);
  pinMode(resetpin,OUTPUT);
  Serial.begin(9600);
  Serial.println("testphase");

  while (i!=pass_length-1)
  {
   
    char customKey = customKeypad.getKey();
  
      if (customKey)
      {
   
        confirmPasscode[i]=customKey;
        i++; 
      }

      if(i == pass_length-1)
      {
          if(!strcmp(passcode,confirmPasscode))
            {
              Serial.println("correct");
              digitalWrite(officeUp,HIGH); // better to use the relay for status than the led
              delay(2000);
              digitalWrite(officeUp,LOW);
              
              
            
           }

        else{Serial.println("incorrect");
              digitalWrite(officeUp,HIGH); // better to use the relay for status than the led
              delay(1000);
              digitalWrite(officeUp,LOW);
              delay(1000);
              digitalWrite(officeUp,HIGH);
              delay(1000);
              digitalWrite(officeUp,LOW);
              
              i =0;
            }
        }
  }
  
}


void reset(){
    //Serial.println("reset");
    //delay(2000);
    digitalWrite(resetpin,LOW);
}

void loop(){

while (i!=0)
{
  //Serial.println("looping here");
   char customKey = customKeypad.getKey();
    if (customKey)
        {
          switch (customKey)
          {
          case '1':
            Serial.println(customKey);
            digitalWrite(officeUp,HIGH); 
            //delay(2000);
            break;

            case '2':
            Serial.println(customKey);
            digitalWrite(officeUp,LOW);
            
            //delay(2000);
            break;

            case '3':
            Serial.println(customKey);
            digitalWrite(bedSide,HIGH); 
            //delay(2000);
            break;

            case '4':
            Serial.println(customKey);
            digitalWrite(bedSide,LOW); 
            
            //delay(2000);
            break;

            case '5':
            Serial.println(customKey);
            digitalWrite(doorUp,HIGH); 
            //delay(2000);
            break;

            case '6':
            Serial.println(customKey);
            digitalWrite(doorUp,LOW);  
            //delay(2000);
            break;

            case '7':
            Serial.println(customKey);
            digitalWrite(lappy,HIGH);
            break;

            case '8':
            Serial.println(customKey);
            digitalWrite(lappy,LOW);
            //delay(2000);
            break;

            case '9':
            Serial.println(customKey);
            digitalWrite(tv,HIGH);
            //delay(2000);
            break;

            case '0':
            Serial.println(customKey);
            
            break;

            case '*':
            //Serial.println(customKey);
            digitalWrite(officeUp,LOW); 
            digitalWrite(lappy,LOW);
            digitalWrite(tv,LOW);
            digitalWrite(doorUp,LOW);
            digitalWrite(bedSide,LOW); 
            //delay(2000);
            break;

            case 'A':
            //Serial.println(customKey);
            digitalWrite(officeUp,HIGH); 
            digitalWrite(lappy,HIGH);
            digitalWrite(tv,HIGH);
            digitalWrite(doorUp,HIGH);
            //delay(2000);
            break;

            case 'B':
            Serial.println(customKey);
            digitalWrite(tv,LOW);
            //delay(2000);
            break;

            case 'C':
            Serial.println(customKey);
           
            //delay(2000);
            break;

            case 'D':
            Serial.println(customKey);
           
           // delay(2000);
            break;

            case '#':
            Serial.println("resetting");
            reset();Serial.println("resetting");

            break;
          
          }
         
        i++; 
        }
}

}