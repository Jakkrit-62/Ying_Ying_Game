#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // TX, RX

int value ;
int Xpin;
int Zpin ;
int buttonPin_shoot;
int buttonPin_restart;
int buttonPin_pause;  
unsigned long lastSwitchTime; 

void setup()
{
  Serial.begin(38400);
  // while (!Serial) ;
  mySerial.begin(38400);
}

void loop()
{
  // if (mySerial.available())
  //   Serial.write(mySerial.read());
  // if (Serial.available())
  //   mySerial.write(Serial.read());
  // Serial.println(mySerial.available());
  // delay(2000);
  if (mySerial.available() >= 5) // ตรวจสอบว่ามีข้อมูลอย่างน้อย 5 ไบต์ในบัฟเฟอร์
  {
    // อ่านค่าข้อมูลทั้งหมดในรอบเดียว
    buttonPin_shoot = mySerial.read();
    buttonPin_restart = mySerial.read();
    buttonPin_pause = mySerial.read();
    Xpin = mySerial.read();
    Zpin = mySerial.read();

    // แสดงค่าที่อ่านได้ใน Serial Monitor
    // Serial.print("Btn_shoot = ");
    Serial.print(buttonPin_shoot);
    Serial.print(",");
    // Serial.print("Btn_restart = ");
    Serial.print(buttonPin_restart);
    Serial.print(",");
    // Serial.print("Btn_pause = ");
    Serial.print(buttonPin_pause);
    Serial.print(",");
    // Serial.print("X = ");
    Serial.print(Xpin);
    Serial.print(",");
    // Serial.print("Z = ");
    Serial.println(Zpin);
    Serial.flush();
    lastSwitchTime = millis(); // รีเซ็ตเวลาของ timeout เมื่อข้อมูลครบ
  }
  if (mySerial.available() <= 5) // ตรวจสอบว่ามีข้อมูลอย่างน้อย 5 ไบต์ในบัฟเฟอร์
  {
    if (millis() - lastSwitchTime > 1000){
      Serial.println("99");
      Serial.flush();
    }
  }
}