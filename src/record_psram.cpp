#include "record_psram.h"
//#include "esp_spiram.h"

void AudioRecordPSRAM::init(int num)
{
    if(num > RECORD_PSRAM_BLOCK_MAX)
        num = RECORD_PSRAM_BLOCK_MAX;
    //buffer = (audio_block_t*)ps_calloc(num, sizeof(audio_block_t));
    buffer = (audio_block_t*)heap_caps_malloc(num * sizeof(audio_block_t), MALLOC_CAP_SPIRAM);
    printf("AudioRecordPSRAM: %i blocks allocated.\n", num);
}

void IRAM_ATTR AudioRecordPSRAM::update(void)
{
	audio_block_t *block;

    if(!initialised)
    {
        init(RECORD_PSRAM_BLOCK_MAX);
        initialised = true;
    }

    if(record)
    {
        block = receiveReadOnly();
	    if (!block) return;

        //printf("Storing audio in buffer %i\n", recordPointer);
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            buffer[recordPointer].data[i] = block->data[i];
        }
        recordPointer++;

        if(recordPointer >= RECORD_PSRAM_BLOCK_MAX)
        {
            record = false;     //Stop recording
            recordPointer = RECORD_PSRAM_BLOCK_MAX;
        }

        release(block);
    }

    if(playing)
    {
        block = allocate();
        if(!block) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            block->data[i] = buffer[playPointer].data[i];
        }
        playPointer++;

        if(playPointer >= recordPointer)
        {
            playing = false;        //Stop playing
        }

        transmit(block);
        release(block);
    }
}

