#include <SoftwareSerial.h>
#include <avr/sleep.h>
// #include <LiquidCrystal_I2C.h>

#define Xpin 14 // A0
#define Zpin 21 // A7
//#define Ypin 16 // A2
#define buttonPin_shoot 17 // A3
#define buttonPin_restart 15 //A1
#define buttonPin_pause 16 //A2
#define INT0_PIN 2

// LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastSwitchTime; 
bool sleep_state = false; //ไม่สลีปโหมด

SoftwareSerial transfer_serial(10, 11); // กำหนดขา TX และ RX ที่ต้องการใช้สำหรับ SoftwareSerial

// byte p[8] = {
//   0x1F,
//   0x1F, 
//   0x1F,
//   0x1F,
//   0x1F,
//   0x1F,
//   0x1F,
//   0x1F
// };

// char value[30];
void SLEEP_DISABLE() {
  SMCR &= 0xFE;
}
void SLEEP_INITIALIZE(uint8_t m) {
  SMCR = (m << 1) | 0x01;
    // SLEEP_INITIALIZE(0): Idle mode (โหมดการสลีปแบบ Idle)
    // ในโหมดนี้, CPU จะหยุดทำงาน แต่ตัวคริสตัลและโมดูลต่างๆ ยังคงทำงานอยู่ ดังนั้น, คุณยังคงสามารถใช้งานคริสตัล (ความถี่) และโมดูลอื่น ๆ ได้.
    // SLEEP_INITIALIZE(1): ADC Noise Reduction mode (โหมดการสลีปแบบลดรบกวนของ ADC)
    // โหมดนี้จะทำให้ CPU หยุดทำงานและปิดรบกวนจาก ADC (Analog-to-Digital Converter) เพื่อให้การวัดสัญญาณแบบอนาล็อกเสียงให้มีความแม่นยำ.
    // SLEEP_INITIALIZE(2): Power-down mode (โหมดการสลีปแบบปิดอุปกรณ์)
    // โหมดนี้จะทำให้ CPU หยุดทำงานและปิดอุปกรณ์ทุกอย่าง มันเหมาะสำหรับการประหยัดพลังงานในระยะเวลาที่ไม่มีการใช้งานและการทำงานจะถูกรีเซ็ตใหม่เมื่อมีการตื่นขึ้น.
    // SLEEP_INITIALIZE(3): Power-save mode (โหมดการสลีปแบบประหยัดพลังงาน)
    // โหมดนี้เป็นโหมดการสลีปที่แบบระหว่าง โหมด Idle และ Power-down โหมดนี้ยังคงทำงานบางส่วนของอุปกรณ์เพื่อประหยัดพลังงาน.
}

void setup() {
  Serial.begin(38400); // ตั้งค่าอุณหภูมิของ Serial
  transfer_serial.begin(38400); // ตั้งค่าอุณหภูมิของ SoftwareSerial
  pinMode(Xpin, INPUT);  
  pinMode(Zpin, INPUT);
  pinMode(buttonPin_shoot, INPUT);
  pinMode(buttonPin_restart, INPUT);
  pinMode(buttonPin_pause, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT0_PIN), Awake_func, FALLING);

  // lcd.init();
  // lcd.backlight();
  // lcd.clear();
}

void Awake_func() {
  if (sleep_state){
    Serial.println("Interrupt from Button Power");
    Serial.println("Exit Sleep");
    sleep_state = false;  //Toggle ค่า
    SLEEP_DISABLE();  // ปิดโหมดการสลีป
    transfer_serial.begin(38400);  // เริ่มต้นการสื่อสารผ่าน SoftwareSerial อีกครั้ง
    lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  }
  // else{ //ถ้าไม่อยู่ในโหมดสลีปแล้วกดปุ่มเพื่อเข้าสู่สลีปโหมด
  //   Serial.println("Sleep");  // แสดงข้อความ "Sleep" ใน Serial Monitor
  //   sleep_state = true; //Toggle ค่า
  //   transfer_serial.end();  // หยุดการสื่อสารผ่าน SoftwareSerial
  //   SLEEP_INITIALIZE(2);  // ตั้งค่าโหมดการสลีป (sleep mode) เป็น 2
  //   lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  //   sleep_enable();
  //   sleep_cpu();  // เข้าสู่โหมดการสลีปเพื่อประหยัดพลังงาน
  //   SLEEP_DISABLE();  // ปิดโหมดการสลีป
  //   Serial.println("Awake");
  // }
  // _delay_ms(2000);
}

void sleep_func(){
  transfer_serial.end();  // หยุดการสื่อสารผ่าน SoftwareSerial
  SLEEP_INITIALIZE(2);  // ตั้งค่าโหมดการสลีป (sleep mode) เป็น 2
  sleep_enable();
  Serial.println("Sleep");  // แสดงข้อความ "Sleep" ใน Serial Monitor
  lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  sleep_state = true;
  delay(1000);
  sleep_cpu();  // เข้าสู่โหมดการสลีปเพื่อประหยัดพลังงาน
}

void loop() {
  //Set up code bluetooth
  // if (transfer_serial.available())
  //   Serial.write(transfer_serial.read());
  // if (Serial.available())
  //   transfer_serial.write(Serial.read());
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
  //เช็คปุ่มว่าโดนกดอยู่ ถ้าไม่กดนานจะได้เข้าสลีป
  if (buttonState_shoot == 1 || buttonState_restart == 1 || buttonState_pause == 1) {
    lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  }

  // lcd.setCursor(0, 0);
  // lcd.print("X = ");
  // lcd.print(xValue);
  // lcd.print("  Z = ");
  // lcd.print(zValue);
  // lcd.setCursor(0, 1);
  // lcd.print("  btn = ");
  // lcd.print(buttonState_shoot);
  if (millis() - lastSwitchTime <= 5000) {  // ตรวจสอบว่าเวลาที่ผ่านมาหลังจากการกดปุ่มหรือ interrupt มากกว่า 30 วินาทีหรือไม่
    Serial.println((millis() - lastSwitchTime)/100);  // แสดงข้อความ "Hello" ใน Serial Monitor
  }
  if (millis() - lastSwitchTime > 5000) {  // ตรวจสอบว่าเวลาที่ผ่านมาหลังจากการกดปุ่มหรือ interrupt มากกว่า 30 วินาทีหรือไม่
    Serial.println("5 Second Pass");  // แสดงข้อความ "Hello" ใน Serial Monitor
    sleep_func();
  }
  // lcd.setCursor(0, 0);
  // lcd.print("X = ");
  // lcd.print(xValue);
  // lcd.print("  Z = ");
  // lcd.print(zValue);
  // lcd.setCursor(0, 1);
  // lcd.print("  btn = ");
  // lcd.print(buttonState_shoot);

  Serial.print(int(buttonState_shoot));
  Serial.print(int(buttonState_restart));
  Serial.print(int(buttonState_pause));
  Serial.print(xValue);
  Serial.println(zValue);

  transfer_serial.write((buttonState_shoot));
  delay(10);
  transfer_serial.write((buttonState_restart));
  delay(10);
  transfer_serial.write((buttonState_pause));
  delay(10);
  transfer_serial.write((xValue));
  delay(10);
  transfer_serial.write((zValue));
  delay(100);
}
