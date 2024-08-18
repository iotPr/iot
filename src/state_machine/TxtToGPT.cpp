#include <Arduino.h>
#include "TxtToGPT.h"

#define WINDOW_SIZE 320
#define STEP_SIZE 160
#define POOLING_SIZE 6
#define AUDIO_LENGTH 16000

TxtToGPT::TxtToGPT(String* gpt_request)
{
    gpt_request = gpt_request;
    apiKey = "";
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
    HTTPClient http;
    http.begin("https://api.openai.com/v1/chat/completions");  // Correct endpoint for chat-based interactions
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));
    String hello = "hello";
    String payload = "{"
                      "\"model\": \"gpt-4\","
                      "\"messages\": [{\"role\": \"user\", \"content\": \"" + hello + "\"}],"
                      "\"max_tokens\": 50"
                    "}";

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        answer = http.getString();
        // Serial.println(httpResponseCode);
        // Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    return true;
}
void TxtToGPT::exitState()
{
    // Create our neural network
    uint32_t free_ram = esp_get_free_heap_size();
    Serial.printf("Free ram after TXTTOGPT cleanup %d\n", free_ram);
    // delete gpt_request;
}
  
String TxtToGPT::get_response()
{
    return answer;
}
