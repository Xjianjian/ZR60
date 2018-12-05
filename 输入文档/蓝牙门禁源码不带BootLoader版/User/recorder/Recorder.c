/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978¼�����ܲ���
******************************************************************************
* @attention
*
* ʵ��ƽ̨:����  STM32 F407 ������  
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include "stm32f4xx.h"
#include "usart/bsp_debug_usart.h"
#include "wm8978/bsp_wm8978.h"
#include "./FATFS/ff.h" 
#include "./recorder/Recorder.h"
#include "./malloc/malloc.h"
#include "string.h"
#include "./systick/bsp_SysTick.h"
int play_n = 0;




/* ¼���ļ����·�� */
#define RECORDERDIR	"0:/recorder"
/* ��Ƶ��ʽ�л��б�(�����Զ���) */
#define FMT_COUNT	6		/* ��Ƶ��ʽ����Ԫ�ظ��� */

/* ¼���ļ�·��ȫ�ƣ���ʼ��Ϊrec001.wav */
static char recfilename[25]={"0:/recorder/rec001.wav"};   
static REC_TYPE Recorder;          /* ¼���豸 */
static WavHead rec_wav;            /* WAV�豸  */
static uint8_t Isread=0;           /* DMA������ɱ�־ */
static uint8_t bufflag=0;          /* ���ݻ�����ѡ���־ */
static uint32_t wavsize=0;         /* wav��Ƶ���ݴ�С */
uint16_t *buffer0;  /* ���ݻ�����1 ��ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */
uint16_t *buffer1;  /* ���ݻ�����2 ��ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */

FIL file;											/* file objects */
FRESULT result; 
UINT bw;            					/* File R/W count */

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
static void DispStatus(void);

static void StartRecord(const char *filename);


void zzw_playconfig(void)
{

		/*  ��ʼ��������I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	buffer0 = (uint16_t *)mymalloc(SRAMEX,RECBUFFER_SIZE*2);
	if(buffer0 != NULL)
	{
		printf("register buffer0 success\n");
	}
	else
	{
		printf("register buffer0 failed\n");
	}
	
	buffer1 = (uint16_t *)mymalloc(SRAMEX,RECBUFFER_SIZE*2);
	if(buffer1 != NULL)
	{
		printf("register buffer1 success\n");
	}
	else
	{
		printf("register buffer1 failed\n");
	}
	I2S_DMA_TX_Callback=MusicPlayer_I2S_DMA_TX_Callback;
	I2S_Play_Stop();
	printf("��ӡ��֤2\n");

}


/**
  * @brief  ����WM8978��STM32��I2S��ʼ������
  * @param  ��
  * @retval ��
  */
void StartPlay(const char *filename,int n)
{
	int i = 0;
	int sound_num;
	DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);
	printf("��ǰ�����ļ� -> %s\n",filename);
	
	result=f_open(&file,filename,FA_READ);
	if(result!=FR_OK)
	{
		printf("����Ƶ�ļ�ʧ��!!!->%d\r\n",result);
		result = f_close (&file);
		Recorder.ucStatus = STA_ERR;
		return;
	}
	else
	{
		printf("��Ƶ�ļ��򿪳ɹ�\n");
	}
	
	play_n = 1;
	//��ȡWAV�ļ�ͷ
	
	result = f_read(&file,&rec_wav,sizeof(rec_wav),&bw);
	for(i = 0;i < RECBUFFER_SIZE*2;i++)
	{
		buffer1[i] = 0x0000;
	}
	//printf("��WAV�ļ�ͷ\n");
	result = f_read(&file,(uint16_t *)buffer1,RECBUFFER_SIZE*2,&bw);
	for(i = 0;i < RECBUFFER_SIZE*2;i++)
	{
		buffer0[i] = 0x0000;
	}
	//printf("������һ\n");
	if(n == 1)
	{
		result = f_read(&file,(uint16_t *)buffer0,RECBUFFER_SIZE*2,&bw);
		play_n = 0;
		//printf("�������\n");
	}
	

	I2S_Stop();			/* ֹͣI2S¼���ͷ��� */

	wm8978_Reset();		/* ��λWM8978����λ״̬ */	

	Recorder.ucStatus = STA_PLAYING;		/* ����״̬ */

	/* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);

	/* ����������������ͬ���� */
	wm8978_SetOUT2Volume(1);

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);

	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);

	
	I2Sxext_RX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);//����˫����

	DMA_ITConfig(I2Sxext_RX_DMA_STREAM,DMA_IT_TC,DISABLE);//������������ж�

  I2Sxext_Recorde_Stop();

	
	I2Sx_TX_DMA_Init(buffer0,buffer1,RECBUFFER_SIZE,n);	

	I2S_Play_Start();
	for(sound_num = 0;sound_num < 1000;sound_num++)
	{
			;
	}
	//delay_ms(1);
	wm8978_SetOUT2Volume(20);
		for(sound_num = 0;sound_num < 1000;sound_num++)
	{
			;
	}
	wm8978_SetOUT2Volume(40);
		for(sound_num = 0;sound_num < 1000;sound_num++)
	{
			;
	}
		wm8978_SetOUT2Volume(63);
		I2Sxext_Recorde_Stop();

}


/* DMA��������жϻص����� */
/* �����������Ѿ�������ɣ���Ҫ�л��������������»��������ݲ��� 
   ͬʱ��ȡWAV�ļ�������䵽�Ѳ�������  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	if(Recorder.ucStatus == STA_PLAYING)
	{

		if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //��ǰʹ��Memory1����
		{
			bufflag=0;                       //���Խ����ݶ�ȡ��������0
		}
		else                               //��ǰʹ��Memory0����
		{
			bufflag=1;                       //���Խ����ݶ�ȡ��������1
		}
		Isread=1;                          // DMA������ɱ�־
	}
		++play_n;
		if(play_n == 2)
		{
			DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);//�ر�DMA TX����,��������
			play_n = 0;
		}
		
}








	
	
	
	


/***************************** (END OF FILE) *********************************/
