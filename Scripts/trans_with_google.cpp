

#define I2S_WS  15
#define I2S_SD  32
#define I2S_SCK 14

const char* ssid = "Yag";
const char* password = "0545624950";
const char* google_api_key = "7cc56355f89a88b31b3180442c7b62172f0760cd";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Setup I2S
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void loop() {
  // Open file in SPIFFS to write
  File file = SPIFFS.open("/audio.wav", FILE_WRITE);

  // Write WAV header (simple 16-bit PCM format)
  uint8_t wav_header[] = {
    0x52, 0x49, 0x46, 0x46, // "RIFF"
    0x00, 0x00, 0x00, 0x00, // File size (will be updated later)
    0x57, 0x41, 0x56, 0x45, // "WAVE"
    0x66, 0x6d, 0x74, 0x20, // "fmt "
    0x10, 0x00, 0x00, 0x00, // Subchunk1 size (16 for PCM)
    0x01, 0x00, 0x01, 0x00, // Audio format (1 for PCM), Num channels (1)
    0x80, 0x3e, 0x00, 0x00, // Sample rate (16000)
    0x00, 0x7d, 0x00, 0x00, // Byte rate (SampleRate * NumChannels * BitsPerSample/8)
    0x02, 0x00, 0x10, 0x00, // Block align, Bits per sample
    0x64, 0x61, 0x74, 0x61, // "data"
    0x00, 0x00, 0x00, 0x00  // Subchunk2 size (NumSamples * NumChannels * BitsPerSample/8)
  };

  file.write(wav_header, sizeof(wav_header));

  // Record audio for 5 seconds
  int seconds_to_record = 5;
  int sample_rate = 16000;
  int samples_to_record = seconds_to_record * sample_rate;
  int16_t sample_buffer[1024];
  size_t bytes_written;

  for (int i = 0; i < samples_to_record; i += 1024) {
    i2s_read(I2S_NUM_0, sample_buffer, sizeof(sample_buffer), &bytes_written, portMAX_DELAY);
    file.write((uint8_t*)sample_buffer, bytes_written);
  }

  // Update WAV header with correct file sizes
  int file_size = file.size();
  file.seek(4);
  uint32_t file_size_minus_8 = file_size - 8;
  file.write((uint8_t*)&file_size_minus_8, 4);
  file.seek(40);
  uint32_t data_size = file_size - 44;
  file.write((uint8_t*)&data_size, 4);

  file.close();

  // Send file to Google Cloud
  sendFileToGoogleCloud("/audio.wav");

  // Print result to Serial
  Serial.println("Transcription: ");
}

void sendFileToGoogleCloud(const char* path) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://speech.googleapis.com/v1/speech:recognize?key=" + String(google_api_key));
    http.addHeader("Content-Type", "application/json");

    // Prepare the request payload
    String payload = "{\"config\": {\"encoding\": \"LINEAR16\", \"sampleRateHertz\": 16000, \"languageCode\": \"en-US\"}, \"audio\": {\"content\": \"";
    
    // Read the file and base64 encode it
    File file = SPIFFS.open(path, FILE_READ);
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }
    
    // Base64 encoding (assuming the file is small enough to fit in memory)
    String base64_audio = "";
    while (file.available()) {
      size_t bytes_read = file.readBytes(buffer, sizeof(buffer));
      base64_audio += base64::encode((uint8_t*)buffer, bytes_read);
    }
    file.close();

    payload += base64_audio + "\"}}";

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);

      // Parse the response to get the transcription
      // You may need to include a JSON library to parse the response
      // Example: DynamicJsonDocument doc(1024);
      // deserializeJson(doc, response);
      // const char* transcription = doc["results"][0]["alternatives"][0]["transcript"];
      // Serial.println(transcription);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
