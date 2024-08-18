#include <Arduino.h>
#include "Application.h"
#include "state_machine/DetectWakeWordState.h"
#include "state_machine/WokeWordDetected.h"
#include "state_machine/TxtToGPT.h"

// #include "IndicatorLight.h"
// #include "Speaker.h"
// #include "IntentProcessor.h"

Application::Application(I2SSampler *sample_provider)
{
    m_sample_provider = sample_provider;
    m_current_state = new DetectWakeWordState(m_sample_provider);
    m_current_state_name = StateNames::DETECTWAKEWORD;
    m_current_state->enterState();
}

// process the next batch of samples
void Application::run()
{
    bool state_done = m_current_state->run();
    if (state_done)
    {
        m_current_state->exitState();
        Application::set_next_state();
        m_current_state->enterState();
    }
    // vTaskDelay(10);
}

void Application::set_next_state()
{
    State* pre_state = m_current_state;
    if (m_current_state_name == StateNames::DETECTWAKEWORD)
    {
        m_current_state_name = StateNames::WAKEWORDDETECTED;
        m_current_state = new WokeWordDetected(m_sample_provider);
    }
    else if (m_current_state_name == StateNames::WAKEWORDDETECTED)
    {
        String* str = new String("Hello World");
        m_current_state_name = StateNames::TXTTOGPT;
        m_current_state = new TxtToGPT(str);
    }
    else if (m_current_state_name == StateNames::TXTTOGPT)
    {
        Serial.printf("!!!!!!!!!!!!!!!!!!!!!!!!!!");
        Serial.println(m_current_state->get_response());
        m_current_state_name = StateNames::TXTTOSPEECH;
    }
    delete pre_state; 
}


