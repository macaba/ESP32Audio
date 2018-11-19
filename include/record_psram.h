#ifndef record_psram_h_
#define record_psram_h_

#include "Audio.h"
#include "AudioStream.h"

#define RECORD_PSRAM_BLOCK_MAX  (AUDIO_SAMPLE_RATE_EXACT / AUDIO_BLOCK_SAMPLES) * 10

class AudioRecordPSRAM : public AudioStream
{
public:
	AudioRecordPSRAM(void) : AudioStream(1, inputQueueArray, "AudioRecordPSRAM") { }
	virtual void update(void);
    void startRecord() { playing = false; recordPointer = 0; record = true; printf("Start Record\n"); }
    void stopRecord() { record = false; printf("Stop Record\n"); }
    void play() { record = false; playPointer = 0; playing = true; printf("Play\n");}
    void stop() { playing = false; }
private:
    void init(int num);
	audio_block_t *inputQueueArray[1];
    audio_block_t *buffer;
    bool record = false;
    bool playing = false;
    int recordPointer = 0;
    int playPointer = 0;
};

#endif