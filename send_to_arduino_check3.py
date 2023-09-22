import serial

# กำหนดพอร์ตที่ Arduino ใช้สำหรับ SoftwareSerial ตาม Xpin, Ypin, Zpin
arduino_port = '/dev/cu.wchusbserial1140'  # แทนด้วยพอร์ตที่ตรงกับการกำหนดใน Arduino

# เริ่มการเชื่อมต่อกับ Arduino ผ่านพอร์ตที่กำหนด
arduino_serial = serial.Serial(arduino_port, 57600)

while True:
    arduino_data = arduino_serial.readline().decode('latin-1').strip()
    arduino_data_parts = arduino_data.split(',')  # แยกค่าด้วย ','

    if len(arduino_data_parts) == 4:  # ตรวจสอบว่ามีค่า 4 ตัวหลังแยกด้วย ',' หรือไม่
        arduino_data_btn, arduino_data_x, arduino_data_y, arduino_data_z = map(int, arduino_data_parts)
        print(f"Received data from Arduino: btn = {arduino_data_btn}, x = {arduino_data_x}, y = {arduino_data_y}, z = {arduino_data_z}")
    else:
        print("Invalid data format from Arduino")



