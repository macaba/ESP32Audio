#ifndef play_sdmmc_wav_h_
#define play_sdmmc_wav_h_

#include "AudioStream.h"
#include "../lib/dr_wav/dr_wav.h"

class AudioPlaySdMmcWav : public AudioStream
{
public:
	AudioPlaySdMmcWav(void) : AudioStream(0, NULL, "AudioPlaySdMmcWav") { initialised = true; }
	//bool play(const char *filename);
	//void stop(void);
	//bool isPlaying(void);
	//uint32_t positionMillis(void);
	//uint32_t lengthMillis(void);
	void playFile(const char *filename);
	virtual void update(void);
private:
	void configure(void);
	drwav* pWav;
	float* pSampleData = NULL;
	bool fileLoaded;
	bool playback;
	int samplePointer;			//Current sample pointer
	int sampleBufferSize;		//Maximum buffer block size
	int fileBlockPointer;		//Current buffer block
	int fileBlockPointerMax;	//Maximum buffer block
	static bool configured;		//This is used to stop multiple instances instantiating the SD card driver
};

#endif
