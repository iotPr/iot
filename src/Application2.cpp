#include <Arduino.h>
#include "Application2.h"
#include "state_machine/DetectWakeWordState.h"
#include "state_machine/SpeechToText.h"
#include "state_machine/TxtToGPT.h"
#include "state_machine/TxtToSpeech.h"

void Phase2::addMessageToHistory(const String& role, String& content) {
    if (role == "user")
        content = content + String(", answer in one sentence");
    String message = "{\"role\": \"" + role + "\", \"content\": \"" + content + "\"}";
    this->conversationHistory.push_back(message);
}

String Phase2::buildPayload() {
    String messagesArray = "[";
    for (size_t i = 0; i < this->conversationHistory.size(); ++i) {
        messagesArray += this->conversationHistory[i];
        if (i != this->conversationHistory.size() - 1) {
            messagesArray += ", ";
        }
    }
    messagesArray += "]";

    String payload = "{"
                      "\"model\": \"gpt-4\","
                      "\"messages\": " + messagesArray + ","
                      "\"max_tokens\": 50"
                    "}";

    return payload;
}

Phase2::Phase2()
{
    stt_client = new CloudSpeechClient();
    m_current_state = new SpeechToText(stt_client);
    m_current_state_name = StateNames::SPEECHTOTXT;
    m_current_state->enterState();
}

// process the next batch of samples
void Phase2::run()
{   
    bool state_done = m_current_state->run();
    if (state_done)
    {
        m_current_state->exitState();
        Phase2::set_next_state();
        m_current_state->enterState();
    }
    vTaskDelay(10);
}

void Phase2::set_next_state()
{
    State* pre_state = m_current_state;
    if (m_current_state_name == StateNames::SPEECHTOTXT)
    {
        String* gpt_q = m_current_state->get_response();
        delete pre_state;
        if (*gpt_q == "Please Try Again")
        {
            ESP.restart();
        }
        else
        {
            this->addMessageToHistory("user", *gpt_q);
            String payload = this->buildPayload();
            m_current_state_name = StateNames::TXTTOGPT;
            m_current_state = new TxtToGPT(payload);
        }
    }
    else if (m_current_state_name == StateNames::TXTTOGPT)
    {
        String* gpt_answer = m_current_state->get_response();
        delete pre_state;
        this->addMessageToHistory("assistant", *gpt_answer);
        m_current_state_name = StateNames::TXTTOSPEECH;
        m_current_state = new TxtToSpeech(gpt_answer);
    }
    else if (m_current_state_name == StateNames::TXTTOSPEECH)
    {
        delete pre_state;
        m_current_state = new SpeechToText(this->stt_client);
        m_current_state_name = StateNames::SPEECHTOTXT;
    }
}


