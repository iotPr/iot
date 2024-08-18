#ifndef _states_h_
#define _states_h_

class State
{
public:
    virtual void enterState() = 0;
    virtual bool run() = 0;
    virtual void exitState() = 0;
    virtual String get_response() = 0; 
};

#endif