#ifndef effect_calibration_h_
#define effect_calibration_h_
#include "Audio.h"
#include "AudioStream.h"

class AudioEffectCalibration : public AudioStream
{
public:
	AudioEffectCalibration() : AudioStream(1, inputQueueArray, "AudioEffectCalibration"), m(1), c(0) { initialised = true; }

    void calibrate(float zeroValue, float measuredZero, float fullScaleValue, float measuredFullScaleValue);

	void enableAverage() { inputAverage = 0; outputAverage = 0; averagingEnable = true; }
	void disableAverage() { averagingEnable = false; inputAverage = 0; outputAverage = 0; }
	float getInputAverage() { return inputAverage; }
	float getOutputAverage() { return outputAverage; }

	void inputDC(float value) { inputOverrideValue = value; inputOverrideEnable = true; }
	void inputNormal() { inputOverrideEnable = false; }

	virtual void update(void);
private:
	bool averagingEnable = false;	
	float inputAverage, outputAverage;
	bool inputOverrideEnable = false;
	float inputOverrideValue;
	audio_block_t *inputQueueArray[1];
    float m, c;
};

#endif