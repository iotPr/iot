#include "Arduino.h"
#include <driver/i2s.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../Scripts/speech_to_text_2.cpp"

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