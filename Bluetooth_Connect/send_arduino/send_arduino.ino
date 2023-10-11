#include <SoftwareSerial.h>
#include <avr/sleep.h>
// #include <LiquidCrystal_I2C.h>

#define Xpin 14 // Pin A0
#define Zpin 21 // Pin A7
#define buttonPin_shoot 5 // Pin D5
#define buttonPin_restart 15 // Pin A1
#define buttonPin_pause 16 // Pin A2
#define INT0_PIN 2 // Pin D2 เป็น Pin Interrupt Pin INT0
#define LED_PIN 12 // หากเข้า Sleep Mode LED ดับ หาก Arduino ทำงาน LED จะติด
#define time_out_sec 15 // เวลา Timeout หน่วยวินาที
#define time_out_milli 15000 // เวลา Timeout หน่วยมิลลิวินาที

// LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastSwitchTime; // ตัวเเปรเก็บเวลาสำหรับ Time Out
bool sleep_state = false; // Default = ไม่สลีปโหมด
int newXValue = -1; // อัพเดตค่า x เพื่อเช็คแกนเปลี่ยนมั้ย
int newZValue = -1; // อัพเดตค่า z เพื่อเช็คแกนเปลี่ยนมั้ย
int restart_times = 0; // จำนวนครั้งที่กด restart = 0
bool pause_state = false; //สถานะปุ่ม Default pause = false

SoftwareSerial transfer_serial(10, 11); // กำหนดขา TX และ RX ที่ต้องการใช้สำหรับ SoftwareSerial

//สัญลักษณ์กระสุนแสดงบน LCD
// byte bulletChar[8] = {
//   0b10000,
//   0b11100,
//   0b11110,
//   0b11111,
//   0b11111,
//   0b11110,
//   0b11100,
//   0b10000
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
  Serial.begin(38400); // ตั้งค่า Baud Rate ของ Serial
  transfer_serial.begin(38400); //  ตั้งค่า Baud Rate ของ SoftwareSerial
  pinMode(Xpin, INPUT);  //กำหนดให้ขาที่ 14 สำหรับรับค่าแกน X ให้เป็นอินพุต
  pinMode(Zpin, INPUT); //กำหนดให้ขาที่ 21 สำหรับรับค่าแกน Z ให้เป็นอินพุต
  pinMode(buttonPin_shoot, INPUT);  //กำหนดให้ขาที่ 5 สำหรับรับค่าปุ่มยิงให้เป็นอินพุต
  pinMode(buttonPin_restart, INPUT); //กำหนดให้ขาที่ 15 สำหรับรับค่าปุ่มรีสตาร์ทให้เป็นอินพุต
  pinMode(buttonPin_pause, INPUT);  //กำหนดให้ขาที่ 16 สำหรับรับค่าปุ่มหยุดให้เป็นอินพุต
  attachInterrupt(digitalPinToInterrupt(INT0_PIN), Awake_func, FALLING); //กำหนด Interrupt
  pinMode(LED_PIN, OUTPUT); //ตั้งให้ขาที่ 12 เป็น OutPut

}

void Awake_func() {
  // หากไม่อยู่ใน Sleep Mode จะไม่สามารถปลุก
  if (sleep_state){
    Serial.println("Interrupt from Button Power");
    Serial.println("Exit Sleep");
    sleep_state = false;  //Toggle ค่า State ของ Sleep Mode
    pause_state = false; //ออกจาก Pause
    SLEEP_DISABLE();  // ออกจาก Sleep Mode
    transfer_serial.begin(38400);  // เริ่มต้นการสื่อสารผ่าน SoftwareSerial อีกครั้ง
    lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
    digitalWrite(LED_PIN, HIGH); // ให้ LED ติด เมื่อ Arduino ไม่อยู่ Sleep Mode
  }
}

