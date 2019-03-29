#include "control_afs22.h"
#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "AudioControlAFSTwoTwo";

bool AudioControlAFSTwoTwo::configured = false;

void AudioControlAFSTwoTwo::setGPIO(uint8_t value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MCP23008_ADR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MCP23008_REG_IODIR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); //All outputs
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, value, ACK_CHECK_EN); //Set the left input to 5V
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MCP23008 I2C error (%i).", ret);
    }
    else
    {
        ESP_LOGI(TAG, "MCP23008 GPIO update successful (%i).", value);
    }

    i2c_cmd_link_delete(cmd); 
}