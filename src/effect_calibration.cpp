#include "effect_calibration.h"
#include "esp_log.h"

static const char *TAG = "AudioEffectCalibration";

void AudioEffectCalibration::update(void)
{
	audio_block_t *blocka;

	blocka = receiveWritable(0);
    if(blocka)
    {
        if(averagingEnable)
        {
            float sum = 0;
            for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
                sum += blocka->data[i];
            }
            inputAverage = sum/AUDIO_BLOCK_SAMPLES;
        }
        float temp;
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            if(inputOverrideEnable)
            {
                temp = inputOverrideValue;
            }
            else
            {
                temp = blocka->data[i]; 
            }
            temp *= m;
            temp += c;
            blocka->data[i] = temp;
        }
        if(averagingEnable)
        {
            float sum = 0;
            for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
                sum += blocka->data[i];
            }
            outputAverage = sum/AUDIO_BLOCK_SAMPLES;
        }
        transmit(blocka);
        release(blocka);
    }
}

void AudioEffectCalibration::calibrate(float zeroValue, float measuredZero, float fullScaleValue, float measuredFullScaleValue)
{
    //To do: use zeroValue      https://racelogic.support/02VBOX_Motorsport/Video_Data_Loggers/Video_VBOX_Range/Video_VBOX_-_User_manual/24_-_Calculating_Scale_and_Offset
    //float scale = fullScaleValue/(measuredFullScaleValue-measuredZero);
    //float offset = (scale * measuredZero) / fullScaleValue;
    float scale = (fullScaleValue-zeroValue)/(measuredFullScaleValue-measuredZero);
    float offset = zeroValue-(measuredZero*scale);
    //offset *= -1;
    ESP_LOGI(TAG, "M: %f, C: %f.", scale, offset);
    m = scale;
    c = offset; 
}
