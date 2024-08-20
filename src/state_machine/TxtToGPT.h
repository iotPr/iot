#ifndef _gpt_to_txt_state_h_
#define _gpt_to_txt_state_h_
#include <string.h>
#include "States.h"
#include <WiFi.h>
#include <HTTPClient.h>

class TxtToGPT : public State
{
private:
    String* gpt_request;
    const char* apiKey;
    String* response;

public:
    TxtToGPT(String* gpt_request);
    void enterState();
    String* get_response();
    bool run();
    void exitState();
};

#endif
