#ifndef play_sdmmc_wav_h_
#define play_sdmmc_wav_h_

#include "AudioStream.h"
#include "../lib/dr_wav/dr_wav.h"

class AudioPlaySdMmcWav : public AudioStream
{
public:
	AudioPlaySdMmcWav(void) : AudioStream(0, NULL, "AudioPlaySdMmcWav") { pSampleData = NULL; started = false; initialised = true; }
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
	float* pSampleData;
	bool sampleLoaded;
	int sampleBlockPointer;
	int sampleMaxBlock;
	int samplePartialEndCount;
};

#endif
