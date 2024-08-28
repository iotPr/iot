#include <Arduino.h>
#include "Application.h"
#include "state_machine/DetectWakeWordState.h"

// #include "IndicatorLight.h"
// #include "Speaker.h"
// #include "IntentProcessor.h"
#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13


Application::Application(I2SSampler *sample_provider)
{
    m_sample_provider = sample_provider;
    m_current_state = new DetectWakeWordState(m_sample_provider);
    m_current_state->enterState();
}

// process the next batch of samples
void Application::run()
{   
    bool state_done = m_current_state->run();

    if (state_done)
    {
        m_current_state->exitState();
        // Application::set_next_state();
        // m_current_state->enterState();
    }
    vTaskDelay(10);
}

// void Application::set_next_state()
// {
//     State* pre_state = m_current_state;
//     if (m_current_state_name == StateNames::DETECTWAKEWORD)
//     {
//         delete pre_state;
//         m_current_state_name = StateNames::SPEECHTOTXT;
//         m_current_state = new SpeechToText(m_sample_provider);
//     }
//     else if (m_current_state_name == StateNames::SPEECHTOTXT)
//     {
//         String* gpt_q = m_current_state->get_response();
//         delete pre_state;
//         if (*gpt_q == "Please Try Again")
//         {
//             m_current_state_name = StateNames::TXTTOSPEECH;
//             m_current_state = new TxtToSpeech(gpt_q);
//         }
//         else
//         {
//             m_current_state_name = StateNames::TXTTOGPT;
//             m_current_state = new TxtToGPT(gpt_q);
//         }
    // }
    // }
    // else if (m_current_state_name == StateNames::TXTTOGPT)
    // {
    //     String* gpt_answer = m_current_state->get_response();
    //     delete pre_state;
    //     m_current_state_name = StateNames::TXTTOSPEECH;
    //     m_current_state = new TxtToSpeech(gpt_answer);
    // }
    // else if (m_current_state_name == StateNames::TXTTOSPEECH)
    // {
    //     ESP.restart();
    // }
// }

Application::~Application()
{
    // delete m_sample_provider;
    delete m_current_state;
}
