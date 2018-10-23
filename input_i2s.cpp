#include <Arduino.h>
#include "input_i2s.h"
#include "output_i2s.h"
#include "driver/i2s.h"

void AudioInputI2S::init()
{
	AudioOutputI2S::configI2S();
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6);		//Pin 34
	i2s_adc_enable(I2S_NUM_0);
	update_setup();			//This signals that the update method will block so we don't end up with an update loop out of control
}

void IRAM_ATTR AudioInputI2S::update(void)
{
	audio_block_t *new_out=NULL;

	//Serial.println("update");

	size_t bytesRead = 0;
	//i2s_adc_enable(I2S_NUM_0);
	i2s_read(I2S_NUM_0, (char*)&inputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)), &bytesRead, portMAX_DELAY);		//Block but yield to other tasks
    //i2s_adc_disable(I2S_NUM_0);

	new_out = allocate();
	if(new_out != NULL){
		for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			new_out->data[i] = ((float)(inputSampleBuffer[i] & 0xfff)/2048.0f) - 1.0f;
		}

		transmit(new_out);
		release(new_out);
	}

	//Serial.println(new_out->data[0]);
}