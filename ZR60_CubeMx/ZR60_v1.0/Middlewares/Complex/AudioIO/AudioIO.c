/*
******************************************************************************
* @file    AudioIO.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978音频接口
******************************************************************************
* @attention
*
* 
* 
* 
*
******************************************************************************
*/
#include "stm32f4xx.h"
#include "usart/bsp_debug_usart.h"
#include "wm8978/bsp_wm8978.h"
#include "./FATFS/ff.h" 
#include "./AudioIO/AudioIO.h"
#include "string.h"
#include "./systick/bsp_SysTick.h"
#include "stm32f4xx_spi.h"


/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT	6		/* 音频格式数组元素个数 */

/* 录音文件路径全称：初始化为rec001.wav */ 
static REC_TYPE Recorder;          /* 录音设备 */
//static WavHead rec_wav;            /* WAV设备  */
static uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0xFF;       /* 数据缓存区选择标志 */

static Recorder_Brdcast Brdcast_buf[2U];  /*播音数据缓存区，存放发送给WM8978的音频数据*/

const wavFile_Struct wavFile[AudioIO_Num] = 
{
	{1536*4096,45056U},//欢迎回家
	{1586*4096,109392U},//消防报警
	{1636*4096,60672U},//卡片无效
	{1686*4096,43520U},//门锁已开
	{1736*4096,43520U},//配置设备取消	
	{1786*4096,8256U},//按键音
	{1836*4096,43520U},//配置设备成功
	{1886*4096,70912U},//配置母卡
	{1936*4096,67520U},//一路顺风
	{1986*4096,43520U},//密码错误
	{2036*4096,77104U},//软件升级
	{2086*4096,59824U},//软件升级成功
	{2136*4096,105904U},//软件升级失败
	{2186*4096,167120U},//lost door_id
	{2236*4096,199376U},//exist door_id
	{2286*4096,62288U},//设备初始化成功
	{2336*4096,62288U}//设备初始化失败
};


//static uint32_t test[8U];/*测试使用*/
static uint8_t  SeAudio_u_PlayFile = AudioIO_Empty;
static uint8_t  SeAudio_u_PlayFlag = 0U;
static uint8_t  SeAudio_u_SuspendFlag = 0U;//推迟停止播放标志
static uint32_t SeAudio_dw_DtAddr = 0;//语音数据地址
static uint32_t SeAudio_dw_Size = 0;//语音数据字节数大小
static uint32_t SeAudio_dw_Cnt = 0;//语音数据字节数计数器

static uint8_t  SeAudio_u_PlayPriority = 0U;//。播放优先级：优先级高的可以中断优先级低的播放.值越大，优先级越高，默认0
static uint8_t  SeAudio_u_PlayPriorityTemp = 0U;//播放优先级临时缓存：优先级高的可以中断优先级低的播放.值越大，优先级越高，默认0。

uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};

const uint16_t recplaybuf[4]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.

/* 仅允许本文件内调用的函数声明 */
void MusicPlayer_I2S_DMA_TX_Callback(void);
void Recorder_I2S_DMA_RX_Callback(void);
static void AudioIO_VoicePlaying(void);
static void AudioIO_StartPlay(uint32_t Le_dw_addr,uint32_t Le_dw_size);
static void Read_AudioIOPlayFile(uint8_t* Le_u_Data);
static uint8 Audio_FLASH_ReadByte(uint32 Le_u_Addr);

