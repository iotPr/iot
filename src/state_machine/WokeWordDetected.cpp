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
    Serial.println("Entering Speech to Text State");
}

void WokeWordDetected::speech_to_text()
{
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY, m_sample_provider);
    response = cloudSpeechClient->Transcribe();
    Serial.printf("Finish transcribe");
    if (*response == "" || response == nullptr)
    {
        Serial.printf("Google Speech couldnt catch any speech");
    }
    delete cloudSpeechClient;
    Serial.printf("After deleteing client");
}
bool WokeWordDetected::run()
{
    for (int i=0; i<6; i++)
    {
    digitalWrite(2, HIGH);
    // Wait for a second
    delay(100);
    // Turn the LED off
    digitalWrite(2, LOW);
    // Wait for a second
    delay(100);
    }
    speech_to_text();
    Serial.printf("After speech to text\n");
    return true;
}

void WokeWordDetected::exitState()
{
    Serial.printf("In exit wokeword\n");
}

String* WokeWordDetected::get_response(){return response;}