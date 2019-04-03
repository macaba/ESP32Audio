#ifndef control_i2s_h_
#define control_i2s_h_

#include "AudioStream.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "output_i2s.h"
#include "input_i2s.h"
#include "esp_log.h"

#define MCLK (AUDIO_SAMPLE_RATE_EXACT * 384)

class AudioControlI2S
{
public:
	AudioControlI2S(void){}
	virtual ~AudioControlI2S();
	void init(i2s_port_t i2s_port, i2s_config_t* i2s_config, i2s_pin_config_t* i2s_pin_config, bool outputMCLK);
    void init_default_codec_rx_tx_24bit();
    void init_default_adc_dac();
	friend class AudioInputI2S;
    friend class AudioOutputI2S;
protected:
	static bool configured;
	static uint8_t bits;
};

#endif