#ifndef input_a2dp_h_
#define input_a2dp_h_

#include "Audio.h"
#include "AudioStream.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"
#include <bt_app_core.h>
//#include <bt_app_av.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#include "driver/i2s.h"

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/queue.h"

class AudioInputA2DP : public AudioStream
{
public:
    AudioInputA2DP() : AudioStream(0, NULL, "AudioInputA2DP") { blocking = true; }
    virtual void update(void);
    void start(){ init(); }
private:
    void init();
    //uint32_t inputSampleBuffer[AUDIO_BLOCK_SAMPLES];
};

#endif