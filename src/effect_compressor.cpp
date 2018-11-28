#include "effect_compressor.h"
#include "AudioStream.h"
#include "../lib/sndfilter/compressor.h"

void AudioEffectCompressor::defaultSetup(){
    sf_defaultcomp(&compState, AUDIO_SAMPLE_RATE_EXACT);    
}

void AudioEffectCompressor::setupSimple(float pregain, float threshold, float knee, float ratio, float attack, float release)
{
    sf_simplecomp(&compState, AUDIO_SAMPLE_RATE_EXACT, pregain, threshold, knee, ratio, attack, release);
}

void AudioEffectCompressor::setupAdvanced(float pregain, float threshold, float knee, float ratio, float attack, float release, 
    float predelay, float releasezone1, float releasezone2, float releasezone3, float releasezone4, float postgain, float wet)
{
    sf_advancecomp(&compState, AUDIO_SAMPLE_RATE_EXACT, pregain, threshold, knee, ratio, attack, release,
	predelay, releasezone1, releasezone2, releasezone3, releasezone4, postgain,	wet);
}

void AudioEffectCompressor::update(void)
{
	audio_block_t *blocka;

	blocka = receiveWritable(0);
	if (!blocka) {
		return;
	}

    //sf_compressor_process(&compState, AUDIO_BLOCK_SAMPLES, input, output);
	transmit(blocka);
	release(blocka);
}
