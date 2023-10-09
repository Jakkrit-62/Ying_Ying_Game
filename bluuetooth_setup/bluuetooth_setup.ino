#include <SoftwareSerial.h>
// #define BTpin 12
SoftwareSerial mySerial(10, 11); //  TX,RX

void setup()
{
  Serial.begin(38400);
  while (!Serial) ;
  mySerial.begin(38400);
}

void loop()
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}