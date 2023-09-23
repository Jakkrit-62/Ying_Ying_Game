#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define Xpin 14 // A0
#define Ypin 21 // A7
//#define Zpin 16 // A2
#define buttonPin_shoot 17 // A3
#define buttonPin_restart 15 //A1
#define buttonPin_pause 16 //A2

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
  pinMode(buttonPin_shoot, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  
  int buttonState_shoot = digitalRead(buttonPin_shoot); // อ่านสถานะของสวิตช์ A3
  int buttonState_restart = digitalRead(buttonPin_restart); // อ่านสถานะของสวิตช์ A1 
  int buttonState_pause = digitalRead(buttonPin_pause); // อ่านสถานะของสวิตช์ A6
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
  lcd.print(buttonState_shoot);

  Serial.println(int(buttonState_shoot));
  //Serial.print("Re = ");
  Serial.println(int(buttonState_restart));
  //Serial.print("Pa = ");
  Serial.println(int(buttonState_pause));
  //Serial.print(",");
  Serial.println(xValue);
  //Serial.print(",");
  Serial.println(yValue);
  //Serial.print(",");
  //Serial.println(zValue);

  transfer_serial.print(int(buttonState_shoot));
  transfer_serial.print(int(buttonState_restart));
  transfer_serial.print(int(buttonState_pause));
  transfer_serial.print(int(xValue));
  transfer_serial.print(int(yValue));
  //transfer_serial.print(zValue);
}
