#ifndef input_i2s_h_
#define input_i2s_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioInputI2S : public AudioStream
{
public:
    AudioInputI2S() : AudioStream(0, NULL, "AudioInputI2S") { init(); blocking = true; }
    virtual void update(void);
private:
    static void init();
    uint32_t inputSampleBuffer[AUDIO_BLOCK_SAMPLES];
};

#endif