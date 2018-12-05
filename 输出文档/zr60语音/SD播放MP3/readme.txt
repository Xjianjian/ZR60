使用Helix开源代码实现软解播放MP3。
循环播放\music下所有的MP3文件，串口115200bps，播放时输出歌曲信息。
MP3文件支持标准的采用率（27000这种非标准采样率不支持），支持CBR与VBR格式
主芯片使用STM32F103RET,STM32F103VET,STM32F103ZET及更高RAM的芯片
SD卡仅支持FAT32文件格式（exFAT不支持）
不支持长文件名，只支持8.3文件名格式，不支持中文文件名
播放的音频文件需要放在\music目录下

硬件连接方法
SD卡：
SDIO_D0		PC8
SDIO_D1		PC9
SDIO_D2		PC10
SDIO_D3		PC11
SDIO_CMD	PD2
SDIO_CLK	PC12

WM8978模块（淘宝上有卖）
I2S_SD		PB15	SDIN
I2S_CK		PB13	SCLK
I2S_WS		PB12	LRCK
I2S_MCK		PC6	MCLK
I2C_SDA		PB7	SDA
I2C_SCL		PB6	SCL

串口
USART_RX	PA10
USART_TX	PA9