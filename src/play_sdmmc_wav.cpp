#include "play_sdmmc_wav.h"
#define DR_WAV_IMPLEMENTATION
#include "../lib/dr_wav/dr_wav.h"

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <dirent.h>


// U3 card, 40Mhz, 256 samples (1024 bytes) i16 - 12.3% / 15% / 12.8%
// U3 card, 40Mhz, 512 samples (2048 bytes) i16 - 0.6% / 82% / 13.6%
// U3 card, 40Mhz, 1024 samples (4096 bytes) i16 - 0.6% / 108% / 26.50%
// U3 card, 40Mhz, 2048 samples (8192 bytes) i16 - 0.6% / 152% / 19.5%

// U3 card, 20Mhz, 256 samples (1024 bytes) i16 - 13.5% / 15.4% / 13.6%
// U3 card, 20Mhz, 512 samples (2048 bytes) i16 - 0.6% / 82% / 14.0%        (28.65%, not 82%, near beginning? Some sort of caching?)
// U3 card, 20Mhz, 1024 samples (4096 bytes) i16 - 0.6% / 109% / 27.9%
// U3 card, 20Mhz, 2048 samples (8192 bytes) i16 - 0.6% / 156% / 20.2%

// Cheap card, 20Mhz, 256 samples (1024 bytes) i16 - 17.7% / 20.65% / 17.95%
// Cheap card, 20Mhz, 512 samples (2048 bytes) i16 - 0.6% / 37.04% / 17.61%
// Cheap card, 20Mhz, 1024 samples (4096 bytes) i16 - 0.6% / 69.93% / 17.45%
// Cheap card, 20Mhz, 2048 samples (8192 bytes) i16 - 0.6% / 136.51% / 17.36%

static const char *TAG = "AudioPlaySdMmcWav";
//#define SampleBufferSize (2 * AUDIO_BLOCK_SAMPLES)       //Minimum of AUDIO_BLOCK_SAMPLES * 2. Should be multiples of AUDIO_BLOCK_SAMPLES * 2.
bool AudioPlaySdMmcWav::configured = false;

void AudioPlaySdMmcWav::configure(void)
{
	if(configured)
		return;

	ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    //host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode((gpio_num_t)15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode((gpio_num_t)2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode((gpio_num_t)4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode((gpio_num_t)12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode((gpio_num_t)13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

	// Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

	// Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
    // Please check its source code and implement error recovery when developing
    // production applications.
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    ESP_LOGI(TAG, "SD card mounted.");

    DIR* dir;				// pointer to the scanned directory.
    struct dirent* entry;	// pointer to one directory entry.

    dir = opendir("/sdcard");
    if (!dir)
    {
        ESP_LOGI(TAG, "Failed to open dir");
    }
    else
    {
        // scan the directory
        ESP_LOGI(TAG, "Listing files");
        while ((entry = readdir(dir)))
        {
            ESP_LOGI(TAG, "[file] %s", entry->d_name);
        }       
        closedir(dir);
    }

	configured = true;
}

void AudioPlaySdMmcWav::playFile(const char *filename)
{
    playback = false;

	if(!configured){
		configure();       
	}
	
    if(fileLoaded)
    {
        ESP_LOGI(TAG, "Closing old file.");
        drwav_close(pWav);
        ESP_LOGI(TAG, "Freeing old buffer.");
        free(pSampleData);
        fileLoaded = false;
    }

    pWav = drwav_open_file(filename);
    if (pWav == NULL) 
    {
        ESP_LOGE(TAG, "Failed to open file for reading.");
        return;
    }
    fileLoaded = true;

    if(pWav->channels > 2)
    {
        ESP_LOGE(TAG, "Wav file contains more than 2 channels.");
        return;
    }

    ESP_LOGI(TAG, "Wav file has %i channels.", pWav->channels);

    sampleBufferSize = pWav->channels * AUDIO_BLOCK_SAMPLES;
    ESP_LOGI(TAG, "Sample buffer size: %i, allocating %i bytes.", sampleBufferSize, sampleBufferSize * sizeof(float));
    pSampleData = (float*)heap_caps_malloc(sampleBufferSize * sizeof(float), MALLOC_CAP_DMA);

    drwav_read_f32(pWav, sampleBufferSize, pSampleData);     //Read 256 samples

    samplePointer = 0;
    fileBlockPointer = 0;
    fileBlockPointerMax = pWav->totalSampleCount / sampleBufferSize;
    ESP_LOGI(TAG, "File block max: %i", fileBlockPointerMax);
    fileLoaded = true;
    playback = true;
}

 void IRAM_ATTR AudioPlaySdMmcWav::update(void)
 {
    audio_block_t *new_left=NULL, *new_right=NULL;

    new_left = allocate();
    if (new_left != NULL) {
        new_right = allocate();
        if (new_right == NULL) {
            release(new_left);
            new_left = NULL;
        }
    }

    if(configured && fileLoaded && playback)
    { 
        switch(pWav->channels)
        {
            case 1:
                for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
                {
                    new_left->data[i] = pSampleData[samplePointer++];
                    new_right->data[i] = new_left->data[i];     //Mono
                }
                break;
            case 2:
                for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
                {
                    new_left->data[i] = pSampleData[samplePointer++];
                    new_right->data[i] = pSampleData[samplePointer++];
                }   
                break;           
        }
        if(samplePointer == sampleBufferSize)
        {
            samplePointer = 0;
            fileBlockPointer++;
            if(fileBlockPointer < fileBlockPointerMax)
            {
                drwav_read_f32(pWav, sampleBufferSize, pSampleData);
            }
            else
            {
                fileLoaded = false;
            }
        }     
    }

    transmit(new_left, 0);
    release(new_left);
    transmit(new_right, 1);
    release(new_right);	
 }