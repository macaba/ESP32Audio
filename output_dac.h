#ifndef output_esp_h_
#define output_esp_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioOutputAnalog : public AudioStream
{
public:
	AudioOutputAnalog(void) : AudioStream(2, inputQueueArray, "AudioOutputAnalog") { begin(); blocking = true; }
	virtual ~AudioOutputAnalog();
	virtual void update(void);
	void begin(void);
private:
	audio_block_t *inputQueueArray[2];
	uint32_t outputSampleBuffer[AUDIO_BLOCK_SAMPLES];
};

#endif