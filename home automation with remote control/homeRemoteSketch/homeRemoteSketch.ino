#include <IRremote.h>
IRrecv IR(13);

void setup() {
IR.enableIRIn();
//pinMode();
Serial.begin(9600);

}

void loop() {
  if(IR.decode()){
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
  delay(2000);
  IR.resume();
  }
  
  // put your main code here, to run repeatedly:

}
