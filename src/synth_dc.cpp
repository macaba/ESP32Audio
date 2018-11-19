#include "synth_dc.h"

void IRAM_ATTR AudioSynthWaveformDC::update(void)
{
	audio_block_t *block;

    block = allocate();
    if (block) {
        for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
            block->data[i] = dcValue;
        }
                    
        AudioStream::transmit(block);
        AudioStream::release(block);
    }
}