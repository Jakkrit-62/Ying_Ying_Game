#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define Xpin 14 // A0
#define Ypin 21 // A7
#define Zpin 16 // A2
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

int buttonState = 0;

void setup() {
  Serial.begin(31250); // ตั้งค่าอุณหภูมิของ Serial
  transfer_serial.begin(31250); // ตั้งค่าอุณหภูมิของ SoftwareSerial
  pinMode(buttonPin, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Read data from sensors connected to Xpin, Ypin, and Zpin
  int xValue = analogRead(Xpin);
  int yValue = analogRead(Ypin);
  int zValue = analogRead(Zpin);

  int buttonState = digitalRead(buttonPin); // อ่านสถานะของสวิตช์

  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("X = ");
  lcd.print(xValue);
  lcd.print("  Y = ");
  lcd.print(yValue);
  lcd.setCursor(0, 1);
  lcd.print("Z = ");
  lcd.print(zValue);
  lcd.print("  btn = ");
  lcd.print(buttonState);

  // Send the sensor values over SoftwareSerial
  Serial.println(buttonState);
  //Serial.print(",");
  Serial.println(xValue);
  //Serial.print(",");
  Serial.println(yValue);
  //Serial.print(",");
  Serial.println(zValue);

  transfer_serial.print(buttonState);
  transfer_serial.print(xValue);
  transfer_serial.print(yValue);
  transfer_serial.print(zValue);
}
