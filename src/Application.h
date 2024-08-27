#ifndef _application_h_
#define _applicaiton_h_

#include "state_machine/States.h"
#include <string.h>
class I2SSampler;


class Application
{
private:
    State* m_current_state;
    void set_next_state();


public:
    I2SSampler* m_sample_provider;
    Application(I2SSampler *sample_provider);
    ~Application();
    void run();
};

#endif