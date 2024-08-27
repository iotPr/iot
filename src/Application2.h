#ifndef _application2_h_
#define _applicaiton2_h_

#include "state_machine/States.h"
#include <string.h>
#include <base64.h>
#include <ArduinoJson.h>


class State;


enum StateNames {
    DETECTWAKEWORD,    // 0
    SPEECHTOTXT,  // 1
    TXTTOGPT,          // 2
    TXTTOSPEECH        // 3
};

class Phase2
{
private:
    State* m_current_state;
    StateNames m_current_state_name;
    void set_next_state();


public:
    Phase2();
    ~Phase2();
    void run();
};

#endif