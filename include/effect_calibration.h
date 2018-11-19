#ifndef effect_calibration_h_
#define effect_calibration_h_
#include "Audio.h"
#include "AudioStream.h"

class AudioEffectCalibration : public AudioStream
{
public:
	AudioEffectCalibration() : AudioStream(1, inputQueueArray, "AudioEffectCalibration"), m(1), c(0) { initialised = true; }
	virtual void update(void);
    void calibrate(float zeroValue, float measuredZero, float fullScaleValue, float measuredFullScaleValue);
	float inputPeek, outputPeek;
private:
	audio_block_t *inputQueueArray[1];
    float m, c;
};

#endif