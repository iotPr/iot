#include <Arduino.h>
// #include <ArduinoJson.h>
#include "I2SSampler.h"
// #include "RingBuffer.h"
#include "SpeechToText.h"
// #include "IndicatorLight.h"
// #include "Speaker.h"
// #include "IntentProcessor.h"
// #include "WitAiChunkedUploader.h"
#include "../config.h"
#include <string.h>
#include "speech_to_text/CloudSpeechClient.h"

SpeechToText::SpeechToText()
{
    // save the sample provider for use later
}
void SpeechToText::enterState()
{
    Serial.println("Entering Speech to Text State");
}

void SpeechToText::speech_to_text()
{
    Serial.println("In speech to text");
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
    Serial.println("Creating client obj");
    response = cloudSpeechClient->Transcribe();
    Serial.printf("Finish transcribe\n");
    if (*response == "" || response == nullptr)
    {
        Serial.printf("Google Speech couldnt catch any speech\n");
        String* error = response;
        response = new String("Please Try Again");
        delete error;
    }
    delete cloudSpeechClient;
    Serial.printf("After deleteing client\n");
}
bool SpeechToText::run()
{
    Serial.printf("in run stt\n");
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
    Serial.printf("in run stt2\n");
    speech_to_text();
    Serial.printf("After speech to text\n");
    return true;
}

void SpeechToText::exitState()
{
    Serial.printf("In exit wokeword\n");
}

String* SpeechToText::get_response(){return response;}