/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978录音功能测试
******************************************************************************
* @attention
*
* 实验平台:秉火  STM32 F407 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
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




/* 录音文件存放路径 */
#define RECORDERDIR	"0:/recorder"
/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT	6		/* 音频格式数组元素个数 */

/* 录音文件路径全称：初始化为rec001.wav */
static char recfilename[25]={"0:/recorder/rec001.wav"};   
static REC_TYPE Recorder;          /* 录音设备 */
static WavHead rec_wav;            /* WAV设备  */
static uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */
static uint32_t wavsize=0;         /* wav音频数据大小 */
uint16_t *buffer0;  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
uint16_t *buffer1;  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */

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

const uint16_t recplaybuf[4]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.

/* 仅允许本文件内调用的函数声明 */
void MusicPlayer_I2S_DMA_TX_Callback(void);
void Recorder_I2S_DMA_RX_Callback(void);
static void DispStatus(void);

static void StartRecord(const char *filename);


void zzw_playconfig(void)
{

		/*  初始化并配置I2S  */
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
	printf("打印验证2\n");

}


/**
  * @brief  配置WM8978和STM32的I2S开始放音。
  * @param  无
  * @retval 无
  */
void StartPlay(const char *filename,int n)
{
	int i = 0;
	int sound_num;
	DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);
	printf("当前播放文件 -> %s\n",filename);
	
	result=f_open(&file,filename,FA_READ);
	if(result!=FR_OK)
	{
		printf("打开音频文件失败!!!->%d\r\n",result);
		result = f_close (&file);
		Recorder.ucStatus = STA_ERR;
		return;
	}
	else
	{
		printf("音频文件打开成功\n");
	}
	
	play_n = 1;
	//读取WAV文件头
	
	result = f_read(&file,&rec_wav,sizeof(rec_wav),&bw);
	for(i = 0;i < RECBUFFER_SIZE*2;i++)
	{
		buffer1[i] = 0x0000;
	}
	//printf("读WAV文件头\n");
	result = f_read(&file,(uint16_t *)buffer1,RECBUFFER_SIZE*2,&bw);
	for(i = 0;i < RECBUFFER_SIZE*2;i++)
	{
		buffer0[i] = 0x0000;
	}
	//printf("读缓冲一\n");
	if(n == 1)
	{
		result = f_read(&file,(uint16_t *)buffer0,RECBUFFER_SIZE*2,&bw);
		play_n = 0;
		//printf("读缓冲二\n");
	}
	

	I2S_Stop();			/* 停止I2S录音和放音 */

	wm8978_Reset();		/* 复位WM8978到复位状态 */	

	Recorder.ucStatus = STA_PLAYING;		/* 放音状态 */

	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);

	/* 调节音量，左右相同音量 */
	wm8978_SetOUT2Volume(1);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);

	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);

	
	I2Sxext_RX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);//配置双缓冲

	DMA_ITConfig(I2Sxext_RX_DMA_STREAM,DMA_IT_TC,DISABLE);//开启传输完成中断

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


/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	if(Recorder.ucStatus == STA_PLAYING)
	{

		if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
		{
			bufflag=0;                       //可以将数据读取到缓冲区0
		}
		else                               //当前使用Memory0数据
		{
			bufflag=1;                       //可以将数据读取到缓冲区1
		}
		Isread=1;                          // DMA传输完成标志
	}
		++play_n;
		if(play_n == 2)
		{
			DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);//关闭DMA TX传输,结束播放
			play_n = 0;
		}
		
}








	
	
	
	


/***************************** (END OF FILE) *********************************/
