#ifndef _gpt_to_speech_state_h_
#define _gpt_to_speech_state_h_
#include <string.h>
#include "States.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "Audio.h"

class TxtToSpeech : public State
{
private:
    String* gpt_answer;
    const char* apiKey;
    String* response;
    Audio* audio;

public:
    TxtToSpeech(String* gpt_request);
    void enterState();
    String* get_response();
    bool run();
    void exitState();
};

#endif
