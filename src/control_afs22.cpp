#include "control_afs22.h"

static const char *TAG = "AudioControlAFSTwoTwo";

bool AudioControlAFSTwoTwo::configured = false;

void AudioControlAFSTwoTwo::init()
{
    if(!configured)
    {
        int i2c_master_port = I2C_MASTER_NUM;
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
        i2c_param_config((i2c_port_t)i2c_master_port, &conf);
        i2c_driver_install((i2c_port_t)i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

        configured = true;
    }
}

void AudioControlAFSTwoTwo::calibrate(AudioEffectCalibration* calibrationInL, AudioEffectCalibration* calibrationOutL, AudioEffectCalibration* calibrationInR, AudioEffectCalibration* calibrationOutR)
{
    static const char *TAG = "AudioControlAFSTwoTwoCalibration";
    const int delayMs = 20;     //Worse case buffer delay for the MUX_OUTPUT calibration is 11.60ms
    float in0V, in5V, out0V, out5V;
    calibrationInL->enableAverage();

    //Input L calibration
    calibrationInL->inputNormal();
    //ESP_LOGI(TAG, "Setting left input to 0V.");
    setInputChannelMux(MUX_ZERO_VOLTS, MUX_NONE);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in0V = calibrationInL->getInputAverage(); 
    //ESP_LOGI(TAG, "Setting left input to 5V.");
    setInputChannelMux(MUX_FIVE_VOLTS, MUX_NONE);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in5V = calibrationInL->getInputAverage();
    ESP_LOGI(TAG, "Left input 0V: %.4f, 5V: %.4f", in0V * 10, in5V * 10);
    if(in0V < 0.05 && in0V > -0.05 && in5V > 0.45 && in5V < 0.55)
    calibrationInL->calibrate(0, in0V, 0.5, in5V);

    //ESP_LOGI(TAG, "Setting left input to 0V.");
    setInputChannelMux(MUX_ZERO_VOLTS, MUX_NONE);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in0V = calibrationInL->getOutputAverage(); 
    //ESP_LOGI(TAG, "Setting left input to 5V.");
    setInputChannelMux(MUX_FIVE_VOLTS, MUX_NONE);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in5V = calibrationInL->getOutputAverage();
    ESP_LOGI(TAG, "Left input 0V: %.4f, 5V: %.4f", in0V * 10, in5V * 10);
    if(in0V < 0.001 && in0V > -0.001 && in5V > 0.499 && in5V < 0.501)
    ESP_LOGI(TAG, "Input L calibration confirmed.");
    else
    ESP_LOGE(TAG, "Input L calibration failed.");

    //Output L calibration
    setInputChannelMux(MUX_OUTPUT, MUX_NONE);
    //ESP_LOGI(TAG, "Setting left input to Out L at 0V.");
    calibrationOutL->inputDC(0.0);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out0V = calibrationInL->getOutputAverage();
    //ESP_LOGI(TAG, "Setting left input to Out L at 5V.");
    calibrationOutL->inputDC(0.5);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out5V = calibrationInL->getOutputAverage();
    ESP_LOGI(TAG, "Left output 0V: %.4f, 5V: %.4f", out0V * 10, out5V * 10);
    if(out0V < 0.05 && out0V > -0.05 && out5V > 0.45 && out5V < 0.55) 
    calibrationOutL->calibrate(0, out0V, 0.5, out5V);

    //ESP_LOGI(TAG, "Setting left input to Out L at 0V.");
    calibrationOutL->inputDC(0.0);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out0V = calibrationInL->getOutputAverage();
    //ESP_LOGI(TAG, "Setting left input to Out L at 5V.");
    calibrationOutL->inputDC(0.5);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out5V = calibrationInL->getOutputAverage();
    ESP_LOGI(TAG, "Left output 0V: %.4f, 5V: %.4f", out0V * 10, out5V * 10);
    if(out0V < 0.001 && out0V > -0.001 && out5V > 0.499 && out5V < 0.501) 
    ESP_LOGI(TAG, "Output L calibration confirmed.");
    else
    ESP_LOGE(TAG, "Output L calibration failed.");

    calibrationOutL->inputNormal();
    calibrationInL->disableAverage();
    calibrationInR->enableAverage();

    //Input R calibration
    calibrationInR->inputNormal();
    //ESP_LOGI(TAG, "Setting right input to 0V.");
    setInputChannelMux(MUX_NONE, MUX_ZERO_VOLTS);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in0V = calibrationInR->getInputAverage(); 
    //ESP_LOGI(TAG, "Setting right input to 5V.");
    setInputChannelMux(MUX_NONE, MUX_FIVE_VOLTS);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in5V = calibrationInR->getInputAverage();
    ESP_LOGI(TAG, "Right input 0V: %.4f, 5V: %.4f", in0V * 10, in5V * 10);
    if(in0V < 0.05 && in0V > -0.05 && in5V > 0.45 && in5V < 0.55) 
    calibrationInR->calibrate(0, in0V, 0.5, in5V);

    //ESP_LOGI(TAG, "Setting right input to 0V.");
    setInputChannelMux(MUX_NONE, MUX_ZERO_VOLTS);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in0V = calibrationInR->getOutputAverage(); 
    //ESP_LOGI(TAG, "Setting right input to 5V.");
    setInputChannelMux(MUX_NONE, MUX_FIVE_VOLTS);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    in5V = calibrationInR->getOutputAverage();
    ESP_LOGI(TAG, "Right input 0V: %.4f, 5V: %.4f", in0V * 10, in5V * 10);
    if(in0V < 0.001 && in0V > -0.001 && in5V > 0.499 && in5V < 0.501)
    ESP_LOGI(TAG, "Input R calibration confirmed.");
    else
    ESP_LOGE(TAG, "Input R calibration failed.");

    //Output R calibration
    setInputChannelMux(MUX_NONE, MUX_OUTPUT);
    //ESP_LOGI(TAG, "Setting right input to Out R at 0V.");
    calibrationOutR->inputDC(0.0);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out0V = calibrationInR->getOutputAverage();
    //ESP_LOGI(TAG, "Setting right input to Out R at 5V.");
    calibrationOutR->inputDC(0.5);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out5V = calibrationInR->getOutputAverage();
    ESP_LOGI(TAG, "Right output 0V: %.4f, 5V: %.4f", out0V * 10, out5V * 10);
    if(out0V < 0.05 && out0V > -0.05 && out5V > 0.45 && out5V < 0.55) 
    calibrationOutR->calibrate(0, out0V, 0.5, out5V);

    //ESP_LOGI(TAG, "Setting right input to Out R at 0V.");
    calibrationOutR->inputDC(0.0);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out0V = calibrationInR->getOutputAverage();
    //ESP_LOGI(TAG, "Setting right input to Out R at 5V.");
    calibrationOutR->inputDC(0.5);
    vTaskDelay(delayMs/portTICK_RATE_MS);
    out5V = calibrationInR->getOutputAverage();
    ESP_LOGI(TAG, "Right output 0V: %.4f, 5V: %.4f", out0V * 10, out5V * 10);
    if(out0V < 0.001 && out0V > -0.001 && out5V > 0.499 && out5V < 0.501) 
    ESP_LOGI(TAG, "Output R calibration confirmed.");
    else
    ESP_LOGE(TAG, "Output R calibration failed.");

    calibrationOutR->inputNormal();
    calibrationInR->disableAverage();
    setInputChannelMux(MUX_INPUT, MUX_INPUT);
    //ESP_LOGI(TAG, "Inputs set to input pins.");
}

void AudioControlAFSTwoTwo::setInputChannelMux(afs_channel_mux left, afs_channel_mux right)
{
    uint8_t value = 0;
    switch(left)
    {
        case MUX_NONE:
        break;
        case MUX_ZERO_VOLTS:
        value |= 0x01;
        break;
        case MUX_FIVE_VOLTS:
        value |= 0x10;
        break;
        case MUX_OUTPUT:
        value |= 0x20;
        break;
        case MUX_INPUT:
        value |= 0x02;
        break;
    }
    switch(right)
    {
        case MUX_NONE:
        break;
        case MUX_ZERO_VOLTS:
        value |= 0x04;
        break;
        case MUX_FIVE_VOLTS:
        value |= 0x40;
        break;
        case MUX_OUTPUT:
        value |= 0x80;
        break;
        case MUX_INPUT:
        value |= 0x08;
        break;
    }
    setGPIO(value);
}

void AudioControlAFSTwoTwo::setGPIO(uint8_t value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MCP23008_ADR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MCP23008_REG_IODIR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); //All MUX_OUTPUTs
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