#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <inttypes.h>
#include "Include.h"

/* RECBUFFER_SIZE决定两个缓存区大小，实际每个缓存区占用RAM空间字节数为：RECBUFFER_SIZE*2
 * 对于选择I2S_AudioFreq_16k采样率使用RECBUFFER_SIZE为1024*4，就可保证录音及放音效果流畅，
 * 如果选择其他更高采样率需要适当调整用RECBUFFER_SIZE大小，比如当选择择I2S_AudioFreq_44k
 * 是设置RECBUFFER_SIZE为1024*8
 * 另外，SD卡本身的读写速度也在一定性能上影响录音效果
 */
#define RECBUFFER_SIZE  	1024

/*STM32数据传输IIS、DMA配置*/
#define AUDIO_PLAY_START    I2S_Play_Start();//开启DMA TX
#define AUDIO_PLAY_STOP    	I2S_Play_Stop();//关闭DMA TX

/* 音频芯片接口配置 */
#define AUDIO_PLAY_POWEROFF    wm8978_PowerDown();//关闭power 
//驱动芯片播放设置,配置为扬声器输出,菲利普格式,16bit采样
#define AUDIO_DRIVERCHIP_PLAYCONFIG		wm8978_CfgAudioPath(0x10,0x04);\
										wm8978_CfgAudioIF((uint16_t)0x0000,16);\
										wm8978_SetOUT2Volume(50);

/* 播放器待播放音频文件类型 */
typedef enum
{
	AudioIO_WelBack = 0,	/* 欢迎回家  */
	AudioIO_FireAlarm,		/* 消防警报  */
	AudioIO_CardInvalid,	/* 卡片无效  */
	AudioIO_DoorOpen,		/* 门锁已开  */
	AudioIO_ConfCancel,		/* 配置设备取消 */
	AudioIO_keyVoice,		/* 按键音   */
	AudioIO_ConfSuccess,	/*  配置设备成功  */
	AudioIO_ConfDevice,		/*  刷卡配置设备  */
	AudioIO_BonVoyage,		/* 一路顺风 */
	AudioIO_ErrKey,			/*  密码错误  */
	AudioIO_SoftUpdata,		/*  正在进行软件升级  */
	AudioIO_SoftUpdataFinish,		/*  软件升级成功  */
	AudioIO_SoftUpdataFail,		/*  软件升级失败  */
	AudioIO_DeInitFailLostId,		/*  设备初始化失败：未绑定门id  */
	AudioIO_DeInitFailExistId,		/*  设备初始化失败：已被绑定的门id  */
	AudioIO_DeInitFinish,		/*  设备初始化成功  */
	AudioIO_DeInitFail,		/*  设备初始化失败  */
	AudioIO_Num,
	AudioIO_Empty = 0xff	/* 空文件 */
}Audio_PlayFile;

typedef union
{
	uint16_t  wBrdcast[RECBUFFER_SIZE];			/* 播音数据缓存区，存放发送给WM8978的音频数据*/
	uint8_t   cBrdcast[RECBUFFER_SIZE*2];			/* 播音数据缓存区，存放发送给WM8978的音频数据*/
}Recorder_Brdcast;	


typedef struct
{
	uint8_t ucInput;			/* 输入源：0:MIC, 1:线输入 */
	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucGain;			  /* 当前增益 */	
	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */	
}REC_TYPE;	

/* WAV文件头格式 */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*/
	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数	*/
	uint32_t	wave;							/* = "WAVE" 0x45564157*/
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*/
	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16) */
	uint16_t	wFormatTag;				/* 编码方式,一般为1	*/
	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2 */
	uint32_t	dwSamplesPerSec;	/* 采样率 */
	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) */
	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数) */
	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) */
																			  
	uint32_t	data;							/* = "data" 0x61746164*/
	uint32_t	datasize;					/* 纯数据长度 */
} WavHead;	

typedef struct
{
	uint32_t read_addr;			/* 音频数据起始地址 */
	uint32_t  BytesSize;			/* 音频数据字节长度 */
}wavFile_Struct;


/* API接口 */
extern void InitAudioIO_playconfig(void);
extern void TskAudioIO_MainFunction(void);
extern void SetAudioIO_PlayFile(uint8_t Le_u_File,uint8_t Le_u_PlayPriority);
extern uint8_t GetAudioIO_u_PlaySt(void);
#endif  /* __RECORD_H__   */

