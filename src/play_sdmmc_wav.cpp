#include "play_sdmmc_wav.h"
#define DR_WAV_IMPLEMENTATION
#include "../lib/dr_wav/dr_wav.h"

// #define STATE_DIRECT_8BIT_MONO		0  // playing mono at native sample rate
// #define STATE_DIRECT_8BIT_STEREO	1  // playing stereo at native sample rate
// #define STATE_DIRECT_16BIT_MONO		2  // playing mono at native sample rate
// #define STATE_DIRECT_16BIT_STEREO	3  // playing stereo at native sample rate
// #define STATE_CONVERT_8BIT_MONO		4  // playing mono, converting sample rate
// #define STATE_CONVERT_8BIT_STEREO	5  // playing stereo, converting sample rate
// #define STATE_CONVERT_16BIT_MONO	6  // playing mono, converting sample rate
// #define STATE_CONVERT_16BIT_STEREO	7  // playing stereo, converting sample rate
// #define STATE_PARSE1			8  // looking for 20 byte ID header
// #define STATE_PARSE2			9  // looking for 16 byte format header
// #define STATE_PARSE3			10 // looking for 8 byte data header
// #define STATE_PARSE4			11 // ignoring unknown chunk after "fmt "
// #define STATE_PARSE5			12 // ignoring unknown chunk before "fmt "
// #define STATE_STOP			13


#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <dirent.h>

static const char *TAG = "AudioPlaySdMmcWav";

void AudioPlaySdMmcWav::begin(void)
{
	if(started)
		return;

	ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

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
            ESP_LOGI(TAG, "file: %s", entry->d_name);
        }       
        closedir(dir);
    }

	started = true;
}

void AudioPlaySdMmcWav::loadFile(const char *filename){
	if(!started){
		begin();
	}
	
    sampleLoaded = false;

    pWav = drwav_open_file(filename);
    if (pWav == NULL) 
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    if(pWav->channels > 2)
    {
        ESP_LOGE(TAG, "Wav file contains more than 2 channels");
        return;
    }
    
    if(pSampleData != NULL)
    {           
        free(pSampleData);
    }

    ESP_LOGI(TAG, "Allocating %i bytes for sample data", (int)(pWav->totalSampleCount * pWav->channels * sizeof(float)));
    //pSampleData = (float*)malloc((size_t)pWav->totalSampleCount * pWav->channels * sizeof(float));
    pSampleData = (float*)heap_caps_malloc((size_t)pWav->totalSampleCount * pWav->channels * sizeof(float), MALLOC_CAP_SPIRAM);
    drwav_read_f32(pWav, pWav->totalSampleCount/4, pSampleData);

    drwav_close(pWav);

    sampleBlockPointer = 0;
    sampleMaxBlock = pWav->totalSampleCount / AUDIO_BLOCK_SAMPLES;
    samplePartialEndCount = pWav->totalSampleCount % AUDIO_BLOCK_SAMPLES;
    sampleLoaded = true;
}

 void AudioPlaySdMmcWav::update(void)
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

    if(started){
        if(sampleLoaded)
        {
            switch(pWav->channels){
                case 1:
                    if(sampleBlockPointer < sampleMaxBlock){
                        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
                        {
                            new_left->data[i] = pSampleData[(sampleBlockPointer * AUDIO_BLOCK_SAMPLES) + i];
                            new_right->data[i] = new_left->data[i];     //Mono
                        }
                        sampleBlockPointer++;
                    }
                    else if (sampleBlockPointer == sampleMaxBlock)
                    {
                        for(int i = 0; i < samplePartialEndCount; i++)
                        {
                            new_left->data[i] = pSampleData[(sampleBlockPointer * AUDIO_BLOCK_SAMPLES) + i];
                            new_right->data[i] = new_left->data[i];     //Mono
                        }
                        for(int i = 0; i < AUDIO_BLOCK_SAMPLES - samplePartialEndCount; i++)
                        {
                            new_left->data[i] = 0;
                            new_right->data[i] = 0;
                        }
                        sampleBlockPointer = 0;
                    }                 
                    break;
            }
        }
    }

    transmit(new_left, 0);
    release(new_left);
    transmit(new_right, 1);
    release(new_right);	
 }