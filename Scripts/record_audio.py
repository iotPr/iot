import serial
import numpy as np
import wave

# Configure the serial port
ser = serial.Serial('COM6', 115200)  # Replace 'COM3' with your serial port

sample_rate = 16000
seconds_to_record = 5
num_samples = sample_rate * seconds_to_record

# Create a buffer to store the audio data
audio_data = np.zeros((num_samples,), dtype=np.int16)

print("Recording...")

index = 0
while index < num_samples:
    if ser.in_waiting:
        data = ser.read(2)
        value = int.from_bytes(data, byteorder='little', signed=True)
        audio_data[index] = value
        index += 1

print("Recording finished.")

# Save the data to a WAV file
with wave.open('output.wav', 'w') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(2)
    wf.setframerate(sample_rate)
    wf.writeframes(audio_data.tobytes())

print("Recording saved to output.wav")
