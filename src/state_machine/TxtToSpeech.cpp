#include <Arduino.h>
#include "TxtToSpeech.h"
#include <ArduinoJson.h>
#include "Audio.h"
#include <vector>

#define I2S_DOUT      25
#define I2S_BCLK      27
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
    audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio->setVolume(21); // 0...21
}
bool TxtToSpeech::run()
{
    String Answer = *this->gpt_answer;
    std::vector<String> words = divideIntoWords(Answer);
    int max_length = 40;
    delete this->gpt_answer;
    // Output the words
    String current_word = ""; 
    for (const auto& word : words) {
        current_word = current_word + " " + word;
        if (current_word.length() >= max_length || (&word == &words[words.size()-1]))
        {
            Serial.println(current_word);
            audio->connecttospeech(current_word.c_str(), "en");
            // delay(1000);  // Add delay here
            current_word = "";
        }
        else
            continue;

        while(audio->isRunning()){
            audio->loop();
        }
    }
    audio->loop();
    return true;
}


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
