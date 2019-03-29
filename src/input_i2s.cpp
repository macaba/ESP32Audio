#include "input_i2s.h"
#include "Audio.h"
#include "driver/i2s.h"
#include "control_i2s.h"

void IRAM_ATTR AudioInputI2S::update(void)
{
	audio_block_t *new_left=NULL, *new_right=NULL;

	if(AudioControlI2S::configured)
	{
		new_left = allocate();
		if (new_left != NULL) {
			new_right = allocate();
			if (new_right == NULL) {
				release(new_left);
				new_left = NULL;
			}
		}

		//Serial.println("update");

		size_t bytesRead = 0;
		//i2s_adc_enable(I2S_NUM_0);
		switch(AudioControlI2S::bits)
		{
			case 16:
				i2s_read(I2S_NUM_0, (char*)&inputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)), &bytesRead, portMAX_DELAY);		//Block but yield to other tasks
				break;
			case 24:
			case 32:
				i2s_read(I2S_NUM_0, (char*)&inputSampleBuffer, (AUDIO_BLOCK_SAMPLES * sizeof(uint32_t)) * 2, &bytesRead, portMAX_DELAY);		//Block but yield to other tasks
				break;
			default:
				printf("Unknown bit depth\n");
				break;
		}
		//i2s_adc_disable(I2S_NUM_0);

		switch(AudioControlI2S::bits)
		{
			case 16:
				for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					if(new_left != NULL)
					{
						new_left->data[i] = ((float)(inputSampleBuffer[i] & 0xfff)/2048.0f) - 1.0f;
					}
				}
				break;
			case 24:
				union foo input;
				union foo output;
				for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					if(new_left != NULL)
					{
						input.integer = inputSampleBuffer[i*2];
						output.byte[0] = input.byte[1];
						output.byte[1] = input.byte[2];
						output.byte[2] = input.byte[3];
						if(CHECK_BIT(output.byte[2], 7))
						{
							output.byte[3] = 0xFF;
						}
						else
							output.byte[3] = 0x00;
						new_left->data[i] = ((float)output.integer)/8388608.0f;
					}
					if(new_right != NULL)
					{
						input.integer = inputSampleBuffer[i*2 + 1];
						output.byte[0] = input.byte[1];
						output.byte[1] = input.byte[2];
						output.byte[2] = input.byte[3];
						if(CHECK_BIT(output.byte[2], 7))
						{
							output.byte[3] = 0xFF;
						}
						else
							output.byte[3] = 0x00;
						new_right->data[i] = ((float)output.integer)/8388608.0f;
					}
					//printf("%f\n", new_out->data[i]);
				}
				//printf("0x%08x %d\n", output.integer, output.integer);
				//printf("%f\n", new_left->data[0]);
				break;
		}
		transmit(new_left, 0);
		release(new_left);
		transmit(new_right, 1);
		release(new_right);		
	}
}