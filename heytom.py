import time
import sounddevice as sd
import numpy as np

import scipy.io.wavfile as wav

# Set the sampling frequency
fs = 16000

# Set the duration of each recording in seconds
duration = 1

# Set the number of recordings
num_recordings = 50

# Create an empty array to store the recordings
recordings = np.empty((num_recordings, duration * fs))

# Loop through the desired number of recordings
for i in range(num_recordings):
	# Record audio for the specified duration
	recording = sd.rec(int(duration * fs), samplerate=fs, channels=1)
	sd.wait()
	print("wait 0.5 sec")
	time.sleep(0.5)
	# Print the name of the record and start recording
	print(f"Recording {i+1} - start record")

	# Delay for 0.5 seconds
	
	# Store the recording in the array
	recordings[i] = np.squeeze(recording)

	# Save the recording as a WAV file
	filename = f"recording_{i+51}.wav"
	wav.write(filename, fs, recording)

print("Recording and saving complete!")

# Loop through all the recordings
for i in range(num_recordings):
	# Get the filename of the recording
	filename = f"recording_{i+51}.wav"
	
	# Read the WAV file to get the sampling frequency
	_, audio_data = wav.read(filename)
	sampling_frequency = audio_data.shape[0]
	
	# Print the filename and the sampling frequency
	print(f"Recording {filename} contains {sampling_frequency} Hz")