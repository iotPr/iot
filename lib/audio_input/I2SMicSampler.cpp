#include "I2SMicSampler.h"
#include "driver/i2s.h"
#include "soc/i2s_reg.h"

I2SMicSampler::I2SMicSampler(i2s_pin_config_t &i2sPins, i2s_config_t config, bool fixSPH0645) : I2SSampler(config)
{
    m_i2sPins = i2sPins;
    m_fixSPH0645 = fixSPH0645;
}


void I2SMicSampler::configureI2S()
{
    i2s_set_pin(I2S_NUM_0, &m_i2sPins);
}

void I2SMicSampler::processI2SData(uint8_t *i2sData, size_t bytesRead)
{
    int32_t *samples = (int32_t *)i2sData;
    for (int i = 0; i < bytesRead / 4; i++)
    {
        addSample(samples[i] >> 11);
    }
}


