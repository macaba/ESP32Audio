#ifndef input_i2s_h_
#define input_i2s_h_

#include "Audio.h"
#include "AudioStream.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

class AudioInputI2S : public AudioStream
{
public:
    AudioInputI2S() : AudioStream(0, NULL, "AudioInputI2S") { blockingObjectRunning = true; blocking = true; initialised = true; }        //blockingObjectRunning - let's the audiostream loop know that something will throttle the loop
    virtual void update(void);
private:
    int32_t inputSampleBuffer[AUDIO_BLOCK_SAMPLES * 2];
    unsigned char reverse(unsigned char b) {
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
    }
    union foo
    {
        int32_t integer;
        uint8_t byte[4];
    };
};



#endif