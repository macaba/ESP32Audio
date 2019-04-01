#ifndef control_pcm3060_h_
#define control_pcm3060_h_

#include "AudioStream.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PCM3060_ADR 0x47
#define PCM3060_REG_64 64
#define PCM3060_REG_65 65
#define PCM3060_REG_66 66
#define PCM3060_REG_67 67
#define PCM3060_REG_68 68
#define PCM3060_REG_69 69
#define PCM3060_REG_70 70
#define PCM3060_REG_71 71
#define PCM3060_REG_72 72
#define PCM3060_REG_73 73
#define PCM3060_PIN_RST 21

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */

class AudioControlPCM3060
{
public:
	AudioControlPCM3060(void){}
	void init(void);
private:
	static bool configured;
};

#endif