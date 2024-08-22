#include <Arduino.h>
#include "TxtToSpeech.h"
#include <ArduinoJson.h>
#include "Audio.h"
#include "WiFiMulti.h"

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26


TxtToSpeech::TxtToSpeech(String* gpt_answer)
{

    this->gpt_answer = gpt_answer;
    Serial.printf("I'm in txt to speech:\n");
    Serial.println(*this->gpt_answer);
    apiKey = "AIzaSyAIgezZitJr_mpSK-7bkC7GPstBSTiR9-M";
}
void TxtToSpeech::enterState()
{
    Audio audio;
    WiFiMulti wifiMulti;
    wifiMulti.addAP("Home04", "13243546");
    wifiMulti.run();
    if(WiFi.status() != WL_CONNECTED){
      WiFi.disconnect(true);
      wifiMulti.run();
  }

    Serial.printf("I'm in txt to speech2:\n");

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    Serial.printf("I'm in txt to speech3:\n");

    audio.setVolume(18); // 0...21
    audio.connecttospeech("Starting", "en"); // Google TTS
    Serial.printf("I'm in txt to speec4:\n");


}
bool TxtToSpeech::run()
{
    String Answer = "this is fake answer";
    Audio audio;

    //-----
        // Split the answer into chunks and send each chunk to connecttospeech
    size_t chunkSize = 80;  // Define chunk size (adjust if necessary)
    for (size_t i = 0; i < Answer.length(); i += chunkSize) {

        String chunk = Answer.substring(i, (i + chunkSize));
        Serial.println(chunk);
        audio.connecttospeech(chunk.c_str(), "en");

        while(audio.isRunning()){
            audio.loop();
        }
    }
    audio.loop();
    return true;
}
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
