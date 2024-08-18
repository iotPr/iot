#include <Arduino.h>
// #include <ArduinoJson.h>
#include "I2SSampler.h"
// #include "RingBuffer.h"
#include "WokeWordDetected.h"
// #include "IndicatorLight.h"
// #include "Speaker.h"
// #include "IntentProcessor.h"
// #include "WitAiChunkedUploader.h"
#include "../config.h"
#include <string.h>
#include "speech_to_text/Audio.h"
#include "speech_to_text/CloudSpeechClient.h"

#define WINDOW_SIZE 320
#define STEP_SIZE 160
#define POOLING_SIZE 6
#define AUDIO_LENGTH 16000

WokeWordDetected::WokeWordDetected(I2SSampler *sample_provider)
{
    // save the sample provider for use later
    m_sample_provider = sample_provider;
}
void WokeWordDetected::enterState()
{
    Serial.println("Ready for action");
}

void WokeWordDetected::speech_to_text()
{
    Serial.println("\r\nRecord start!\r\n");
    // Audio* audio = new Audio(m_sample_provider);
    // audio->Record();
    Serial.println("Recording Completed. Now Processing...");
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY, m_sample_provider);
    cloudSpeechClient->Transcribe();
    delete cloudSpeechClient;
    // delete audio;
}
bool WokeWordDetected::run()
{
    for (int i=0; i<2; i++)
    {
    digitalWrite(2, HIGH);
    // Wait for a second
    delay(1000);
    // Turn the LED off
    digitalWrite(2, LOW);
    // Wait for a second
    delay(1000);
    }
    speech_to_text();
    return true;

}

void WokeWordDetected::exitState()
{
    // clean up the speech recognizer client as it takes up a lot of RAM
    uint32_t free_ram = esp_get_free_heap_size();
    Serial.printf("Free ram after request %d\n", free_ram);
}

String WokeWordDetected::get_response(){return response;}