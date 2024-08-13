#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>
#include "Audio.h"

CloudSpeechClient::CloudSpeechClient(Authentication authentication) 
{
  this->authentication = authentication;
  Serial.printf("Free heap before connection: %d\n", ESP.getFreeHeap());

  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) delay(1000);
  client.setCACert(root_ca);
  Serial.printf("Free heap before connection: %d\n", ESP.getFreeHeap());
  
  if (!client.connect(server, 443)) Serial.println("Connection failed!");
  Serial.printf("Free heap before connection: %d\n", ESP.getFreeHeap());

}

String ans;

CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
  WiFi.disconnect();
}

void CloudSpeechClient::PrintHttpBody2(Audio* audio)
{
  String enc = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
  enc.replace("\n", "");  // delete last "\n"
  client.print(enc);      // HttpBody2
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    enc.replace("\n", "");// delete last "\n"
    client.print(enc);    // HttpBody2
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  Serial.printf("Im in transcribe");
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-US\"},\"audio\":{\"content\":\"";
  Serial.printf("Im in transcribe2");

  String HttpBody3 = "\"}}\r\n\r\n";
  Serial.printf("Im in transcribe3");

  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // 4/3 is from base64 encoding
    Serial.printf("Im in transcribe4");

  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader;
  // if (authentication == USE_APIKEY)
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  //  else if (authentication == USE_ACCESSTOKEN)
  //    HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
  //   + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  Serial.printf("Im in transcribe5\n");

  client.print(HttpHeader);
  client.print(HttpBody1);
  PrintHttpBody2(audio);
  client.print(HttpBody3);
  String My_Answer="";
  unsigned long startMillis = millis();
  unsigned long timeout = 10000; // 10 seconds timeout
  while (!client.available())
  { 
      if (millis() - startMillis >= timeout) {
        Serial.println("Timeout waiting for client to become available");
        return;  // Or handle the timeout appropriately
    }
  }
  Serial.printf("Im in transcribe10\n");
  while (client.available())
  {
    char temp = client.read();
    My_Answer = My_Answer + temp;
  //  Serial.write(client.read());
  }
  Serial.printf("Im in transcribe11\n");
  Serial.print("My Answer - ");Serial.println(My_Answer);
  int postion = My_Answer.indexOf('{');
  Serial.println(postion);
  ans = My_Answer.substring(postion);
  Serial.print("Json daata--");
  Serial.print(ans);
}
