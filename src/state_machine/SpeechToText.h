#ifndef _recognise_command_state_h_
#define _recognise_command_state_h_
#include "speech_to_text/CloudSpeechClient.h"
#include "States.h"

class I2SSampler;


class SpeechToText : public State
{
private:
    String* response; 
    CloudSpeechClient* client;
    void speech_to_text();
public:
    SpeechToText(CloudSpeechClient* client);
    String* get_response();
    void enterState();
    bool run();
    void exitState();
};

#endif
