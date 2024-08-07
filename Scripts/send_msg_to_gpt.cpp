
WiFiMulti wifiMulti;
String ssid = "yag";
String password = "0545624950";
String apiKey = "sk-None-NWc1XdBugZqGlL46oYVhT3BlbkFJ8ornaGsYwuZsSDljtReU";

void sendTextToOpenAI(String inputText);
void setup() {
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid.c_str(), password.c_str());
  wifiMulti.run();
  Serial.begin(115200);
  // WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect(true);
    wifiMulti.run();
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");

  sendTextToOpenAI("Hello, GPT!");
}

void loop() {
  // Your main code here
}

void sendTextToOpenAI(String inputText) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.openai.com/v1/completions");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));

    String payload = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"" + inputText + "\"}], \"max_tokens\": 50}";    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}
