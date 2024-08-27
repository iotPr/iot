#ifndef _CLOUDSPEECHCLIENT_H
#define _CLOUDSPEECHCLIENT_H
#include <WiFiClientSecure.h>
#include "I2SSampler.h"

enum Authentication {
  USE_ACCESSTOKEN,
  USE_APIKEY
};

class CloudSpeechClient {
  WiFiClientSecure client;
  Authentication authentication;

public:
  CloudSpeechClient(Authentication authentication);
  ~CloudSpeechClient();
  String* Transcribe();
  void sendAudioChunks();
  void CreateWavHeader(byte* header, int wavDataSize);
};

#endif // _CLOUDSPEECHCLIENT_H
