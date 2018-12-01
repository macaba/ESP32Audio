/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef AudioStream_h
#define AudioStream_h

#include <stdio.h>  // for NULL
#include <string.h> // for memcpy
#include <inttypes.h>

#define AUDIO_BLOCK_SAMPLES  128
#define AUDIO_SAMPLE_RATE_EXACT 44100
#define AUDIO_SAMPLE_RATE AUDIO_SAMPLE_RATE_EXACT

class AudioStream;
class AudioConnection;

typedef struct audio_block_struct {
	uint8_t  ref_count;
	uint8_t  reserved1;
	uint16_t memory_pool_index;
    float    data[AUDIO_BLOCK_SAMPLES];
	//int      blockLength = AUDIO_BLOCK_SAMPLES; // AUDIO_BLOCK_SAMPLES is 128, from AudioStream.h
    //float    sampleRate = AUDIO_SAMPLE_RATE; // AUDIO_SAMPLE_RATE is 44117.64706 from AudioStream.h
	//int 	 byteLength = AUDIO_BLOCK_SAMPLES * sizeof(float);
} audio_block_t;


class AudioConnection
{
public:
	AudioConnection(AudioStream &source, AudioStream &destination) :
		src(source), dst(destination), src_index(0), dest_index(0),
		next_dest(NULL)
		{ isConnected = false;
		  connect(); }
	AudioConnection(AudioStream &source, unsigned char sourceOutput,
		AudioStream &destination, unsigned char destinationInput) :
		src(source), dst(destination),
		src_index(sourceOutput), dest_index(destinationInput),
		next_dest(NULL)
		{ isConnected = false;
		  connect(); }
	friend class AudioStream;
	~AudioConnection() {
		disconnect();
	}
	void disconnect(void);
	void connect(void);
protected:
	AudioStream &src;
	AudioStream &dst;
	unsigned char src_index;
	unsigned char dest_index;
	AudioConnection *next_dest;
	bool isConnected;
};


#define AudioMemory(num) ({ \
	static audio_block_t data[num]; \
	AudioStream::initialize_memory(data, num); \
})

/* #define AudioMemory(num) ({ \
	static audio_block_t *data = (audio_block_t*)ps_malloc(num * sizeof(audio_block_t)); \
	AudioStream::initialize_memory(data, num); \
}) */

#define AudioMemoryUsage() (AudioStream::memory_used)
#define AudioMemoryUsageMax() (AudioStream::memory_used_max)
#define AudioMemoryUsageMaxReset() (AudioStream::memory_used_max = AudioStream::memory_used)

class AudioStream
{
public:
	AudioStream(unsigned char ninput, audio_block_t **iqueue, const char* defaultName) :
		name(defaultName), num_inputs(ninput), inputQueue(iqueue) {
			active = false;
			blocking = false;
			destination_list = NULL;
			for (int i=0; i < num_inputs; i++) {
				inputQueue[i] = NULL;
			}
			// add to a simple list, for update_all
			// TODO: replace with a proper data flow analysis in update_all
			if (first_update == NULL) {
				first_update = this;
			} else {
				AudioStream *p;
				for (p=first_update; p->next_update; p = p->next_update) ;
				p->next_update = this;
			}
			next_update = NULL;
			numConnections = 0;
		}
	static void initialize_memory(audio_block_t *data, unsigned int num);
	bool isActive(void) { return active; }
	uint32_t clocksPerUpdate;
	uint32_t clocksPerUpdateMax;
	uint32_t clocksPerUpdateMin;
	uint32_t clocksPerSecond;
	static uint16_t memory_used;
	static uint16_t memory_used_max;
	static void update_all(void);

	//The following 5 are public for CPU reporting
	const char* name;
	static AudioStream *first_update; // for update_all
	AudioStream *next_update; // for update_all
	bool active;			//If true; object has been connected to
	bool blocking;			//If true; Ignore this object when calculating CPU clocks
	bool initialised;		//If false: Ignore this object when calculating CPU clocks. Allows for lazy loaded classes that instantiate PSRAM or Flash.
	static bool blockingObjectRunning;
protected:
	unsigned char num_inputs;
	static audio_block_t * allocate(void);
	static void release(audio_block_t * block);
	void transmit(audio_block_t *block, unsigned char index = 0);
	audio_block_t * receiveReadOnly(unsigned int index = 0);
	audio_block_t * receiveWritable(unsigned int index = 0);
	//static void update_all(void) { software_isr(); }
	//friend void software_isr(void);
	friend class AudioConnection;
	uint8_t numConnections;	
private:
	AudioConnection *destination_list;
	audio_block_t **inputQueue;
	virtual void update(void) = 0;
	static audio_block_t *memory_pool;
	static uint32_t memory_pool_available_mask[];
	static uint16_t memory_pool_first_mask;	
	uint32_t clocksPerSecondSum;		
};

#endif