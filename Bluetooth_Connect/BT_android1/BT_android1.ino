#include <SoftwareSerial.h>
// #define BTpin 12
#define Btnpin 8
SoftwareSerial mySerial(2, 3); //  TX,RX

int state = 0;

void setup()
{
  Serial.begin(38400);
  // while (!Serial) ;
  mySerial.begin(38400);
  pinMode(Btnpin,INPUT);
}

void loop()
{
  if (digitalRead(Btnpin)==0){
    Serial.println("PRESS!!");
    mySerial.write(3);
  }
  else{
    mySerial.write(1);
  }
  if (mySerial.available()>0){
    state = mySerial.read();
    Serial.println(state);
  }
  // Serial.println(digitalRead(BTpin));
  // if (mySerial.available())
  //   Serial.write(mySerial.read());
  // if (Serial.available())
  //   mySerial.write(Serial.read());

  delay(1000);
}