//ฟังก์ชัน Sleep
void sleep_func(){
  transfer_serial.end();  // หยุดการสื่อสารผ่าน SoftwareSerial
  SLEEP_INITIALIZE(2);  // ตั้งค่าโหมดการสลีป (sleep mode) เป็น 2 Power Down Mod
  sleep_enable(); //เปิดการใช้งานโหมดการสลีป

  lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  sleep_state = true; // สถานะการนอนเป็น True เพื่อให้สามารถกดปลุกได้
  digitalWrite(LED_PIN, LOW); // ปิด LED ก่อนเข้า Sleep Mode
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
  
  transfer_serial.write(xValue);
  transfer_serial.write(zValue);
  //เเปลงค่า Digital เป็นคำสั่งการเคลื่อนที่
  if (xValue > 435) {
    xValue = 0; // ถอยหลัง
  } else if (xValue < 350) {
    xValue = 1;  //ไปหน้า
  }
  else{
    xValue = 2; // IDLE
    } 
  if (zValue > 400) {
    zValue = 1;   //เลี้ยวขวา
  } else if (zValue < 350) {
    zValue = 0; //เลี้ยวซ้าย
  }
  else{
    zValue = 2; //IDLE
  }

  //เช็คปุ่มว่าโดนกดอยู่ ถ้าไม่กดนานจะได้เข้า Sleep Mode
  if (buttonState_shoot == 1 || buttonState_restart == 1 || buttonState_pause == 1) {
    lastSwitchTime = millis();  // รีเซ็ตเวลาเมื่อมีการกดสวิตช์
  }
  // ถ้าค่า x หรือ z มีการเปลี่ยนแปลงจะไม่เข้าสู่ Sleep Mode
  if (newXValue != xValue || newZValue != zValue) {
    lastSwitchTime = millis(); // รีเซ็ตเวลา
  }
  newXValue = xValue; //เก็บค่าปัจจุบันเพื่อไว้เปรียบเทียบในลูปถัดไป
  newZValue = zValue; //เก็บค่าปัจจุบันเพื่อไว้เปรียบเทียบในลูปถัดไป

  
  // ตรวจสอบว่าเวลาที่ผ่านมาหลังจากการกดปุ่มหรือ interrupt มากกว่า 15 วินาทีหรือไม่
  // และเเสดงเวลาที่เหลือก่อนจะเข้าสู่ Sleep Mode บน LCD
  if (millis() - lastSwitchTime <= time_out_milli) {  
    Serial.print("Sleep in ");
    Serial.print(time_out_sec - ( (millis() - lastSwitchTime)/1000 ) );
    Serial.println(" Sec");
  }
  
  // หาก Time Out ก็จะเรียกใช้ Function Sleep Mode --> sleep_func
  if (millis() - lastSwitchTime > time_out_milli) {
    Serial.print(time_out_sec);
    Serial.println(" Second Pass");
    sleep_func();
  }

  //แสดงผลการค่าที่อ่าน
  Serial.print(int(buttonState_shoot));
  Serial.print(int(buttonState_restart));
  Serial.print(int(buttonState_pause));
  Serial.print(xValue);
  Serial.println(zValue);

  //ส่งข้อมูล 1 Byte โดยเเต่ละบิตจะมีค่าต่าง ๆ
  byte dataToSend = 0;
  dataToSend |= (byte)(buttonState_shoot << 0); // บิตที่ 0 เก็บค่าจากปุ่มยิง
  dataToSend |= (byte)(buttonState_restart << 1); // บิตที่ 1 เก็บค่าจากปุ่มรีสตาร์ท
  dataToSend |= (byte)(buttonState_pause << 2); // บิตที่ 2 เก็บค่าจากปุ่มหยุด
  dataToSend |= (byte)(xValue << 3); // บิตที่ 3,4 เก็บค่าคำสั่งการเคลื่อนที่แกน X
  dataToSend |= (byte)(zValue << 5); // บิตที่ 5,6 เก็บค่าคำสั่งการเคลื่อนที่แกน Z

  // เคลียร์บิตที่ 7
  dataToSend &= ~(1 << 7);

  transfer_serial.write(dataToSend);
  delay(100);
}