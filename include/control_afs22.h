#ifndef control_afs22_h_
#define control_afs22_h_
#include "AudioStream.h"
#include "effect_calibration.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define MCP23008_ADR 0x24
#define MCP23008_REG_IODIR 0x00
#define MCP23008_REG_IPOL 0x01
#define MCP23008_REG_GPINTEN 0x02
#define MCP23008_REG_DEFVAL 0x03
#define MCP23008_REG_INTCON 0x04
#define MCP23008_REG_IOCON 0x05
#define MCP23008_REG_GPPU 0x06
#define MCP23008_REG_INTF 0x07
#define MCP23008_REG_INTCAP 0x08 //(Read-only)
#define MCP23008_REG_GPIO 0x09
#define MCP23008_REG_OLAT 0x0A

#define I2C_MASTER_SCL_IO 19                /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 18                /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0            /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400000           /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */

typedef enum {
	MUX_NONE = 0,
    MUX_ZERO_VOLTS = 1,
    MUX_FIVE_VOLTS = 2,
    MUX_OUTPUT = 3,
	MUX_INPUT = 4
} afs_channel_mux;

class AudioEffectCalibration;

class AudioControlAFSTwoTwo
{
public:
	AudioControlAFSTwoTwo(void){}
	void init(void);
	void calibrate(AudioEffectCalibration* calibrationInL, AudioEffectCalibration* calibrationOutL, AudioEffectCalibration* calibrationInR, AudioEffectCalibration* calibrationOutR);
	void setInputChannelMux(afs_channel_mux left, afs_channel_mux right);
private:
	static bool configured;
	void setGPIO(uint8_t value);
};

#endif