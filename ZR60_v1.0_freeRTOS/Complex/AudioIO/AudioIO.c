/*
******************************************************************************
* @file    AudioIO.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978��Ƶ�ӿ�
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


/* ��Ƶ��ʽ�л��б�(�����Զ���) */
#define FMT_COUNT	6		/* ��Ƶ��ʽ����Ԫ�ظ��� */

/* ¼���ļ�·��ȫ�ƣ���ʼ��Ϊrec001.wav */ 
static REC_TYPE Recorder;          /* ¼���豸 */
//static WavHead rec_wav;            /* WAV�豸  */
static uint8_t Isread=0;           /* DMA������ɱ�־ */
static uint8_t bufflag=0xFF;       /* ���ݻ�����ѡ���־ */

static Recorder_Brdcast Brdcast_buf[2U];  /*�������ݻ���������ŷ��͸�WM8978����Ƶ����*/

const wavFile_Struct wavFile[AudioIO_Num] = 
{
	{1536*4096,45056U},//��ӭ�ؼ�
	{1586*4096,109392U},//��������
	{1636*4096,60672U},//��Ƭ��Ч
	{1686*4096,43520U},//�����ѿ�
	{1736*4096,43520U},//�����豸ȡ��	
	{1786*4096,8256U},//������
	{1836*4096,43520U},//�����豸�ɹ�
	{1886*4096,70912U},//����ĸ��
	{1936*4096,67520U},//һ·˳��
	{1986*4096,43520U},//�������
	{2036*4096,77104U},//�������
	{2086*4096,59824U},//��������ɹ�
	{2136*4096,105904U},//�������ʧ��
	{2186*4096,167120U},//lost door_id
	{2236*4096,199376U},//exist door_id
	{2286*4096,62288U},//�豸��ʼ���ɹ�
	{2336*4096,62288U}//�豸��ʼ��ʧ��
};


//static uint32_t test[8U];/*����ʹ��*/
static uint8_t  SeAudio_u_PlayFile = AudioIO_Empty;
static uint8_t  SeAudio_u_PlayFlag = 0U;
static uint8_t  SeAudio_u_SuspendFlag = 0U;//�Ƴ�ֹͣ���ű�־
static uint32_t SeAudio_dw_DtAddr = 0;//�������ݵ�ַ
static uint32_t SeAudio_dw_Size = 0;//���������ֽ�����С
static uint32_t SeAudio_dw_Cnt = 0;//���������ֽ���������

static uint8_t  SeAudio_u_PlayPriority = 0U;//���������ȼ������ȼ��ߵĿ����ж����ȼ��͵Ĳ���.ֵԽ�����ȼ�Խ�ߣ�Ĭ��0
static uint8_t  SeAudio_u_PlayPriorityTemp = 0U;//�������ȼ���ʱ���棺���ȼ��ߵĿ����ж����ȼ��͵Ĳ���.ֵԽ�����ȼ�Խ�ߣ�Ĭ��0��

uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};

const uint16_t recplaybuf[4]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.

/* �������ļ��ڵ��õĺ������� */
void MusicPlayer_I2S_DMA_TX_Callback(void);
void Recorder_I2S_DMA_RX_Callback(void);
static void AudioIO_VoicePlaying(void);
static void AudioIO_StartPlay(uint32_t Le_dw_addr,uint32_t Le_dw_size);
static void Read_AudioIOPlayFile(uint8_t* Le_u_Data);
static uint8 Audio_FLASH_ReadByte(uint32 Le_u_Addr);

void InitAudioIO_playconfig(void)
{
	/*  ��ʼ��������I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	I2S_DMA_TX_Callback = MusicPlayer_I2S_DMA_TX_Callback;
	//Recorder.ucFmtIdx = 1;
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2Sx_TX_DMA_Init(Brdcast_buf[0].wBrdcast,Brdcast_buf[1].wBrdcast,RECBUFFER_SIZE,1);	
}



/****************************************
  * @brief   ������
	* @note   
  * @param  ��
  * @retval ��
*****************************************/
void TskAudioIO_MainFunction(void)
{
	//if((0U == SPI_FLASH_WrInProgStatus()) && (0U == SeAudio_u_PlayFlag))//flash����
	if((0U == SPI_FLASH_WrInProgStatus()) && (0U == SeAudio_u_PlayFlag) && (1U == GetMemIf_u_Idle()))//flash����
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
		
				/* ȫ���ļ���ȡ��� */
				if(SeAudio_dw_Cnt >= SeAudio_dw_Size)
				{
					//USART_PRINTF_S("ȫ����Ƶ�ļ���ȡ���\r\n");
					SeAudio_u_SuspendFlag = 1U;//�Ƴ�ֹͣ���ű�־
				}
			}
		}
		else
		{
			if(1U == Isread)
			{
				USART_PRINTF_S("�˳�����\r\n");
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
  * @brief  ���ò����ļ���
  * @param  ��
  * @retval ��
  */
void SetAudioIO_PlayFile(uint8_t Le_u_File,uint8_t Le_u_PlayPriority)
{
	SeAudio_u_PlayFile = Le_u_File;
	SeAudio_u_PlayPriorityTemp = Le_u_PlayPriority;
	//USART_PRINTF_D("���ò����ļ� %d\n",SeAudio_u_PlayFile);
	if((0U == SeAudio_u_PlayFlag) || (Le_u_PlayPriority > SeAudio_u_PlayPriority))
	{
		SeAudio_u_PlayFlag = 1U;	
		AudioIO_VoicePlaying();
	}
}


/**
  * @brief  �������õ���Ƶ�ļ���
  * @param  ��
  * @retval ��
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
			AUDIO_PLAY_STOP;//�ر�DMA TX����,��������	
			//����WAV�ļ�ͷ��ʼ��ȡ��Ƶ����
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
  * @brief  ����WM8978��STM32��I2S��ʼ������
  * @param  ��
  * @retval ��
  */
static void AudioIO_StartPlay(uint32_t Le_dw_addr,uint32_t Le_dw_size)
{
	AUDIO_PLAY_STOP;//�ر�DMA TX����,��������
	
	//����WAV�ļ�ͷ��ʼ��ȡ��Ƶ����
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
  * @brief  ��ȡ����״̬
  * @param  ��
  * @retval ��
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


/* DMA��������жϻص����� */
/* �����������Ѿ�������ɣ���Ҫ�л��������������»��������ݲ��� 
   ͬʱ��ȡWAV�ļ�������䵽�Ѳ�������  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //��ǰʹ��Memory1����
	{
		//test[0] = GetSysTick_Timer();
		bufflag=0;                       //���Խ����ݶ�ȡ��������0
	}
	else                               //��ǰʹ��Memory0����
	{
		//test[1] = GetSysTick_Timer();
		bufflag=1;                       //���Խ����ݶ�ȡ��������1
	}
	Isread=1;                          // DMA������ɱ�־
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
