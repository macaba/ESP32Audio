#ifndef record_flash_h_
#define record_flash_h_

#include "Audio.h"
#include "AudioStream.h"
#include "esp_partition.h"

class AudioRecordFlash : public AudioStream
{
public:
	AudioRecordFlash(void) : AudioStream(1, inputQueueArray, "AudioRecordFlash") {  }
	virtual void update(void);
    void startRecord() { playing = false; recordPointer = 0; record = true; printf("Start Record\n"); }
    void stopRecord() { record = false; printf("Stop Record\n"); }
    void play() { record = false; playPointer = 0; playing = true; printf("Play\n");}
    void stop() { playing = false; }
private:
    void init();
	audio_block_t *inputQueueArray[1];
    bool initialised = false;
    const esp_partition_t *data_partition = NULL;
    bool record = false;
    bool playing = false;
    int recordPointer = 0;
    int playPointer = 0;
};

#endif