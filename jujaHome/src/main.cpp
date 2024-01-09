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
int fan =22;
int bedSide = 23;
int tv = 24;
int officeUp = 25;  //officeDown
int lappy = 26;
int doorUp = 27;   // bedUnder
int irchair=28;
int tvState=0;
int lappystate=0;

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
  pinMode(fan,OUTPUT);
  pinMode(bedSide,OUTPUT);
  pinMode(tv,OUTPUT);
  pinMode(lappy,OUTPUT);
  pinMode(doorUp,OUTPUT);
  pinMode(officeUp,OUTPUT);
  pinMode(irchair,INPUT);
  
  digitalWrite(fan,HIGH);
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
              
              digitalWrite(officeUp,HIGH); // better to use the relay for status than the led
              delay(2000);
              digitalWrite(officeUp,LOW);
              
            
           }

        else{
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




  
//void passwordChecker(){
  
//}

void chairChecker(){
int state= digitalRead(irchair);
tvState = digitalRead(tv);
lappystate=digitalRead(lappy);
int officestate=digitalRead(officeUp);
int doorstate=digitalRead(doorUp);
int bedstate= digitalRead(bedSide);


if(state ==1 && tvState==0 && lappystate==0 && officestate ==0 && doorstate==0 && bedstate==0  )
  {
      digitalWrite(lappy,LOW);
      digitalWrite(tv,LOW);
      digitalWrite(officeUp,LOW);
      digitalWrite(bedSide,LOW);
      digitalWrite(doorUp,LOW);
  }
if (state==0 && tvState==0 && lappystate==0 && officestate ==0 && doorstate==0 && bedstate==0 )

    {
     
    }
if(state==0 && (tvState==1 || lappystate==1 || officestate ==1 || doorstate==1 || bedstate==1))
{


}
} 

void loop(){

Serial.println("here now");
}