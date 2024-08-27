#include <Arduino.h>
#include "I2SSampler.h"
#include <driver/i2s.h>
#include <algorithm>
#include "RingBuffer.h"
#include "esp_system.h"

void I2SSampler::addSample(int16_t sample)
{
    // store the sample
    m_write_ring_buffer_accessor->setCurrentSample(sample);
    if (m_write_ring_buffer_accessor->moveToNextSample() && this->stop_task == false)
    {
        // trigger the processor task as we've filled a buffer
        xTaskNotify(m_processor_task_handle, 1, eSetBits);
    }
    else if (this->stop_task == true)
        xTaskNotify(m_processor_task_handle, 1, eSetBits);

    
}

void i2sReaderTask(void *param)
{
    I2SSampler *sampler = (I2SSampler *)param;
    while (!sampler->stop_task)
    {
        // wait for some data to arrive on the queue
        i2s_event_t evt;
        if (xQueueReceive(sampler->m_i2s_queue, &evt, portMAX_DELAY) == pdPASS)
        {
            if (evt.type == I2S_EVENT_RX_DONE)
            {
                size_t bytesRead = 0;
                do
                {
                    // read data from the I2S peripheral
                    uint8_t i2sData[1024];
                    // read from i2s
-                    i2s_read(I2S_NUM_0, i2sData, 1024, &bytesRead, 10);                    
                    // process the raw data
                    sampler->processI2SData(i2sData, bytesRead);
                } while (bytesRead > 0 && sampler->stop_task==false);
            }
        }
    }
    vTaskDelay(100);
    i2s_driver_uninstall(I2S_NUM_0);
    vTaskDelete(NULL);

}

I2SSampler::I2SSampler(i2s_config_t config)
{
    this-> config = config;
    this->stop_task = false;
    // allocate the audio buffers
    for (int i = 0; i < AUDIO_BUFFER_COUNT; i++)
    {
        m_audio_buffers[i] = new AudioBuffer();
    }
    m_write_ring_buffer_accessor = new RingBufferAccessor(m_audio_buffers, AUDIO_BUFFER_COUNT);
}

void I2SSampler::start(i2s_port_t i2s_port, TaskHandle_t processor_task_handle)
{
    Serial.println("Starting i2s");
    m_i2s_port = i2s_port;
    m_processor_task_handle = processor_task_handle;
    //install and start i2s driver
    i2s_driver_install(I2S_NUM_0, &this->config, 4, &m_i2s_queue);
    // set up the I2S configuration from the subclass
    configureI2S();
    // set up clock, for matching the timing between the data an the mic
    // i2s_set_clk(I2S_NUM_0, i2s_config.sample_rate, i2s_config.bits_per_sample, I2S_CHANNEL_STEREO);

    // start a task to read samples
    xTaskCreate(i2sReaderTask, "i2s Reader Task", 4096, this, 1, &m_reader_task_handle);
}

RingBufferAccessor *I2SSampler::getRingBufferReader()
{
    RingBufferAccessor *reader = new RingBufferAccessor(m_audio_buffers, AUDIO_BUFFER_COUNT);
    // place the reaader at the same position as the writer - clients can move it around as required
    reader->setIndex(m_write_ring_buffer_accessor->getIndex());
    return reader;
}

int I2SSampler::Read(uint8_t* data, int numData) {
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


void I2SSampler::stop() {
    stop_task = true;

    // Wait until the task has stopped (optional but recommended)
    while (eTaskGetState(m_reader_task_handle) != eDeleted) {
        delay(40); // Small delay to let the task exit gracefully
    }
    i2s_driver_install(I2S_NUM_0, &this->config, 4, NULL);
    Serial.println("I2S Reader Task stopped.");
}


int I2SSampler::GetBitPerSample() {
  return (int)i2s_bits_per_sample_t(16);
}
