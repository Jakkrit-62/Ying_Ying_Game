import serial

# กำหนดพอร์ตที่ Arduino ใช้สำหรับ SoftwareSerial ตาม Xpin, Ypin, Zpin
arduino_port = '/dev/cu.wchusbserial1110'  # แทนด้วยพอร์ตที่ตรงกับการกำหนดใน Arduino

# เริ่มการเชื่อมต่อกับ Arduino ผ่านพอร์ตที่กำหนด
arduino_serial = serial.Serial(arduino_port, 9600)

while True:
    if arduino_serial.inWaiting() >=1 :
        arduino_serial.flushInput()
    print("Flush",arduino_serial.inWaiting())
    arduino_data_btn = (arduino_serial.readline().decode('latin-1').strip())
    arduino_data_x = int(arduino_serial.readline().decode('latin-1').strip())
    arduino_data_y = int(arduino_serial.readline().decode('latin-1').strip())
    #arduino_data_z = int(arduino_serial.readline().decode('latin-1').strip())

    print(f"Received data from Arduino: btn = {arduino_data_btn}, x = {arduino_data_x}, y = {arduino_data_y}")
    



