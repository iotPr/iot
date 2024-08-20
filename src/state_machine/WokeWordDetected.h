#ifndef _recognise_command_state_h_
#define _recognise_command_state_h_

#include "States.h"

class I2SSampler;


class WokeWordDetected : public State
{
private:
    I2SSampler *m_sample_provider;
    String* response; 
    void speech_to_text();
public:
    WokeWordDetected(I2SSampler *sample_provider);
    String* get_response();
    void enterState();
    bool run();
    void exitState();
};

#endif
