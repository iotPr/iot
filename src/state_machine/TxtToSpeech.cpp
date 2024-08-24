#include <Arduino.h>
#include "TxtToSpeech.h"
#include <ArduinoJson.h>
#include "Audio.h"
#include <vector>
#include "WiFiMulti.h"

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

std::vector<String> divideIntoChunks(const String &str, size_t maxLength) {
    std::vector<String> chunks;
    String currentChunk = "";
    int length = str.length();
    int lastSpaceIndex = -1;

    for (int i = 0; i < length;) {
        // Track the last space index within the current chunk
        lastSpaceIndex = -1;

        // Build the chunk without exceeding the maxLength
        while (i < length && currentChunk.length() < maxLength) {
            char currentChar = str[i];
            if (currentChar == ' ') {
                lastSpaceIndex = i;
            }
            currentChunk += currentChar;
            i++;
        }

        // If the chunk exceeds maxLength and there was a space in it, split at the last space
        if (currentChunk.length() > maxLength && lastSpaceIndex != -1) {
            i = lastSpaceIndex + 1;  // Move the index back to the last space
            currentChunk.remove(currentChunk.lastIndexOf(' '));  // Remove the excess part
        }

        chunks.push_back(currentChunk);
        currentChunk = "";
    }

    // Add the last remaining chunk if there's any leftover text
    if (currentChunk.length() > 0) {
        chunks.push_back(currentChunk);
    }

    return chunks;
}



TxtToSpeech::TxtToSpeech(String* gpt_answer)
{
    this->gpt_answer = gpt_answer;
    Serial.printf("I'm in txt to speech:\n");
    Serial.println(*this->gpt_answer);
    this->audio = new Audio();
}
void TxtToSpeech::enterState()
{
    WiFiMulti wifiMulti;
    wifiMulti.addAP("Home04", "13243546");
    wifiMulti.run();
    if(WiFi.status() != WL_CONNECTED){
      WiFi.disconnect(true);
      wifiMulti.run();
  }

    Serial.printf("I'm in txt to speech2:\n");

    audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    Serial.printf("I'm in txt to speech3:\n");

    audio->setVolume(18); // 0...21
    audio->connecttospeech("Starting", "en"); // Google TTS
    Serial.printf("I'm in txt to speec4:\n");


}
bool TxtToSpeech::run()
{
    Serial.printf("Im in tts run1 ");
    String Answer = *this->gpt_answer;
    std::vector<String> words = divideIntoChunks(Answer, 20);

    // Output the words
    for (const auto& word : words) {
        Serial.println(word);
        audio->connecttospeech(word.c_str(), "en");

        while(audio->isRunning()){
            audio->loop();
        }
        delay(1000); 
    }
    audio->loop();
    return true;
}

    //-----
        // Split the answer into chunks and send each chunk to connecttospeech
    // size_t chunkSize = 80;  // Define chunk size (adjust if necessary)
    // for (size_t i = 0; i < Answer.length(); i += chunkSize) {

    //     String chunk = Answer.substring(i, (i + chunkSize));
    //     Serial.println(chunk);
    //     audio.connecttospeech(chunk.c_str(), "en");

    //     while(audio.isRunning()){
    //         audio.loop();
    //     }
    // }
    // audio.loop();
    // return true;

void TxtToSpeech::exitState()
{
    // Create our neural network
    // delete gpt_request;
    return;
}
  
String* TxtToSpeech::get_response()
{
    return nullptr;
}
