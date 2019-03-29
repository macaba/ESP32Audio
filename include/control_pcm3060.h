#ifndef control_pcm3060_h_
#define control_pcm3060_h_

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

#define I2C_MASTER_SCL_IO 19                /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 18                /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0            /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000           /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

class AudioControlPCM3060
{
public:
	AudioControlPCM3060(void){}
	void init(void);
private:
	static bool configured;
};

#endif