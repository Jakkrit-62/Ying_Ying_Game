import serial
import time


# กำหนดพอร์ตที่ Arduino ใช้สำหรับ SoftwareSerial ตาม Xpin, Ypin, Zpin
arduino_port = '/dev/cu.wchusbserial1110'  # แทนด้วยพอร์ตที่ตรงกับการกำหนดใน Arduino

# เริ่มการเชื่อมต่อกับ Arduino ผ่านพอร์ตที่กำหนด
arduino_serial = serial.Serial(arduino_port, 9600)
start_time = time.time()
values=[]

while True:
    end_time = time.time()
    elapsed_time = end_time - start_time
  # เริ่มจับเวลาใหม่สำหรับรอบถัดไป
    #print("1")
    #if arduino_serial.inWaiting() >=1 :
        #arduino_serial.flushInput()
    #print("Flush",arduino_serial.inWaiting())
    arduino_data = arduino_serial.read(1).decode()
    #arduino_data = arduino_data.replace('\r', '2')
    values.append(arduino_data) # แยกค่าด้วย ',' เพื่อแยกเป็นตัวแปรแต่ละค่า
    if len(values)==5:
        print("ARDUINO = ",values)
        arduino_data_btn_shoot,arduino_data_btn_restart,arduino_data_btn_pause,arduino_data_x, arduino_data_y = map(str, values)
        print(f"Received data from Arduino0: shoot = {arduino_data_btn_shoot}, restart = {arduino_data_btn_restart}, pause = {arduino_data_btn_pause}")
        print(f"Received data from Arduino1: x = {arduino_data_x}, y = {arduino_data_y}")
        values=[]
        print(f"รอบลูประยะเวลา: {elapsed_time} วินาที")
        start_time = end_time
    # #arduino_data_z = int(arduino_serial.readline().decode().strip())
