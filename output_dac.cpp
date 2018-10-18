#include <Arduino.h>
#include "output_dac.h"
#include "driver/i2s.h"

void AudioOutputAnalog::begin(void)
{
	i2s_mode_t mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);
	i2s_comm_format_t comm_fmt = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB;

	i2s_config_t i2s_config_dac = {
		.mode = mode,
		.sample_rate = AUDIO_SAMPLE_RATE_EXACT,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = comm_fmt,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // lowest interrupt priority
		.dma_buf_count = 2,
		.dma_buf_len = AUDIO_BLOCK_SAMPLES,
		.use_apll = 0
	};
	Serial.printf("+%d %p\n", 0, &i2s_config_dac);
	if (i2s_driver_install((i2s_port_t)0, &i2s_config_dac, 0, NULL) != ESP_OK) 
	{
		Serial.println("ERROR: Unable to install I2S driver\n");
	}
	i2s_set_pin((i2s_port_t)0, NULL);
	i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
	i2s_zero_dma_buffer((i2s_port_t)0);	
  	i2s_set_sample_rates((i2s_port_t)0, AUDIO_SAMPLE_RATE_EXACT); 

	update_setup();
}

AudioOutputAnalog::~AudioOutputAnalog(){
    Serial.printf("UNINSTALL I2S\n");
    i2s_driver_uninstall((i2s_port_t)0); //stop & destroy i2s driver
}

void IRAM_ATTR AudioOutputAnalog::update(void)
{
	audio_block_t *block_left, *block_right;

	block_left = receiveReadOnly(0);  // input 0
	block_right = receiveReadOnly(1); // input 1

	for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++){
		int16_t sample[2];
		if(block_left)
			sample[0] = (int16_t)(block_left->data[i] * 32767.0f);
		else
			sample[0] = 0;
		if(block_right)
			sample[1] = (int16_t)(block_right->data[i] * 32767.0f);
		else
			sample[1] = sample[0];		//Fall back to mono
		outputSampleBuffer[i] = (((sample[1]+ 0x8000)<<16) | ((sample[0]+ 0x8000) & 0xffff));
	}

	size_t totalBytesWritten = 0;
	size_t bytesWritten = 0;
	while(1)
	{ 
		i2s_write(I2S_NUM_0, (const char*)&outputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)), &bytesWritten, portMAX_DELAY);		//Block but yield to other tasks
		totalBytesWritten += bytesWritten;
		if(totalBytesWritten >= (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)))
			break;
		yield();
	}

	if (block_left) release(block_left);
	if (block_right) release(block_right);

	//if (update_responsibility) AudioStream::update_all();
	//AudioStream::update_all();
}


