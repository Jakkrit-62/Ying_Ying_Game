import serial

# กำหนดพอร์ตที่ Arduino ใช้สำหรับ SoftwareSerial ตาม Xpin, Ypin, Zpin
arduino_port = '/dev/cu.wchusbserial1140'  # แทนด้วยพอร์ตที่ตรงกับการกำหนดใน Arduino

# เริ่มการเชื่อมต่อกับ Arduino ผ่านพอร์ตที่กำหนด
arduino_serial = serial.Serial(arduino_port, 31250)

while True:
    arduino_data_btn = int(arduino_serial.readline().decode().strip())
    arduino_data_x = int(arduino_serial.readline().decode().strip())
    arduino_data_y = int(arduino_serial.readline().decode().strip())
    arduino_data_z = int(arduino_serial.readline().decode().strip())

    print(f"Received data from Arduino: btn = {arduino_data_btn}, x = {arduino_data_x}, y = {arduino_data_y}, z = {arduino_data_z}")



