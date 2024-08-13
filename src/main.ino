#include <driver/i2s.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
// #include <Base64.h>
#include <Arduino.h>


const char* ssid = "yag";
const char* password = "0545624950";
const char* apiKey = "sk-_tyTJ_g1PWrqKewxwB5Uc4Lqg7iv-HAPKpHNz6SLbdT3BlbkFJ_5lUhmv5pGkj9dPAPRMDZxO1-YrxbQIw5jQeZ6dz4A";

void sendTextToOpenAI(String inputText);
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
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
    http.begin("https://api.openai.com/v1/chat/completions");  // Correct endpoint for chat-based interactions
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));

    String payload = "{"
                      "\"model\": \"gpt-4\","
                      "\"messages\": [{\"role\": \"user\", \"content\": \"" + inputText + "\"}],"
                      "\"max_tokens\": 50"
                    "}";

    int httpResponseCode = http.POST(payload);
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

// #include <ArduinoBase64.h>


// #define I2S_WS 12
// #define I2S_SD 2
// #define I2S_SCK 13
// // #define I2S_WS 15
// // #define I2S_SD 13
// // #define I2S_SCK 2
// #define I2S_PORT I2S_NUM_0
// #define I2S_SAMPLE_RATE   (16000)
// #define I2S_SAMPLE_BITS   (16)
// #define I2S_READ_LEN      (16 * 1024)
// #define RECORD_TIME       (5) //Seconds
// #define I2S_CHANNEL_NUM   (1)
// #define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

// File file;
// const char filename[] = "/recording.wav";
// const int headerSize = 44;
// bool isWIFIConnected;

// void convertWavToBase64AndPrint(File file);

// void uploadFile();

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   SPIFFSInit();
//   i2sInit();
//   xTaskCreate(i2s_adc, "i2s_adc", 4096 * 2, NULL, 1, NULL);
//   delay(500);
//   xTaskCreate(wifiConnect, "wifi_Connect", 4096, NULL, 0, NULL);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// void SPIFFSInit(){
//   if(!SPIFFS.begin(true)){
//     Serial.println("SPIFFS initialisation failed!");
//     while(1) yield();
//   }

//   SPIFFS.remove(filename);
//   file = SPIFFS.open(filename, FILE_WRITE);
//   if(!file){
//     Serial.println("File is not available!");
//   }

//   byte header[headerSize];
//   wavHeader(header, FLASH_RECORD_SIZE);

//   file.write(header, headerSize);
//   listSPIFFS();
// }

// void i2sInit(){
//   i2s_config_t i2s_config = {
//     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//     .sample_rate = I2S_SAMPLE_RATE,
//     .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
//     .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//     .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
//     .intr_alloc_flags = 0,
//     .dma_buf_count = 64,
//     .dma_buf_len = 1024,
//     .use_apll = 1
//   };

//   i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

//   const i2s_pin_config_t pin_config = {
//     .bck_io_num = I2S_SCK,
//     .ws_io_num = I2S_WS,
//     .data_out_num = -1,
//     .data_in_num = I2S_SD
//   };

//   i2s_set_pin(I2S_PORT, &pin_config);
// }


// void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len)
// {
//     uint32_t j = 0;
//     uint32_t dac_value = 0;
//     for (int i = 0; i < len; i += 2) {
//         dac_value = ((((uint16_t) (s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
//         d_buff[j++] = 0;
//         d_buff[j++] = dac_value * 256 / 2048;
//     }
// }

// void i2s_adc(void *arg)
// {
    
//     int i2s_read_len = I2S_READ_LEN;
//     int flash_wr_size = 0;
//     size_t bytes_read;

//     char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
//     uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

//     i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
//     i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    
//     Serial.println(" *** Recording Start *** ");
//     while (flash_wr_size < FLASH_RECORD_SIZE) {
//         //read data from I2S bus, in this case, from ADC.
//         i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
//         //example_disp_buf((uint8_t*) i2s_read_buff, 64);
//         //save original data from I2S(ADC) into flash.
//         i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, i2s_read_len);
//         file.write((const byte*) flash_write_buff, i2s_read_len);
//         flash_wr_size += i2s_read_len;
//         ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
//         ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
//     }

//     Serial.println(" *** Recording End *** ");
    
//     // Convert the file from WAV to Base64 and print it to Serial
//     convertWavToBase64AndPrint(file);

//     file.close();

//     free(i2s_read_buff);
//     i2s_read_buff = NULL;
//     free(flash_write_buff);
//     flash_write_buff = NULL;
    
//     listSPIFFS();

//     if(isWIFIConnected){
//       // uploadFile();
//     }
    
//     vTaskDelete(NULL);
// }

// void convertWavToBase64AndPrint(File file) {
//     if (!file) {
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     size_t fileSize = file.size();
//     Serial.print("File size: ");
//     Serial.println(fileSize);

//     if (fileSize == 0) {
//         Serial.println("File is empty");
//         return;
//     }
// if (fileSize == 0) {
//     Serial.println("File is empty");
//     return;
// }

// const size_t bufferSize = 512; // Adjust buffer size as needed
// uint8_t buffer[bufferSize];
// size_t bytesRead;
// String base64String;

// file.seek(0); // Go to the beginning of the file

// Serial.println("Converting WAV file to Base64...");

// while ((bytesRead = file.read(buffer, bufferSize)) > 0) {
//     Serial.print("Bytes read: ");
//     Serial.println(bytesRead);

//     // Encode the buffer to Base64
//     char encodedChunk[Base64.encodedLength(bufferSize)];
//     Base64.encode(encodedChunk, (char*)buffer, bytesRead);

//     Serial.print("Encoded chunk: ");
//     Serial.println(encodedChunk);

//     base64String += encodedChunk;
// }

