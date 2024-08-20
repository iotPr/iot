#include <Arduino.h>
#include "TxtToGPT.h"
#include <ArduinoJson.h>

#define WINDOW_SIZE 320
#define STEP_SIZE 160
#define POOLING_SIZE 6
#define AUDIO_LENGTH 16000

TxtToGPT::TxtToGPT(String* gpt_request)
{
    this->gpt_request = gpt_request;
    Serial.println(*this->gpt_request);
    apiKey = "sk-Dem3XHGSgPVtMeJUO8iWZxQTVGrdJjMbq4czUPWGqZT3BlbkFJPFZeKbc6Gl6xxCSpRwjwAIkSOT83_ENmVKGDsCHRkA";
}
void TxtToGPT::enterState()
{
    return;
}
bool TxtToGPT::run()
{
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection");
  }
    Serial.printf("TXTTOGPT1\n");
    Serial.println(*gpt_request);
    String gpt_request_in_string = *gpt_request + String(", answer in one sentence");
    HTTPClient http;
    http.begin("https://api.openai.com/v1/chat/completions");  // Correct endpoint for chat-based interactions
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));
    String payload = "{"
                      "\"model\": \"gpt-4\","
                      "\"messages\": [{\"role\": \"user\", \"content\": \"" + gpt_request_in_string + "\"}],"
                      "\"max_tokens\": 50"
                    "}";
    delete gpt_request;
    Serial.printf("TXTTOGPT2\n");
    String gpt_ans;
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        gpt_ans = http.getString();
    } else {
        gpt_ans = "Error on sending POST";
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    DynamicJsonDocument doc(2048);


    DeserializationError error = deserializeJson(doc, gpt_ans);
    if (error) {
    // Serial.print("deserializeJson() failed: ");
    // Serial.println(error.c_str());
        return new String("Sorry I couldn't understand. Please tell me again!");
    }
    JsonObject results_0 = doc["choices"][0];
    //const char*
    const char* chatgpt_A = results_0["content"];

    this->response = new String(chatgpt_A);
    return true;
}
void TxtToGPT::exitState()
{
    // Create our neural network
    // delete gpt_request;
}
  
String* TxtToGPT::get_response()
{
    return this->response;
}
