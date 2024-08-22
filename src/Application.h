#ifndef _application_h_
#define _applicaiton_h_

#include "state_machine/States.h"
#include <string.h>
class I2SSampler;
class I2SOutput;
class State;
class IndicatorLight;
class Speaker;
class IntentProcessor;

enum StateNames {
    DETECTWAKEWORD,    // 0
    SPEECHTOTXT,  // 1
    TXTTOGPT,          // 2
    TXTTOSPEECH        // 3
};

class Application
{
private:
    State* m_current_state;
    I2SSampler* m_sample_provider;
    StateNames m_current_state_name;
    std::string* m_txt_from_google;
    std::string* m_gpt_answer;
    void set_next_state();


public:
    Application(I2SSampler *sample_provider);
    ~Application();
    void run();
};

#endif