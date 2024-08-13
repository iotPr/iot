#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include "I2SMicSampler.h"
#include "ADCSampler.h"
#include "I2SOutput.h"
#include "config.h"
#include "Application.h"
#include "SPIFFS.h"

// #include "IntentProcessor.h"
// #include "Speaker.h"
// #include "IndicatorLight.h"
#define I2S_WS 25     // aka LRCL
#define I2S_SD 32     // aka DOUT
#define I2S_SCK 33        // aka BCLK


// i2s config for reading from both channels of I2S
i2s_config_t i2sMemsConfigBothChannels = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
    };

// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD};

// // i2s speaker pins
// i2s_pin_config_t i2s_speaker_pins = {
//     .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
//     .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
//     .data_out_num = I2S_SPEAKER_SERIAL_DATA,
//     .data_in_num = I2S_PIN_NO_CHANGE};

// This task does all the heavy lifting for our application
void applicationTask(void *param)
{
  Application *application = static_cast<Application *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  while (true)
  {
    // wait for some audio samples to arrive
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      application->run();
    }
  }
}

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting up");
  Serial.println("Connecting to WIFI ...");


  // start up wifi
  // launch WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin("Home04", "13243546");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

  // startup SPIFFS for the wav files
  // SPIFFS.begin();
  // make sure we don't get killed for our long running tasks
  esp_task_wdt_init(10, false);

  // start up the I2S input (from either an I2S microphone or Analogue microphone via the ADC)
  I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins, false);

  // start the i2s speaker output
  // I2SOutput *i2s_output = new I2SOutput();
  // i2s_output->start(I2S_NUM_1, i2s_speaker_pins);
  // Speaker *speaker = new Speaker(i2s_output);

  // indicator light to show when we are listening
  // IndicatorLight *indicator_light = new IndicatorLight();

  // and the intent processor
  // IntentProcessor *intent_processor = new IntentProcessor(speaker);
  // intent_processor->addDevice("kitchen", GPIO_NUM_5);
  // intent_processor->addDevice("bedroom", GPIO_NUM_21);
  // intent_processor->addDevice("table", GPIO_NUM_23);

  // create our application
  Application *application = new Application(i2s_sampler);

  // set up the i2s sample writer task
  TaskHandle_t applicationTaskHandle; 
  xTaskCreate(applicationTask, "Application Task", 8192, application, 1, &applicationTaskHandle);

  // start sampling from i2s device - use I2S_NUM_0 as that's the one that supports the internal ADC
  i2s_sampler->start(I2S_NUM_0, i2sMemsConfigBothChannels, applicationTaskHandle);
}

void loop()
{
  vTaskDelay(1000);
}
