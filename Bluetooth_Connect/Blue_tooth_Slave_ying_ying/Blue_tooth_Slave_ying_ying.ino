#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // TX, RX

int value;
int Xpin;
int Zpin;
int buttonPin_shoot;
int buttonPin_restart;
int buttonPin_pause;
unsigned long lastSwitchTime;

void setup() {
  Serial.begin(38400);
  // while (!Serial) ;
  mySerial.begin(38400);
  Serial.flush(); // เคลียร์ค่าในบัฟเฟอร์
}

void loop() {
  //Set up code bluetooth สำหรับใช้ config ตัว Bluetooth
  // if (mySerial.available())
  //   Serial.write(mySerial.read());
  // if (Serial.available())
  //   mySerial.write(Serial.read());
  Serial.println(mySerial.available());
  //อ่านค่าที่ส่งมาผ่าน Bluetooth
  if (mySerial.available() >= 1) {
    byte receivedData = mySerial.read();

    // แยกค่าปุ่มและค่าคำสั่งเคลื่อนที่ จากข้อมูลที่รับมา
    int buttonState_shoot = (receivedData >> 0) & 0x01; // บิตที่ 0 เก็บค่าจากปุ่มยิง
    int buttonState_restart = (receivedData >> 1) & 0x01; // บิตที่ 1 เก็บค่าจากปุ่มรีสตาร์ท
    int buttonState_pause = (receivedData >> 2) & 0x01; // บิตที่ 2 เก็บค่าจากปุ่มหยุด
    int xValue = (receivedData >> 3) & 0x03; // บิตที่ 3,4 เก็บค่าคำสั่งการเคลื่อนที่แกน X
    int zValue = (receivedData >> 5) & 0x03;  // บิตที่ 5,6 เก็บค่าคำสั่งการเคลื่อนที่แกน Z

    // แสดงผลข้อมูลที่รับมาใน Serial Monitor
    Serial.print(buttonState_shoot);
    Serial.print(",");
    Serial.print(buttonState_restart);
    Serial.print(",");
    Serial.print(buttonState_pause);
    Serial.print(",");
    Serial.print(xValue);
    Serial.print(",");
    Serial.println(zValue);
    lastSwitchTime = millis();  // รีเซ็ตเวลาของ timeout เมื่อข้อมูลครบ
  }
  if (mySerial.available() <= 1)  // ตรวจสอบว่ามีข้อมูลอย่างน้อย 1 ไบต์ในบัฟเฟอร์
  {
    if (millis() - lastSwitchTime > 1000) {
      Serial.println("Timeout");
      Serial.flush(); // เคลียร์ค่าในบัฟเฟอร์
    }
  }
}