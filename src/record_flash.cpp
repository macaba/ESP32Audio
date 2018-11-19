//#include "Audio.h"
#include "record_flash.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_partition.h"

#define PARTITION_NAME   "storage"

//flash record size, for recording 5 seconds' data
#define I2S_CHANNEL_NUM 1
#define FLASH_RECORD_SIZE         (I2S_CHANNEL_NUM * AUDIO_SAMPLE_RATE_EXACT * 32 / 8 * 1)
#define FLASH_SECTOR_SIZE         (0x1000)
#define FLASH_ERASE_SIZE          (FLASH_RECORD_SIZE % FLASH_SECTOR_SIZE == 0) ? FLASH_RECORD_SIZE : FLASH_RECORD_SIZE + (FLASH_SECTOR_SIZE - FLASH_RECORD_SIZE % FLASH_SECTOR_SIZE)
#define MAX_RECORD                4194304
#define AUDIO_BYTES               AUDIO_BLOCK_SAMPLES * sizeof(float)

static const char *TAG = "record_flash";

void AudioRecordFlash::init()
{
    printf("Erasing flash \n");
    data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, PARTITION_NAME);
    if (data_partition != NULL)
    {
        printf("partiton addr: 0x%08x; size: %d; label: %s\n", data_partition->address, data_partition->size, data_partition->label);
    }
    else
    {
        ESP_LOGE(TAG, "Partition error: can't find partition name: %s\n", PARTITION_NAME);
        return;
    }
    printf("Erase size: %d Bytes\n", FLASH_ERASE_SIZE);
    //ESP_ERROR_CHECK(esp_partition_erase_range(data_partition, 0, FLASH_ERASE_SIZE));
}

void IRAM_ATTR AudioRecordFlash::update(void)
{
	audio_block_t *block;

    if(!initialised)
    {
        init();
        initialised = true;
    }

    if(record)
    {
        block = receiveReadOnly();
	    if (!block) return;

        if (data_partition != NULL)
        {
            printf("Writing to partition\n");
            esp_partition_write(data_partition, recordPointer, (char *)block->data, AUDIO_BYTES);
            printf("Finished writing to partition\n");
            recordPointer += AUDIO_BYTES;
            if(recordPointer >= MAX_RECORD)
            {
                record = false;
            }
        }

        release(block);
    }

    if(playing)
    {
        block = allocate();
        if(!block) return;

        if (data_partition != NULL)
        {
            printf("Reading from partition\n");
            esp_partition_read(data_partition, playPointer, (char *)block->data, AUDIO_BYTES);
            playPointer += AUDIO_BYTES;
            if(playPointer >= recordPointer)
            {
                playing = false;
                printf("Ran out of things to play.\n");
            }
        }

        transmit(block);
        release(block);
    }
}

