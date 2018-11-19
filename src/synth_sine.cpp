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

#include "synth_sine.h"

// data_waveforms.c
extern "C" {
  extern const float AudioWaveformSine[257];
}


void IRAM_ATTR AudioSynthWaveformSine::update(void)
{
	audio_block_t *block;
	uint32_t i, index;
	float scale, val1, val2;

	if (magnitude > 0) {
		block = allocate();
		if (block) {
			for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
				index = (uint32_t)phase_accumulator;
				val1 = AudioWaveformSine[index];
				val2 = AudioWaveformSine[index+1];
				scale = phase_accumulator - index;
				val1 *= (1.0f - scale);
				val2 *= scale;
				block->data[i] = (val1 + val2) * magnitude;
				phase_accumulator += phase_increment;
				if(phase_accumulator > 255.0)
					phase_accumulator -= 255.0;
			}
						
			AudioStream::transmit(block);
			AudioStream::release(block);
		}
	}
	else
	{
		phase_accumulator += phase_increment * AUDIO_BLOCK_SAMPLES;	
		if(phase_accumulator > 255.0)
			phase_accumulator -= 255.0;
	}
}

void AudioSynthWaveformSineModulated::update(void)
{
	audio_block_t *block, *modinput;
	uint32_t i, ph, inc, index;
	float scale, val1, val2;
	int16_t mod;

	modinput = receiveReadOnly();
	ph = phase_accumulator;
	inc = phase_increment;
	block = allocate();
	if (!block) {
		// unable to allocate memory, so we'll send nothing
		if (modinput) {
			// but if we got modulation data, update the phase
			for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
				mod = modinput->data[i];
				ph += inc + ((inc * (mod << 16)) << 1);
			}
			release(modinput);
		} else {
			ph += phase_increment * AUDIO_BLOCK_SAMPLES;
		}
		phase_accumulator = ph;
		return;
	}
	if (modinput) {
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
			index = ph >> 24;
			val1 = AudioWaveformSine[index];
			val2 = AudioWaveformSine[index+1];
			scale = (ph >> 8) & 0xFFFF;
			val2 *= (scale  / 65535.0);
			val1 *= ((0x10000 - scale)  / 65535.0);
			//block->data[i] = (((val1 + val2) >> 16) * magnitude) >> 16;
			block->data[i] = (val1 + val2) * magnitude;
			// -32768 = no phase increment
			// 32767 = double phase increment
			mod = modinput->data[i];
			ph += inc + ((inc * (mod << 16)) << 1);
			//ph += inc + (((int64_t)inc * (mod << 16)) >> 31);
		}
		release(modinput);
	} else {
		ph = phase_accumulator;
		inc = phase_increment;
		for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
			index = ph >> 24;
			val1 = AudioWaveformSine[index];
			val2 = AudioWaveformSine[index+1];
			scale = (ph >> 8) & 0xFFFF;
			val2 *= (scale  / 65535.0);
			val1 *= ((0x10000 - scale)  / 65535.0);
			block->data[i] = (val1 + val2) * magnitude;
			ph += inc;
		}
	}
	phase_accumulator = ph;
	transmit(block);
	release(block);
}