# Voice Assistant - Marvin

This project is a voice assistant that utilizes a wake-word to activate and perform various tasks. It is designed to work with an ESP32 board and several hardware components including the INMP441 microphone, I2S speaker, and RGB light.

## Summary

The voice assistant operates as follows:

0. The system waits for the user's wake word, indicated by a green light.
1. When the user says the word 'Marvin', the system transitions to the next state and starts listening for the user's voice input. This state is indicated by a red light, which later turns blue.
2. After the user speaks, the system records the audio, converts it to Base64, and streams it to the Google Voice-to-Text service. The red light indicates that the system is processing the audio, and it later turns blue.
3. Once the Google Voice-to-Text service returns the transcribed text, it is sent to the Chat-GPT service for further processing. The blue light remains on during this step.
4. The response from the Chat-GPT service is then sent to the Google Translate service for translation. It's important to note that this translation service is free, as it is Google Translate and not Google Text-to-Speech. The blue light remains on.
5. The user can either proceed to the next question or wait for a moment to start a new conversation.

## Hardware Components

The project utilizes the following hardware components:

- ESP32 board: This serves as the main controller for the voice assistant.
- INMP441 microphone: It captures the user's voice input.
- I2S speaker: The voice assistant's responses are played through this speaker.
- RGB light: The light indicates the different states of the voice assistant, such as waiting for the wake word, processing audio, and providing responses.

## Training the Wake Word

The wake word 'Marvin' is trained using the TensorFlow library. Approximately 2000 different WAV files are used to generate spectrograms, which are then converted to hexadecimal characters.

Please refer to the project documentation for more detailed information.
To use this voice assistant, you will need to obtain the GPT API and Google API credentials. Once you have obtained these credentials, replace the placeholders "GPT_API_HERE" and "GOOGLE_API_HERE" in the code with your actual API keys.

Please note that the GPT API is used to process the transcribed text, while the Google API is used for voice-to-text purposes.

For detailed instructions on how to obtain the API credentials and replace them in the code, please refer to the project documentation.
Before uploading the code to the ESP32 board, make sure to update the following parameters:

1. SSID and Password: Replace the placeholders "YOUR_SSID" and "YOUR_PASSWORD" with your actual Wi-Fi network name and password in the code.

2. COMX: Replace "COMX" with the appropriate COM port number where the USB of the ESP32 board is plugged into your computer.

By updating these parameters, you will ensure that the voice assistant can connect to your Wi-Fi network and communicate with the ESP32 board correctly.

Remember to save the changes before uploading the code to the ESP32 board.
