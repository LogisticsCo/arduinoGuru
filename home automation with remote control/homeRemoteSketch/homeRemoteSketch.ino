#include <IRremote.h>
IRrecv IR(13);
int office=22;
int kitchen=24;
int lappy=26;
int tv= 28;


void setup() {
IR.enableIRIn();
pinMode(office,OUTPUT);
pinMode(kitchen,OUTPUT);
pinMode(lappy,OUTPUT);
pinMode(tv,OUTPUT);


Serial.begin(9600);

}

void loop() {
  if(IR.decode()){
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    if(IR.decodedIRData.decodedRawData==OxEA157FOO){
      //ON  


    }
    if(IR.decodedIRData.decodedRawData==OxFE017FOO){
//1
    }
    if(IR.decodedIRData.decodedRawData==OxFD027FOO){
//2
    }
    if(IR.decodedIRData.decodedRawData==OxFC037FOO){
//3
    }
    if(IR.decodedIRData.decodedRawData==OxFB047FOO){
//4
    }
    if(IR.decodedIRData.decodedRawData==OxFA057FOO){
//5
    }
    if(IR.decodedIRData.decodedRawData==OxF9067FOO){
//6
    }
    if(IR.decodedIRData.decodedRawData==OxF8077FOO){
//7
    }
    if(IR.decodedIRData.decodedRawData==OxEF107FOO){
//OFF
    }
    if(IR.decodedIRData.decodedRawData==OxF7087FOO){
//8
    }
    if(IR.decodedIRData.decodedRawData==OxF6097FOO){
//9
    }
  delay(1000);
  IR.resume();
  }
  
  // put your main code here, to run repeatedly:

}
