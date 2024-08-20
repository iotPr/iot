#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>
#include "Audio.h"

CloudSpeechClient::CloudSpeechClient(Authentication authentication, I2SSampler *m_sample_provider) 
{
  this->m_sample_provider = m_sample_provider;
  this->authentication = authentication;
  Serial.printf("Free heap before connection: %d\n", ESP.getFreeHeap());
  client.setCACert(root_ca);
  Serial.printf("Free heap after connection: %d\n", ESP.getFreeHeap());
  
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
  return;
}

String CloudSpeechClient::Transcribe() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection");
  }
  String SpeechtoText_Language = "en-US"; // Language 

  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":44100,\"languageCode\":\"" + SpeechtoText_Language + "\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n\r\n";
  int record_size = 270000;
  int httpBody2Length = (record_size + 48) * 4 / 3; // 4/3 is from base64 encoding
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader;
  // if (authentication == USE_APIKEY)
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  //  else if (authentication == USE_ACCESSTOKEN)
  //    HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
  //   + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  client.print(HttpHeader);
  client.print(HttpBody1);


  //  else if (authentication == USE_ACCESSTOKEN)
  //    HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
  //   + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  int buffer_size = 1200;
  int dividedWavDataSize = buffer_size/4;
  byte header[44];
  byte data[buffer_size];
  CloudSpeechClient::CreateWavHeader(header, record_size);
  String enc = base64::encode(header, 48);
  enc.replace("\n", "");
  client.print(enc);
  digitalWrite(2, HIGH);
  for (int i = 0; i<record_size/buffer_size; i++)
  {

    int data_read = m_sample_provider->Read(data, buffer_size);
    enc = base64::encode(data, buffer_size);
    enc.replace("\n", "");
    client.print(enc);
  }

  digitalWrite(2, LOW);

  client.print(HttpBody3);
  String My_Answer = "";
  Serial.printf("Im in transcribe8\n");
  while (!client.available())
  {
        if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection");
  }
  };
  Serial.printf("Im in transcribe9\n");

  while (client.available())
  {
    char temp = client.read();
    My_Answer = My_Answer + temp;
    // Serial.write(client.read());
  }
client.stop();
// Serial.print("My Answer - ");Serial.println(My_Answer);
int postion = My_Answer.indexOf('{');
// Serial.println(postion);
ans = My_Answer.substring(postion);
Serial.print("Json daata--");
Serial.print(ans);
Serial.printf("Im in transcribe10\n");
DynamicJsonDocument doc(1024);

//StaticJsonDocument<384> doc;
Serial.printf("Im in transcribe11\n");

DeserializationError error = deserializeJson(doc, ans);
if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return String("Sorry I couldn't understand. Please tell me again!");
}
JsonObject results_0 = doc["results"][0];
//const char*
const char* chatgpt_Q = results_0["alternatives"][0]["transcript"];
String chat_gpt_q = String(chatgpt_Q);
return chat_gpt_q;
}

void CloudSpeechClient::sendAudioChunks()
{
  return;
}

void CloudSpeechClient::CreateWavHeader(byte* header, int waveDataSize){
  Serial.printf("Im in wavheader1");

  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSizeMinus8 = waveDataSize + 44 - 8;
  header[4] = (byte)(fileSizeMinus8 & 0xFF);
  header[5] = (byte)((fileSizeMinus8 >> 8) & 0xFF);
  header[6] = (byte)((fileSizeMinus8 >> 16) & 0xFF);
  header[7] = (byte)((fileSizeMinus8 >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;  // linear PCM
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;  // linear PCM
  header[21] = 0x00;
  header[22] = 0x01;  // monoral
  header[23] = 0x00;
  header[24] = 0x44;  // sampling rate 16000
  header[25] = 0xAC;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;  // Byte/sec = 16000x2x1 = 32000
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;  // 16bit monoral
  header[33] = 0x00;
  header[34] = 0x10;  // 16bit
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(waveDataSize & 0xFF);
  header[41] = (byte)((waveDataSize >> 8) & 0xFF);
  header[42] = (byte)((waveDataSize >> 16) & 0xFF);
  header[43] = (byte)((waveDataSize >> 24) & 0xFF);
  header[44] = 0;
  header[45] = 0;
  header[46] = 0;
  header[47] = 0;

  Serial.printf("Im in wavheader2");
}
