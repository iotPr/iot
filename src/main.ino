#include <driver/i2s.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Audio.h"
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26



#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (16 * 1024)
#define RECORD_TIME       (5) //Seconds
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

File file;
const char filename[] = "/recording.wav";
const int headerSize = 44;
bool isWIFIConnected;

void uploadFile();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFSInit();
  i2sInit();
  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
  delay(500);
  xTaskCreate(wifiConnect, "wifi_Connect", 4096, NULL, 0, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void SPIFFSInit(){
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS initialisation failed!");
    while(1) yield();
  }

  SPIFFS.remove(filename);
  file = SPIFFS.open(filename, FILE_WRITE);
  if(!file){
    Serial.println("File is not available!");
  }

  byte header[headerSize];
  wavHeader(header, FLASH_RECORD_SIZE);

  file.write(header, headerSize);
  listSPIFFS();
}

void i2sInit(){
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}


void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len)
{
    uint32_t j = 0;
    uint32_t dac_value = 0;
    for (int i = 0; i < len; i += 2) {
        dac_value = ((((uint16_t) (s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048;
    }
}

void i2s_adc(void *arg)
{
    
    int i2s_read_len = I2S_READ_LEN;
    int flash_wr_size = 0;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    
    Serial.println(" *** Recording Start *** ");
    while (flash_wr_size < FLASH_RECORD_SIZE) {
        //read data from I2S bus, in this case, from ADC.
        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
        //example_disp_buf((uint8_t*) i2s_read_buff, 64);
        //save original data from I2S(ADC) into flash.
        i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, i2s_read_len);
        file.write((const byte*) flash_write_buff, i2s_read_len);
        flash_wr_size += i2s_read_len;
        ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
        ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
    }
    file.close();

    free(i2s_read_buff);
    i2s_read_buff = NULL;
    free(flash_write_buff);
    flash_write_buff = NULL;
    
    listSPIFFS();

    if(isWIFIConnected){
        Serial.println("now upload from esp to google!");
        uploadFile();
    }
    
    vTaskDelete(NULL);
}

void example_disp_buf(uint8_t* buf, int length)
{
    printf("======\n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("======\n");
}

void wavHeader(byte* header, int wavSize){
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x01;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);
  
}


void listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

void wifiConnect(void *pvParameters){
  isWIFIConnected = false;
  String ssid = "yag";
  String password = "0545624950";

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    vTaskDelay(500);
    Serial.print(".");
  }
  Serial.println("Wifi is connect!");
  isWIFIConnected = true;
  while(true){
    vTaskDelay(1000);
  }
}



//   HTTPClient client;
//   client.begin("http://172.31.192.1:8888/uploadAudio");
//   client.addHeader("Content-Type", "audio/wav");
//   int httpResponseCode = client.sendRequest("POST", &file, file.size());
//   Serial.print("httpResponseCode : ");
//   Serial.println(httpResponseCode);

    #include <base64.h>
    #include <WiFiClientSecure.h>

    void uploadFile(){
        file = SPIFFS.open(filename, FILE_READ);
        if(!file){
            Serial.println("FILE IS NOT AVAILABLE!");
            return;
        }


        Serial.println("play the record");
        #include <driver/i2s.h>

        // Set I2S configuration for speaker
        i2s_config_t i2s_speaker_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = I2S_SAMPLE_RATE,
            .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BITS,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_I2S_MSB,
            .intr_alloc_flags = 0,
            .dma_buf_count = 8,
            .dma_buf_len = 64,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0
        };

        // Set I2S pin configuration for speaker
        i2s_pin_config_t i2s_speaker_pin_config = {
            .bck_io_num = I2S_BCLK,
            .ws_io_num = I2S_LRC,
            .data_out_num = I2S_DOUT,
            .data_in_num = I2S_PIN_NO_CHANGE
        };

        // Initialize I2S driver for speaker
        i2s_driver_install(I2S_PORT, &i2s_speaker_config, 0, NULL);
        i2s_set_pin(I2S_PORT, &i2s_speaker_pin_config);

        // Open the WAV file for reading
        File wavFile = SPIFFS.open(filename, FILE_READ);
        if (!wavFile) {
            Serial.println("Failed to open WAV file!");
            return;
        }

        // Read the WAV header
        byte wavHeader[44];
        wavFile.read(wavHeader, sizeof(wavHeader));

        // Declare the variable 'bytes_written'
        size_t bytes_written;
        
        // Write the WAV header to the I2S speaker
        i2s_write(I2S_PORT, wavHeader, sizeof(wavHeader), &bytes_written, portMAX_DELAY);

        // Read and write the WAV data to the I2S speaker
        byte buffer[1024];
        while (wavFile.available()) {
            int bytesRead = wavFile.read(buffer, sizeof(buffer));
            i2s_write(I2S_PORT, buffer, bytesRead, &bytes_written, portMAX_DELAY);
        }

        // Close the WAV file
        wavFile.close();

        // Stop the I2S driver for speaker
        i2s_driver_uninstall(I2S_PORT);
        



        // Serial.println("===> convert file to Base64");

        // #include <driver/i2s.h>

 
        // // Convert file to base64
        // String base64Data = "";
        // while (file.available()) {
        //     base64Data += (char)file.read();
        // }
        // String encodedData = base64::encode(base64Data);

        // // Read API key from file
        // Serial.println("Base64 converted succesfully!");
        
        // // create apiKeyFile that will get its content from "stt_yagel\NodejsServer\api_key.txt" (windows path)
        // // create apiKeyFile that will get its content from "stt_yagel\NodejsServer\api_key.txt" (windows path)
        // // File apiKeyFile = SPIFFS.open("/stt_yagel/NodejsServer/api_key.txt", FILE_READ);
        

        // // if (!apiKeyFile) {
        // //     Serial.println("API key file is not available!");
        // //     return;
        // // }
        // // String apiKey = apiKeyFile.readString();
        // String apiKey = "AIzaSyAIgezZitJr_mpSK-7bkC7GPstBSTiR9-M";
        // // apiKeyFile.close();

        // // // Create JSON payload
        // // DynamicJsonDocument payload(1024);
        // // payload["audio"] = encodedData;
        // // payload["api_key"] = apiKey;

        // // // Convert payload to string
        // // String payloadString;
        // // serializeJson(payload, payloadString);

        // // Create JSON payload
        // String payloadString = "{\"audio\":\"" + encodedData + "\",\"api_key\":\"" + apiKey + "\"}";

        // Serial.println(payloadString);

        // // Send HTTP POST request to Google Speech-to-Text API
        // WiFiClientSecure client;
        // if (!client.connect("speech.googleapis.com", 443)) {
        //     Serial.println("Failed to connect to Google Speech-to-Text API!");
        //     return;
        // }
        // client.println("POST /v1/speech:recognize HTTP/1.1");
        // client.println("Host: speech.googleapis.com");
        // client.println("Content-Type: application/json");
        // client.print("Content-Length: ");
        // client.println(payloadString.length());
        // client.println();
        // client.println(payloadString);

        // // Read response from Google Speech-to-Text API
        // String response = "";
        // while (client.connected()) {
        //     String line = client.readStringUntil('\n');
        //     if (line == "\r") {
        //     break;
        //     }
        // }
        // while (client.available()) {
        //     response += (char)client.read();
        // }
        // client.stop();

        // // Parse response and extract transcription
        // String transcription = response.substring(response.indexOf("\"transcript\":\"") + 14);
        // transcription = transcription.substring(0, transcription.indexOf("\""));

        // // Print transcription to Serial
        // Serial.println("Transcription: " + transcription);
    }

    
