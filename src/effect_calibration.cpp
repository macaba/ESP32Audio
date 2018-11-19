#include "effect_calibration.h"

void AudioEffectCalibration::update(void)
{
	audio_block_t *blocka;

	blocka = receiveWritable(0);
    if(blocka)
    {
        inputPeek = blocka->data[0];
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            blocka->data[i] *= m;
            blocka->data[i] += c;
            if(blocka->data[i] > 1.0)
                blocka->data[i] = 1.0;
            if(blocka->data[i] < -1.0)
                blocka->data[i] = -1.0;
        }
        outputPeek = blocka->data[0];
        transmit(blocka);
        release(blocka);
    }
}

void AudioEffectCalibration::calibrate(float zeroValue, float measuredZero, float fullScaleValue, float measuredFullScaleValue)
{
    //To do: use zeroValue      https://racelogic.support/02VBOX_Motorsport/Video_Data_Loggers/Video_VBOX_Range/Video_VBOX_-_User_manual/24_-_Calculating_Scale_and_Offset
    float scale = fullScaleValue/(measuredFullScaleValue-measuredZero);
    float offset = (scale * measuredZero) / fullScaleValue;
    offset *= -1;
    printf("M: %f, C: %f\n", scale, offset);
    m = scale;
    c = offset; 
}
