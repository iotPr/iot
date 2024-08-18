#ifndef _CLOUDSPEECHCLIENT_H
#define _CLOUDSPEECHCLIENT_H
#include <WiFiClientSecure.h>
#include "Audio.h"

enum Authentication {
  USE_ACCESSTOKEN,
  USE_APIKEY
};

class CloudSpeechClient {
  I2SSampler *m_sample_provider;
  WiFiClientSecure client;
  void PrintHttpBody2(Audio* audio);
  Authentication authentication;

public:
  CloudSpeechClient(Authentication authentication, I2SSampler *m_sample_provider);
  ~CloudSpeechClient();
  void Transcribe();
  void sendAudioChunks();
  void CreateWavHeader(byte* header, int wavDataSize);
};

#endif // _CLOUDSPEECHCLIENT_H
