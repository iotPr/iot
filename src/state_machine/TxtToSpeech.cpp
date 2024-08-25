#include <Arduino.h>
#include "TxtToSpeech.h"
#include <ArduinoJson.h>
#include "Audio.h"
#include <vector>
#include "WiFiMulti.h"

#define I2S_DOUT      25
#define I2S_BCLK      14
#define I2S_LRC       26

std::vector<String> divideIntoWords(const String& str) {
    std::vector<String> words;
    int startIndex = 0;
    int spaceIndex = str.indexOf(' ');

    // Loop through the string to find spaces and extract words
    while (spaceIndex != -1) {
        words.push_back(str.substring(startIndex, spaceIndex));
        startIndex = spaceIndex + 1;
        spaceIndex = str.indexOf(' ', startIndex);
    }
    
    // Add the last word (or the entire string if no spaces were found)
    words.push_back(str.substring(startIndex));

    return words;
}




TxtToSpeech::TxtToSpeech(String* gpt_answer)
{
    this->gpt_answer = gpt_answer;
    this->audio = new Audio(false, 3, I2S_NUM_1);
}
void TxtToSpeech::enterState()
{
//     WiFiMulti wifiMulti;
//     wifiMulti.addAP("Home04", "13243546");
//     wifiMulti.run();
//     if(WiFi.status() != WL_CONNECTED){
//       WiFi.disconnect(true);
//       wifiMulti.run();
//   }


    audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

    audio->setVolume(18); // 0...21
    audio->connecttospeech("Starting", "en"); // Google TTS
}
bool TxtToSpeech::run()
{
    String Answer = *this->gpt_answer;
    std::vector<String> words = divideIntoWords(Answer);
    int max_length = 20;
    delete this->gpt_answer;
    // Output the words
    String current_word = ""; 
    for (const auto& word : words) {
        current_word = current_word + " " + word;
        if (current_word.length() >= max_length)
        {
            Serial.println(current_word);
            audio->connecttospeech(current_word.c_str(), "en");
            current_word = "";
        }
        else
        {
            if (&word == &words[words.size()-1])
            {
                Serial.println(current_word);
                audio->connecttospeech(current_word.c_str(), "en");
            }
            else
                continue;
        }
        while(audio->isRunning()){
            audio->loop();
        }
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
    delete audio;
    return;
}
  
String* TxtToSpeech::get_response()
{
    return nullptr;
}
