#ifndef effect_compressor_h_
#define effect_compressor_h_

#include "AudioStream.h"
#include "..\lib\sndfilter\compressor.h"

class AudioEffectCompressor : public AudioStream
{
public:
	AudioEffectCompressor() : AudioStream(1, inputQueueArray, "AudioEffectCompressor") { defaultSetup(); initialised = true; }
    void setupSimple(	
        float pregain,   // dB, amount to boost the signal before applying compression [0 to 100]
        float threshold, // dB, level where compression kicks in [-100 to 0]
        float knee,      // dB, width of the knee [0 to 40]
        float ratio,     // unitless, amount to inversely scale the output when applying comp [1 to 20]
        float attack,    // seconds, length of the attack phase [0 to 1]
        float release    // seconds, length of the release phase [0 to 1]
    );
    void setupAdvanced(
        // these parameters are the same as the simple version above:
        float pregain, float threshold, float knee, float ratio, float attack, float release,
        // these are the advanced parameters:
        float predelay,     // seconds, length of the predelay buffer [0 to 1]
        float releasezone1, // release zones should be increasing between 0 and 1, and are a fraction
        float releasezone2, //  of the release time depending on the input dB -- these parameters define
        float releasezone3, //  the adaptive release curve, which is discussed in further detail in the
        float releasezone4, //  demo: adaptive-release-curve.html
        float postgain,     // dB, amount of gain to apply after compression [0 to 100]
        float wet           // amount to apply the effect [0 completely dry to 1 completely wet]
    );
	virtual void update(void);
private:
    void defaultSetup();
	audio_block_t *inputQueueArray[1];
    sf_compressor_state_st compState;
};

#endif