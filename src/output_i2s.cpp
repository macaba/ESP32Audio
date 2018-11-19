#include "output_i2s.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "control_i2s.h"

void IRAM_ATTR AudioOutputI2S::update(void)
{
	audio_block_t *block_left, *block_right;

	if(AudioControlI2S::configured)
	{
		block_left = receiveReadOnly(0);  // input 0
		block_right = receiveReadOnly(1); // input 1

		switch(AudioControlI2S::bits)
		{
			case 16:
				for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					int16_t sample[2];
					if(block_left)
						sample[0] = (int16_t)(block_left->data[i] * 32767.0f);
					else
						sample[0] = 0;
					if(block_right)
						sample[1] = (int16_t)(block_right->data[i] * 32767.0f);
					else
						sample[1] = 0;

					outputSampleBuffer[i] = (((sample[1]+ 0x8000)<<16) | ((sample[0]+ 0x8000) & 0xffff));
				}
				break;
			case 24:
				for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					if(block_left)
						outputSampleBuffer[i*2 + 1] = (-((int32_t)(block_left->data[i] * 8388608.0f))) << 8;
					else
						outputSampleBuffer[i*2 + 1] = 0;
					
					if(block_right)
						outputSampleBuffer[i*2] = (-((int32_t)(block_right->data[i] * 8388608.0f))) << 8;
					else
						outputSampleBuffer[i*2] = 0;
				}
				break;
			case 32:
				for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					if(block_left)
						outputSampleBuffer[i*2 + 1] = ((int32_t)(block_left->data[i] * 1073741823.0f));
					else
						outputSampleBuffer[i*2 + 1] = 0;
					
					if(block_right)
						outputSampleBuffer[i*2] = ((int32_t)(block_right->data[i] * 1073741823.0f));
					else
						outputSampleBuffer[i*2] = 0;
				}
				break;
			default:
				printf("Unknown bit depth\n");
				break;
		}

		size_t totalBytesWritten = 0;
		size_t bytesWritten = 0;
		for(;;)
		{ 
			switch(AudioControlI2S::bits)
			{
				case 16:
					i2s_write(I2S_NUM_0, (const char*)&outputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)), &bytesWritten, portMAX_DELAY);		//Block but yield to other tasks
					break;
				case 24:
				case 32:
					i2s_write(I2S_NUM_0, (const char*)&outputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)) * 2, &bytesWritten, portMAX_DELAY);		//Block but yield to other tasks				
					break;
				default:
					printf("Unknown bit depth\n");
					break;
			}
			totalBytesWritten += bytesWritten;
			if(totalBytesWritten >= (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)))
				break;
			vPortYield();
		}

		if (block_left) release(block_left);
		if (block_right) release(block_right);

		//if (update_responsibility) AudioStream::update_all();
		//AudioStream::update_all();
	}
}
