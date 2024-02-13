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
count=0;
while (count<4){
  Serial.println("starting up");
}


}

void loop() {
  if(IR.decode()){
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    if(IR.decodedIRData.decodedRawData==OxEA157FOO){
      //OFF  
      digitalWrite(office,LOW);
      digitalWrite(tv,LOW);
      digitalWrite(lappy,LOW);
      digitalWrite(kitchen,LOW);


    }
    if(IR.decodedIRData.decodedRawData==OxFE017FOO){
    //1
    digitalWrite(office,HIGH);

    }
    if(IR.decodedIRData.decodedRawData==OxFD027FOO){
    //2
    digitalWrite(office,LOW);
    }
    if(IR.decodedIRData.decodedRawData==OxFC037FOO){
    //3
    digitalWrite(tv,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==OxFB047FOO){
    //4
    digitalWrite(tv,LOW);
    }
    if(IR.decodedIRData.decodedRawData==OxFA057FOO){
    //5
    digitalWrite(lappy,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==OxF9067FOO){
    //6
    digitalWrite(lappy,LOW);
    }
    if(IR.decodedIRData.decodedRawData==OxF8077FOO){
    //7
    digitalWrite(kitchen,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==OxEF107FOO){
    //ON
    digitalWrite(office,HIGH);
    digitalWrite(tv,HIGH);
    digitalWrite(lappy,HIGH);
    digitalWrite(kitchen,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==OxF7087FOO){
    //8
    digitalWrite(kitchen,LOW);
    }
    if(IR.decodedIRData.decodedRawData==OxF6097FOO){
    //9
    digitalWrite()
    }
  delay(1000);
  IR.resume();
  }
  
  // put your main code here, to run repeatedly:

}