// Serial.println("Base64 Encoded WAV File:");
// Serial.println(base64String);
// }

// void example_disp_buf(uint8_t* buf, int length)
// {
//     printf("======\n");
//     for (int i = 0; i < length; i++) {
//         printf("%02x ", buf[i]);
//         if ((i + 1) % 8 == 0) {
//             printf("\n");
//         }
//     }
//     printf("======\n");
// }

// void wavHeader(byte* header, int wavSize){
//   header[0] = 'R';
//   header[1] = 'I';
//   header[2] = 'F';
//   header[3] = 'F';
//   unsigned int fileSize = wavSize + headerSize - 8;
//   header[4] = (byte)(fileSize & 0xFF);
//   header[5] = (byte)((fileSize >> 8) & 0xFF);
//   header[6] = (byte)((fileSize >> 16) & 0xFF);
//   header[7] = (byte)((fileSize >> 24) & 0xFF);
//   header[8] = 'W';
//   header[9] = 'A';
//   header[10] = 'V';
//   header[11] = 'E';
//   header[12] = 'f';
//   header[13] = 'm';
//   header[14] = 't';
//   header[15] = ' ';
//   header[16] = 0x10;
//   header[17] = 0x00;
//   header[18] = 0x00;
//   header[19] = 0x00;
//   header[20] = 0x01;
//   header[21] = 0x00;
//   header[22] = 0x01;
//   header[23] = 0x00;
//   header[24] = 0x80;
//   header[25] = 0x3E;
//   header[26] = 0x00;
//   header[27] = 0x00;
//   header[28] = 0x00;
//   header[29] = 0x7D;
//   header[30] = 0x01;
//   header[31] = 0x00;
//   header[32] = 0x02;
//   header[33] = 0x00;
//   header[34] = 0x10;
//   header[35] = 0x00;
//   header[36] = 'd';
//   header[37] = 'a';
//   header[38] = 't';
//   header[39] = 'a';
//   header[40] = (byte)(wavSize & 0xFF);
//   header[41] = (byte)((wavSize >> 8) & 0xFF);
//   header[42] = (byte)((wavSize >> 16) & 0xFF);
//   header[43] = (byte)((wavSize >> 24) & 0xFF);
  
// }


// void listSPIFFS(void) {
//   Serial.println(F("\r\nListing SPIFFS files:"));
//   static const char line[] PROGMEM =  "=================================================";

//   Serial.println(FPSTR(line));
//   Serial.println(F("  File name                              Size"));
//   Serial.println(FPSTR(line));

//   fs::File root = SPIFFS.open("/");
//   if (!root) {
//     Serial.println(F("Failed to open directory"));
//     return;
//   }
//   if (!root.isDirectory()) {
//     Serial.println(F("Not a directory"));
//     return;
//   }

//   fs::File file = root.openNextFile();
//   while (file) {

//     if (file.isDirectory()) {
//       Serial.print("DIR : ");
//       String fileName = file.name();
//       Serial.print(fileName);
//     } else {
//       String fileName = file.name();
//       Serial.print("  " + fileName);
//       // File path can be 31 characters maximum in SPIFFS
//       int spaces = 33 - fileName.length(); // Tabulate nicely
//       if (spaces < 1) spaces = 1;
//       while (spaces--) Serial.print(" ");
//       String fileSize = (String) file.size();
//       spaces = 10 - fileSize.length(); // Tabulate nicely
//       if (spaces < 1) spaces = 1;
//       while (spaces--) Serial.print(" ");
//       Serial.println(fileSize + " bytes");
//     }

//     file = root.openNextFile();
//   }

//   Serial.println(FPSTR(line));
//   Serial.println();
//   delay(1000);
// }

// void wifiConnect(void *pvParameters){
//   isWIFIConnected = false;
//   String ssid = "yag";
//   String password = "0545624950";

//   WiFi.begin(ssid, password);
//   while(WiFi.status() != WL_CONNECTED){
//     vTaskDelay(500);
//     Serial.print(".");
//   }
//   isWIFIConnected = true;
//   while(true){
//     vTaskDelay(1000);
//   }
// }

// // void uploadFile(){
//   // file = SPIFFS.open(filename, FILE_READ);
//   // if(!file){
//   //   Serial.println("FILE IS NOT AVAILABLE!");
//   //   return;
//   // }

//   // // Convert file to BASE64 and print to Serial
//   // size_t fileSize = file.size();
//   // uint8_t* fileData = (uint8_t*)malloc(fileSize);
//   // if (fileData == NULL) {
//   //   Serial.println("Failed to allocate memory for file data");
//   //   return;
//   // }
//   // if (file.read(fileData, fileSize) != fileSize) {
//   //   Serial.println("Failed to read file data");
//   //   free(fileData);
//   //   return;
//   // }
//   // String base64Data = base64::encode(fileData, fileSize);
//   // Serial.println(base64Data);
//   // free(fileData);
//   //   return;
//   // }
//   // String base64Data = base64::encode(fileData, fileSize);
//   // Serial.println(base64Data);
//   // free(fileData);

//   // Serial.println("===> Upload FILE to Node.js Server");

//   // HTTPClient client;
//   // client.begin("http://192.168.1.37:8888/uploadAudio");
//   // client.addHeader("Content-Type", "audio/wav");
//   // int httpResponseCode = client.sendRequest("POST", &file, file.size());
//   // Serial.print("httpResponseCode : ");
//   // Serial.println(httpResponseCode);

//   // if(httpResponseCode == 200){
//   //   String response = client.getString();
//   //   Serial.println("==================== Transcription ====================");
//   //   Serial.println(response);
//   //   Serial.println("====================      End      ====================");
//   // }else{
//   //   Serial.println("Error");
//   // }
//   // file.close();
//   // client.end();
// // }