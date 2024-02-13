#include <IRremote.h>
IRrecv IR(12);
int led=13;
int office=22;
int kitchen=24;
int lappy=26;
int tv= 28;
int count=0;
String passcode ="1234";
String inputpasscode="";
void setup() {
IR.enableIRIn();
pinMode(office,OUTPUT);
pinMode(kitchen,OUTPUT);
pinMode(lappy,OUTPUT);
pinMode(tv,OUTPUT);

Serial.begin(9600);

//put a passcode-4 digit
while (inputpasscode!=passcode){
  
  if(IR.decode()){
    if(IR.decodedIRData.decodedRawData==0x0){
      
      IR.resume();
    }
    else{
      
        if(count<4){

            if(IR.decodedIRData.decodedRawData==0xFE017F00){
            //1
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+=1;
                Serial.println(inputpasscode);
                count+=1;

            }
            if(IR.decodedIRData.decodedRawData==0xFD027F00){
            //2 
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="2";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xFC037F00){
            //3
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="3";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xFB047F00){
            //4
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="4";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xFA057F00){
            //5 
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="5";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xF9067F00){
            //6
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="6";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xF8077F00){
            //7
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="7";
                Serial.println(inputpasscode);
                count+=1;
            }

            if(IR.decodedIRData.decodedRawData==0xF7087F00){
            //8
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="8";
                Serial.println(inputpasscode);
                count+=1;
            }
            if(IR.decodedIRData.decodedRawData==0xF6097F00){
            //9
                digitalWrite(led,HIGH);
                delay(500);
                digitalWrite(led,LOW);
                inputpasscode+="9";
                Serial.println(inputpasscode);
                count+=1;
            }


        }
        if(count==4 && passcode!=inputpasscode){
        Serial.println("incorrect");
        count=0;
        inputpasscode="";
        IR.resume();
        }
    
    
    IR.resume(); 
    
    }
    
  }
  
}
digitalWrite(office,HIGH);
delay(2000);
digitalWrite(office,LOW);


}

void loop() {
  if(IR.decode()){
    
    if(IR.decodedIRData.decodedRawData==0x0){
      
      IR.resume();
    }
    if(IR.decodedIRData.decodedRawData == 0xEA157F00){
      //OFF  
      digitalWrite(office,LOW);
      digitalWrite(tv,LOW);
      digitalWrite(lappy,LOW);
      digitalWrite(kitchen,LOW);
      digitalWrite(led,HIGH);


    }
    if(IR.decodedIRData.decodedRawData==0xFE017F00){
    //1
    digitalWrite(office,HIGH);

    }
    if(IR.decodedIRData.decodedRawData==0xFD027F00){
    //2
    digitalWrite(office,LOW);
    }
    if(IR.decodedIRData.decodedRawData==0xFC037F00){
    //3
    digitalWrite(tv,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==0xFB047F00){
    //4
    digitalWrite(tv,LOW);
    }
    if(IR.decodedIRData.decodedRawData==0xFA057F00){
    //5
    digitalWrite(lappy,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==0xF9067F00){
    //6
    digitalWrite(lappy,LOW);
    }
    if(IR.decodedIRData.decodedRawData==0xF8077F00){
    //7
    digitalWrite(kitchen,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==0xEF107F00){
    //ON
    digitalWrite(office,HIGH);
    digitalWrite(tv,HIGH);
    digitalWrite(lappy,HIGH);
    digitalWrite(kitchen,HIGH);
    }
    if(IR.decodedIRData.decodedRawData==0xF7087F00){
    //8
    digitalWrite(kitchen,LOW);
    }
    if(IR.decodedIRData.decodedRawData==0xF6097F00){
    //9
    //
    }
  delay(1000);
  IR.resume();
  }
  
  // put your main code here, to run repeatedly:

}
