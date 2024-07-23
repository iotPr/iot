import serial

def read_file_from_esp32(port, baudrate, output_file):
    with serial.Serial(port, baudrate, timeout=10) as ser:
        ser.write(b'GET_FILE\n')
        with open(output_file, 'wb') as f:
            while True:
                data = ser.read(1024)
                if not data:
                    break
                f.write(data)
        print(f"File saved as {output_file}")

if __name__ == "__main__":
    port = 'COM5'  # Replace with your serial port
    baudrate = 115200
    output_file = 'recording.wav'
    
    read_file_from_esp32(port, baudrate, output_file)