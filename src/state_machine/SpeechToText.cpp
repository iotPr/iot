#include <Arduino.h>
#include "SpeechToText.h"
#include "../config.h"

#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13

SpeechToText::SpeechToText()
{
    
}
void SpeechToText::enterState()
{
    Serial.println("Entering Speech to Text State");
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
}

void SpeechToText::speech_to_text()
{
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient();
    response = cloudSpeechClient->Transcribe();
    if (*response == "" || response == nullptr)
    {
        Serial.printf("Google Speech couldnt catch any speech\n");
        String* error = response;
        response = new String("Please Try Again");
        delete error;
    }
    delete cloudSpeechClient;
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