void InitAudioIO_playconfig(void)
{
	/*  初始化并配置I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	I2S_DMA_TX_Callback = MusicPlayer_I2S_DMA_TX_Callback;
	//Recorder.ucFmtIdx = 1;
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2Sx_TX_DMA_Init(Brdcast_buf[0].wBrdcast,Brdcast_buf[1].wBrdcast,RECBUFFER_SIZE,1);	
}



/****************************************
  * @brief   主任务
	* @note   
  * @param  无
  * @retval 无
*****************************************/
void TskAudioIO_MainFunction(void)
{
	//if((0U == SPI_FLASH_WrInProgStatus()) && (0U == SeAudio_u_PlayFlag))//flash空闲
	if((0U == SPI_FLASH_WrInProgStatus()) && (0U == SeAudio_u_PlayFlag) && (1U == GetMemIf_u_Idle()))//flash空闲
	{
		AudioIO_VoicePlaying();
	}
	
	if(1U == SeAudio_u_PlayFlag)
	{
		if(0U == SeAudio_u_SuspendFlag)
		{
			if(1U == Isread)
			{
				Isread = 0U;		
				if(bufflag ==0)
				{
					Read_AudioIOPlayFile(Brdcast_buf[0].cBrdcast);
				}
				else if(bufflag ==1)
				{
					Read_AudioIOPlayFile(Brdcast_buf[1].cBrdcast);
				}
				else
				{}
		
				/* 全部文件读取完成 */
				if(SeAudio_dw_Cnt >= SeAudio_dw_Size)
				{
					//USART_PRINTF_S("全部音频文件读取完成\r\n");
					SeAudio_u_SuspendFlag = 1U;//推迟停止播放标志
				}
			}
		}
		else
		{
			if(1U == Isread)
			{
				USART_PRINTF_S("退出播放\r\n");
				Isread = 0U;
				AUDIO_PLAY_STOP;
				AUDIO_PLAY_POWEROFF;
				SeAudio_u_SuspendFlag = 0U;
				SeAudio_u_PlayFlag = 0U;
				SeAudio_dw_Cnt = 0U;
			}
		}
	}
}


/**
  * @brief  设置播放文件。
  * @param  无
  * @retval 无
  */
void SetAudioIO_PlayFile(uint8_t Le_u_File,uint8_t Le_u_PlayPriority)
{
	SeAudio_u_PlayFile = Le_u_File;
	SeAudio_u_PlayPriorityTemp = Le_u_PlayPriority;
	//USART_PRINTF_D("设置播放文件 %d\n",SeAudio_u_PlayFile);
	if((0U == SeAudio_u_PlayFlag) || (Le_u_PlayPriority > SeAudio_u_PlayPriority))
	{
		SeAudio_u_PlayFlag = 1U;	
		AudioIO_VoicePlaying();
	}
}


/**
  * @brief  播放设置的音频文件。
  * @param  无
  * @retval 无
  */
static void AudioIO_VoicePlaying(void)
{
	switch(SeAudio_u_PlayFile)
	{
		case AudioIO_FireAlarm:
		case AudioIO_WelBack:
		case AudioIO_CardInvalid:
		case AudioIO_BonVoyage:
		case AudioIO_ErrKey:
		case AudioIO_ConfDevice:
		case AudioIO_ConfSuccess:
		case AudioIO_DoorOpen:
		case AudioIO_keyVoice:
		case AudioIO_ConfCancel:
		case AudioIO_SoftUpdata:
		case AudioIO_SoftUpdataFinish:
		case AudioIO_SoftUpdataFail:
		case AudioIO_DeInitFailLostId:
		case AudioIO_DeInitFailExistId:
		case AudioIO_DeInitFinish:
		case AudioIO_DeInitFail:	
		{
			//AudioIO_StartPlay(wavFile[SeAudio_u_PlayFile].read_addr,wavFile[SeAudio_u_PlayFile].BytesSize);	
			AUDIO_PLAY_STOP;//关闭DMA TX传输,结束播放	
			//跳过WAV文件头开始读取音频数据
			SeAudio_dw_DtAddr = wavFile[SeAudio_u_PlayFile].read_addr + 80;//sizeof(rec_wav) + 20;
			SeAudio_dw_Size = wavFile[SeAudio_u_PlayFile].BytesSize - 80;//sizeof(rec_wav) -20;
			
			SeAudio_dw_Cnt = 0U;
			
			//USART_PRINTF_D("SeAudio_dw_Size = %d\n",SeAudio_dw_Size);		
			if(bufflag ==0)
			{
				Read_AudioIOPlayFile(Brdcast_buf[0].cBrdcast);
				Read_AudioIOPlayFile(Brdcast_buf[1].cBrdcast);
			}
			else
			{
				Read_AudioIOPlayFile(Brdcast_buf[1].cBrdcast);
				Read_AudioIOPlayFile(Brdcast_buf[0].cBrdcast);
			}

			AUDIO_DRIVERCHIP_PLAYCONFIG;
			SeAudio_u_PlayFlag = 1U;
			AUDIO_PLAY_START;
					
			SeAudio_u_PlayPriority = SeAudio_u_PlayPriorityTemp;
		}
		break;
		default:
		break;
	}
	SeAudio_u_PlayFile = AudioIO_Empty;
	//USART_PRINTF_D("SeAudio_u_PlayFile = %d\n",SeAudio_u_PlayFile);
}


/**
  * @brief  配置WM8978和STM32的I2S开始放音。
  * @param  无
  * @retval 无
  */
static void AudioIO_StartPlay(uint32_t Le_dw_addr,uint32_t Le_dw_size)
{
	AUDIO_PLAY_STOP;//关闭DMA TX传输,结束播放
	
	//跳过WAV文件头开始读取音频数据
	SeAudio_dw_DtAddr = Le_dw_addr + 80;//sizeof(rec_wav) + 20;
	SeAudio_dw_Size = Le_dw_size - 80;//sizeof(rec_wav) -20;
	
	SeAudio_dw_Cnt = 0U;
	
	//USART_PRINTF_D("SeAudio_dw_Size = %d\n",SeAudio_dw_Size);		
	if(bufflag ==0)
	{
		Read_AudioIOPlayFile(Brdcast_buf[0].cBrdcast);
		Read_AudioIOPlayFile(Brdcast_buf[1].cBrdcast);
	}
	else
	{
		Read_AudioIOPlayFile(Brdcast_buf[1].cBrdcast);
		Read_AudioIOPlayFile(Brdcast_buf[0].cBrdcast);
	}

	AUDIO_DRIVERCHIP_PLAYCONFIG;
	SeAudio_u_PlayFlag = 1U;
	AUDIO_PLAY_START;
}


static void Read_AudioIOPlayFile(uint8_t* Le_u_Data)
{
	uint16_t Le_w_i;
	for(Le_w_i = 0;Le_w_i < (RECBUFFER_SIZE*2);Le_w_i++)
	{
		Le_u_Data[Le_w_i] = Audio_FLASH_ReadByte(SeAudio_dw_DtAddr++);
		SeAudio_dw_Cnt++;
	}
}

static uint8 Audio_FLASH_ReadByte(uint32 Le_u_Addr)
{
	uint8 ReadByte;
	SPI_FLASH_BufferRead(&ReadByte,Le_u_Addr,1);
	return ReadByte;
}

/**
  * @brief  获取播放状态
  * @param  无
  * @retval 无
  */
uint8_t GetAudioIO_u_PlaySt(void)
{
	if(SeAudio_u_PlayFlag || (AudioIO_Empty != SeAudio_u_PlayFile))
	{
		return 1U;
	}
	else
	{
		return 0U;
	}
}


/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
	{
		//test[0] = GetSysTick_Timer();
		bufflag=0;                       //可以将数据读取到缓冲区0
	}
	else                               //当前使用Memory0数据
	{
		//test[1] = GetSysTick_Timer();
		bufflag=1;                       //可以将数据读取到缓冲区1
	}
	Isread=1;                          // DMA传输完成标志
	/*if(1U == SeAudio_u_SuspendFlag)
	{
		Isread = 0U;
		AUDIO_PLAY_STOP;
		AUDIO_PLAY_POWEROFF;
		SeAudio_u_SuspendFlag = 0U;
		SeAudio_u_PlayFlag = 0U;
	}*/
}

/***************************** (END OF FILE) *********************************/
