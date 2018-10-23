#ifndef output_esp_h_
#define output_esp_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioOutputI2S : public AudioStream
{
public:
	AudioOutputI2S(void) : AudioStream(2, inputQueueArray, "AudioOutputI2S") { init(); blocking = true; initialised = true; }
	virtual ~AudioOutputI2S();
	virtual void update(void);	
	friend class AudioInputI2S;
protected:
	static void configI2S();			//Also called by AudioInputI2S
private:
	void init(void);
	audio_block_t *inputQueueArray[2];
	uint32_t outputSampleBuffer[AUDIO_BLOCK_SAMPLES];
	static bool configured;
};

#endif