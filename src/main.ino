#include "Arduino.h"
// #include <driver/i2s.h>
// #include <SPIFFS.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <I2S.h>
// #include <SPIFFS.h>
// #include <SD.h>

// #include "../Scripts/audio_test.cpp"
// #include "../Scripts/speech_to_text_2.cpp"
// #include <driver/i2s.h>
// #include "../Scripts/create_WAV.cpp"


// #include "../Scripts/send_msg_to_gpt.cpp"
// #include <driver/i2s.h>
// #include <SD.h>
// #include "../Scripts/create_WAV.cpp"

// const int micPin = A0;         // Microphone connected to analog pin A0
// const int sampleRate = 8000;   // Sample rate (samples per second)
// const int bufferSize = 12800;    // Buffer size for storing samples

// int16_t buffer[bufferSize];    // Buffer to hold audio samples

// void setup() {
//     Serial.begin(115200);      // Initialize serial communication
//     pinMode(micPin, INPUT);    // Set microphone pin as input
// }

// void loop() {
//     // Read audio samples from microphone
//     for (int i = 0; i < bufferSize; i++) {
//         buffer[i] = analogRead(micPin);  // Read analog sample
//         delayMicroseconds(1000000 / sampleRate);  // Adjust for sampling rate
//     }

//     // Send buffer over serial to computer
//     Serial.write((uint8_t *)buffer, bufferSize * sizeof(int16_t));
// }
// Pin configurations for INMP441
#include "driver/ledc.h"

// Define the pin where the speaker is attached
const int speakerPin = 25; // Change this to the pin you are using

// Define the frequency of the beep
const int freq = 2000; // Frequency in Hz

// Define the channel to be used by the LED PWM Controller (0-15)
const int channel = 0;

// Define the resolution in bits
const int resolution = 8; // 8-bit resolution (0-255)

void setup() {
  // Set up the PWM channel for the speaker
  ledcSetup(channel, freq, resolution);
  // Attach the channel to the GPIO to be controlled
  ledcAttachPin(speakerPin, channel);
}

void loop() {
  // Start the beep
  ledcWrite(channel, 125); // 50% duty cycle to generate a square wave
  delay(500); // Beep for 500 milliseconds

  // Stop the beep
  ledcWrite(channel, 0); // 0% duty cycle to stop the sound
  delay(500); // Pause for 500 milliseconds
}