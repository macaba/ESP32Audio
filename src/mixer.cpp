/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Audio.h"
#include "mixer.h"

#define MULTI_UNITYGAIN 1.0

static void applyGain(float *data, float mult)
{
    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        data[i] *= mult;
    }
}

static void applyGainThenAdd(float *dst, const float *src, float mult)
{
	if (mult == MULTI_UNITYGAIN) {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            dst[i] += src[i];
        }
	} else {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            dst[i] += src[i] * mult;
        }
	}
}

void IRAM_ATTR AudioMixer4::update(void)
{
	audio_block_t *in, *out=NULL;
	unsigned int channel;

	for (channel=0; channel < 4; channel++) {
		if (!out) {
			out = receiveWritable(channel);
			if (out) {
				if (multiplier[channel] != MULTI_UNITYGAIN) applyGain(out->data, multiplier[channel]);
			}
		} else {
			in = receiveReadOnly(channel);
			if (in) {
				applyGainThenAdd(out->data, in->data, multiplier[channel]);
				release(in);
			}
		}
	}
	if (out) {
		transmit(out);
		release(out);
	}
}

void IRAM_ATTR AudioAmplifier::update(void)
{
	audio_block_t *block;
	float mult = multiplier;

	if (mult == 0) {
		// zero gain, discard any input and transmit nothing
		block = receiveReadOnly(0);
		if (block) release(block);
	} else if (mult == MULTI_UNITYGAIN) {
		// unity gain, pass input to output without any change
		block = receiveReadOnly(0);
		if (block) {
			transmit(block);
			release(block);
		}
	} else {
		// apply gain to signal
		block = receiveWritable(0);
		if (block) {
			applyGain(block->data, mult);
			transmit(block);
			release(block);
		}
	}
}