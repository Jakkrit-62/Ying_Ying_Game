#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define Xpin 14 // A0
#define Ypin 21 // A7
//#define Zpin 16 // A2
#define buttonPin 17 // A3

LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial transfer_serial(10, 11); // กำหนดขา RX และ TX ที่ต้องการใช้สำหรับ SoftwareSerial

byte p[8] = {
  0x1F,
  0x1F, 
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};


void setup() {
  Serial.begin(9600); // ตั้งค่าอุณหภูมิของ Serial
  transfer_serial.begin(9600); // ตั้งค่าอุณหภูมิของ SoftwareSerial
  pinMode(buttonPin, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Read data from sensors connected to Button,Xpin, Ypin
  bool buttonState = digitalRead(buttonPin); // อ่านสถานะของสวิตช์
  int xValue = analogRead(Xpin);
  int yValue = analogRead(Ypin);
  //int zValue = analogRead(Zpin);


  if (xValue > 420) {
    xValue = 0;
  } else if (xValue < 365) {
    xValue = 1;
  }
  else{
    xValue = -1;}

  if (yValue > 410) {
    yValue = 0;
  } else if (yValue < 350) {
    yValue = 1;
  }
  else{
    yValue = -1;}

  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("X = ");
  lcd.print(xValue);
  lcd.print("  Y = ");
  lcd.print(yValue);
  lcd.setCursor(0, 1);
  //lcd.print("Z = ");
  //lcd.print(zValue);
  lcd.print("  btn = ");
  lcd.print(buttonState);

  // Send the sensor values over SoftwareSerial
  Serial.println(int(buttonState));
  //Serial.print(",");
  Serial.println(xValue);
  //Serial.print(",");
  Serial.println(yValue);
  //Serial.print(",");
  //Serial.println(zValue);

  transfer_serial.print(int(buttonState));
  transfer_serial.print(int8_t(xValue));
  transfer_serial.print(int8_t(yValue));
  //transfer_serial.print(zValue);
}
