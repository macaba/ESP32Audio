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

#ifndef mixer_h_
#define mixer_h_

#include <math.h>
#include "AudioStream.h"

class AudioMixer4 : public AudioStream
{
public:
	AudioMixer4(void) : AudioStream(4, inputQueueArray, "AudioMixer4") {
		for (int i=0; i<4; i++) multiplier[i] = 1.0;
		initialised = true;
	}
	virtual void update(void);
	void gain(unsigned int channel, float gain) {
		if (channel >= 4) return;
		if (gain > 100000.0f) gain = 100000.0f;   //100000 = 100db
		else if (gain < -100000.0f) gain = -100000.0f;
		multiplier[channel] = gain; // TODO: proper roundoff?
	}
    void gainDb(unsigned int channel, float db, bool invert){
        if (channel >= 4) return;
        if (db > 100.0f) db = 100.0f;
		else if (db < -100.0f) db = -100.0f;
        if(invert)
            multiplier[channel] = powf(10.0f,db/20.0f) * -1.0;
        else
            multiplier[channel] = powf(10.0f,db/20.0f);   
    }
private:
	float multiplier[4];
	audio_block_t *inputQueueArray[4];
};

class AudioAmplifier : public AudioStream
{
public:
	AudioAmplifier(void) : AudioStream(1, inputQueueArray, "AudioAmplifier"), multiplier(1.0) {
	}
	virtual void update(void);
	void gain(float n) {
		if (n > 100000.0f) n = 100000.0f;       //100000 = 100db
		else if (n < -100000.0f) n = -100000.0f;
		multiplier = n;
	}
    void gainDb(float db){
        if (db > 100.0f) db = 100.0f;
		else if (db < -100.0f) db = -100.0f;
        multiplier = powf(10.0f,db/20.0f);
    }
private:
	float multiplier;
	audio_block_t *inputQueueArray[1];
};

#endif