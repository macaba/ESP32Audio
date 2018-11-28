#ifndef play_sdmmc_wav_h_
#define play_sdmmc_wav_h_

#include "AudioStream.h"
#include "../lib/dr_wav/dr_wav.h"

class AudioPlaySdMmcWav : public AudioStream
{
public:
	AudioPlaySdMmcWav(void) : AudioStream(0, NULL, "AudioPlaySdMmcWav") { started = false; }
	//bool play(const char *filename);
	//void stop(void);
	//bool isPlaying(void);
	//uint32_t positionMillis(void);
	//uint32_t lengthMillis(void);
	void loadFile(const char *filename);
	virtual void update(void);
private:
	void begin(void);
	bool started;
	drwav* pWav;
	//File wavfile;
	//FILE* f
	//bool consume(uint32_t size);
	//bool parse_format(void);
	//uint32_t header[10];		// temporary storage of wav header data
	//uint32_t data_length;		// number of bytes remaining in current section
	//uint32_t total_length;		// number of audio data bytes in file
	//uint32_t bytes2millis;
	//audio_block_t *block_left;
	//audio_block_t *block_right;
	//uint16_t block_offset;		// how much data is in block_left & block_right
	//uint8_t buffer[512];		// buffer one block of data
	//uint16_t buffer_offset;		// where we're at consuming "buffer"
	//uint16_t buffer_length;		// how much data is in "buffer" (512 until last read)
	//uint8_t header_offset;		// number of bytes in header[]
	//uint8_t state;
	//uint8_t state_play;
	//uint8_t leftover_bytes;
};

#endif
