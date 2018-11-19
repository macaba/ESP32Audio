#ifndef output_i2s_h_
#define output_i2s_h_

#include "Audio.h"
#include "AudioStream.h"

class AudioOutputI2S : public AudioStream
{
public:
	AudioOutputI2S(void) : AudioStream(2, inputQueueArray, "AudioOutputI2S") { update_setup(); blocking = true; initialised = true; }		//update_setup(); let's the audiostream loop know that something will throttle the loop
	virtual void update(void);	
private:
	//void init(void);
	audio_block_t *inputQueueArray[2];
	int32_t outputSampleBuffer[AUDIO_BLOCK_SAMPLES * 2];
};

#endif