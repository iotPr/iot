#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>

#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13

int read_from_mic(uint8_t* data, int numData) {
    size_t bytes_read = 0;
    esp_err_t result = i2s_read(I2S_NUM_0, (void*)data, numData, &bytes_read, portMAX_DELAY);
    
    // Check if the read was successful
    if (result == ESP_OK) {
        return bytes_read;
    } else {
        // Handle the error case (you can choose what to return in case of an error)
        Serial.printf("Error occured! Couldn't read data from mic");
        return -1; // or any other error indicator
    }
}


CloudSpeechClient::CloudSpeechClient() 
{
  client.setCACert(root_ca);
  int trys_to_connect = 3;
  while(!client.connect(server, 443) && trys_to_connect > 0)
   {trys_to_connect--;}
  //todo: add handaling if try to connec is 0. meaning couldnt connect even after few trys.

}


CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
}


String* CloudSpeechClient::Transcribe() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection");
  }
  String SpeechtoText_Language = "en-US"; // Language 
  // build http format, after "content:" will go our record. 
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":44100,\"languageCode\":\"" + SpeechtoText_Language + "\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n\r\n";
  int record_size = 270000;
  int httpBody2Length = (record_size + 48) * 4 / 3; // 4/3 is from base64 encoding

  // content length is super important!!! If it's not accurate then google won't response to any request
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());


  String HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  client.print(HttpHeader);
  client.print(HttpBody1);

// start creating the wav and record
  int buffer_size = 1200;
  byte header[44 + 4] = {0}; 
  byte data[buffer_size];
  CloudSpeechClient::CreateWavHeader(header, record_size);
  String enc = base64::encode(header, 48);
  enc.replace("\n", "");
  client.print(enc);

  //start recording 
  digitalWrite(2, HIGH);
  Serial.printf("Start speaking\n");
  digitalWrite(BLUE_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  for (int i = 0; i<record_size/buffer_size; i++)
  {
    int data_read = read_from_mic(data, buffer_size);
    enc = base64::encode(data, buffer_size);
    enc.replace("\n", "");
    client.print(enc);
  }
  Serial.printf("Stop speaking\n");
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(2, LOW);
  //stop recording 
  client.print(HttpBody3);
  String My_Answer = "";
  while (!client.available())
  {
        if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection");
  }
  };

  while (client.available())
  {
    char temp = client.read();
    My_Answer = My_Answer + temp;
    // Serial.write(client.read());
  }
client.stop();
digitalWrite(BLUE_PIN, HIGH);
digitalWrite(GREEN_PIN, LOW);
digitalWrite(RED_PIN, LOW);
Serial.print("My Answer - ");Serial.println(My_Answer);
int postion = My_Answer.indexOf('{');
Serial.println(postion);
String ans = My_Answer.substring(postion);
Serial.print("Json daata--");
Serial.print(ans);
DynamicJsonDocument doc(1024);


DeserializationError error = deserializeJson(doc, ans);
if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return new String("Sorry I couldn't understand. Please tell me again!");
}
JsonObject results_0 = doc["results"][0];
//const char*
const char* chatgpt_Q = results_0["alternatives"][0]["transcript"];
String* chat_gpt_q = new String(chatgpt_Q);
Serial.println(*chat_gpt_q);
return chat_gpt_q;
}

void CloudSpeechClient::sendAudioChunks()
{
  return;
}

void CloudSpeechClient::CreateWavHeader(byte* header, int waveDataSize){

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
  header[24] = 0x44;  // sampling rate 44100
  header[25] = 0xAC;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x88;  // Byte/sec = 44100x2x1 = 88200
  header[29] = 0x58;
  header[30] = 0x01;
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
}
