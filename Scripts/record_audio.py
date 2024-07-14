import serial
import wave
import struct

COM_PORT = 'COM6'       # Replace with your Arduino's serial port
BAUD_RATE = 115200
SAMPLE_RATE = 8000
BITS_PER_SAMPLE = 16

ser = serial.Serial(COM_PORT, BAUD_RATE)

buffer_size = 128      # Buffer size for receiving data
wav_file = wave.open('recorded_audio.wav', 'wb')
wav_file.setnchannels(1)    # Mono audio
wav_file.setsampwidth(2)    # 16-bit samples
wav_file.setframerate(SAMPLE_RATE)

try:
    while True:
        if ser.in_waiting >= buffer_size * 2:   # Check if enough bytes are available
            data = ser.read(buffer_size * 2)    # Read data in bytes
            audio_data = struct.unpack('<' + 'h' * (len(data) // 2), data)   # Convert bytes to integers
            wav_file.writeframesraw(struct.pack('<' + 'h' * len(audio_data), *audio_data))   # Write data to WAV file
except KeyboardInterrupt:
    print("Recording stopped.")
    wav_file.close()
    ser.close()
