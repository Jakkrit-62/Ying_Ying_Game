#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define Xpin 14 // A0
#define Zpin 21 // A7
//#define Ypin 16 // A2
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
  pinMode(Xpin, INPUT);  
  pinMode(Zpin, INPUT);
  pinMode(buttonPin_shoot, INPUT);
  pinMode(buttonPin_restart, INPUT);
  pinMode(buttonPin_pause, INPUT);

  // lcd.init();
  // lcd.backlight();
  // lcd.clear();
}

void loop() {
  
  int buttonState_shoot = digitalRead(buttonPin_shoot); // อ่านสถานะของสวิตช์ A3
  int buttonState_restart = digitalRead(buttonPin_restart); // อ่านสถานะของสวิตช์ A1 
  int buttonState_pause = digitalRead(buttonPin_pause); // อ่านสถานะของสวิตช์ A6
  int xValue = analogRead(Xpin); // Default x = 380  
  int zValue = analogRead(Zpin); // Default z =390
  //int zValue = analogRead(Ypin);


  if (xValue > 420) {
    xValue = 0; // ถอยหลัง
  } else if (xValue < 365) {
    xValue = 1;  //ไปหน้า
  }
  else{
    xValue = 2;}

  if (zValue > 410) {
    zValue = 1;   //เลี้ยวซ้าย
  } else if (zValue < 350) {
    zValue = 0; //เลี้ยวขวา
  }
  else{
    zValue = 2;}

  // delay(100);
  // lcd.clear();

  // lcd.setCursor(0, 0);
  // lcd.print("X = ");
  // lcd.print(xValue);
  // lcd.print("  Y = ");
  // lcd.print(zValue);
  // lcd.setCursor(0, 1);
  //lcd.print("Z = ");
  //lcd.print(zValue);
  // lcd.print("  btn = ");
  // lcd.print(buttonState_shoot);

  // //while shoot
  // if (buttonState_shoot==0){
  //   lcd.setCursor(0, 0);
  //   lcd.print("Shooting PiwPiw");
  // }

  Serial.print(int(buttonState_shoot));
  Serial.print(",");
  //Serial.print("Re = ");
  Serial.print(int(buttonState_restart));
  Serial.print(",");
  //Serial.print("Pa = ");
  Serial.print(int(buttonState_pause));
  Serial.print(",");
  // Serial.print("X = ");
  Serial.print(int(xValue));
  Serial.print(",");
  // Serial.print("Z = ");
  Serial.println(int(zValue));

  transfer_serial.print(int(buttonState_shoot));
  transfer_serial.print(int(buttonState_restart));
  transfer_serial.print(int(buttonState_pause));
  transfer_serial.print(int(xValue));
  transfer_serial.print(int(zValue));
  //transfer_serial.print(zValue);
}
