#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // TX, RX

int value;
int Xpin;
int Zpin;
int buttonPin_shoot;
int buttonPin_restart;
int buttonPin_pause;
unsigned long lastSwitchTime;

//reset Arduino
void (*resetFunc)(void) = 0;

void setup() {
  Serial.begin(38400);
  // while (!Serial) ;
  mySerial.begin(38400);
  Serial.flush();
}

void loop() {
  // if (mySerial.available())
  //   Serial.write(mySerial.read());
  // if (Serial.available())
  //   mySerial.write(Serial.read());
  // Serial.println(mySerial.available());
  // delay(2000);
  if (mySerial.available() >= 1) {
    byte receivedData = mySerial.read();

    // แยกค่าปุ่มและค่า X และ Z จากข้อมูลที่รับมา
    int buttonState_shoot = (receivedData >> 0) & 0x01;
    int buttonState_restart = (receivedData >> 1) & 0x01;
    int buttonState_pause = (receivedData >> 2) & 0x01;
    int xValue = (receivedData >> 3) & 0x03;
    int zValue = (receivedData >> 5) & 0x03;

    // แสดงผลข้อมูลที่รับมาใน Serial Monitor
    // Serial.print("Button Shoot: ");
    Serial.print(buttonState_shoot);
    Serial.print(",");
    // Serial.print("Btn_restart = ");
    Serial.print(buttonState_restart);
    Serial.print(",");
    // Serial.print("Btn_pause = ");
    Serial.print(buttonState_pause);
    Serial.print(",");
    // Serial.print("X = ");
    Serial.print(xValue);
    Serial.print(",");
    // Serial.print("Z = ");
    Serial.println(zValue);
    lastSwitchTime = millis();  // รีเซ็ตเวลาของ timeout เมื่อข้อมูลครบ
  }
  if (mySerial.available() <= 5)  // ตรวจสอบว่ามีข้อมูลอย่างน้อย 5 ไบต์ในบัฟเฟอร์
  {
    if (millis() - lastSwitchTime > 1000) {
      Serial.println("Timeout");
      Serial.flush();
    }
  }
}