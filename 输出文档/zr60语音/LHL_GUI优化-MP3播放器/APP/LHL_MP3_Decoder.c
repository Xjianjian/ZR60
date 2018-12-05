#include "LHL_MP3_Decoder.h"
#include "mp3dec.h"
#include "mp3common.h"
#include "ff.h"
#include "string.h"
#include "LHL_GUIAlloc.h"
#include "LHL_Font.h"

#include "stm32f4xx.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void LHL_MP3_DecoderProcess(void *p);
/************************************************
 *@breif 初始化中断到44.1KHz
 *
 *
 ***********************************************/
void InterruptOutput_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 1904;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6, ENABLE);
}

/************************************************
 *@breif 初始化DAC的1通道
 *
 *
 ***********************************************/
void DAC_Ch1_Config(void)
{
	DAC_InitTypeDef  DAC_InitStructure;
 /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);

  /* Set DAC Channel1 DHR12L register */
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
}

/************************************************
 *@breif 初始化DAC的2通道
 *
 *
 ***********************************************/
void DAC_Ch2_Config(void)
{
	DAC_InitTypeDef  DAC_InitStructure;
 /* DAC channel2 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_2047;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Set DAC channel2 DHR12RD register */
  DAC_SetChannel2Data(DAC_Align_12b_R, 0);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
}

/************************************************
 *@breif 初始化DAC
 *
 *
 ***********************************************/
void DAC_Config(void)
{
  /* Preconfiguration before using DAC----------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA clock enable (to be used with DAC) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                         
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  /* DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	DAC_Ch1_Config();
	DAC_Ch2_Config();
}



pSound_GroupTypeDef pSound_List = NULL;
volatile short playindex = 0;
volatile unsigned char fatfs_lock=0;

void SoundList_ResAdd(pSound_BuffDef *ppdata,xQueueHandle handle)
{
	GUI_HMEM tmphmem = GUI_ALLOC_AllocNoInit(sizeof(Sound_GroupTypeDef));
	pSound_GroupTypeDef newNude = GUI_ALLOC_h2p(tmphmem);
	newNude->hmem = tmphmem;
	newNude->next = NULL;
	newNude->ppdata = ppdata;
	newNude->ProcessMessageQueue = handle;
	
	if(pSound_List == NULL)
	{
		pSound_List = newNude;
	}
	else
	{
		pSound_GroupTypeDef tmp=pSound_List;
		while(tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = newNude;
	}
}
void SoundList_ResRelease(pSound_BuffDef soundbuff)
{
	pSound_GroupTypeDef Tmp=pSound_List,Tmp1;
	if(pSound_List==NULL)
		return;
	if((pSound_BuffDef)(*Tmp->ppdata)==soundbuff)//如果第一个就是需要删除的
	{
		Tmp=pSound_List->next;
		GUI_ALLOC_Free(pSound_List->hmem);
		pSound_List=Tmp;
		return;
	}
	while(Tmp->next!=NULL&&(pSound_BuffDef)(*Tmp->ppdata)!=soundbuff)//如果当前节点下一个就是需要删除的
	{
		Tmp=Tmp->next;
	}
	if((pSound_BuffDef)(*Tmp->ppdata)==soundbuff)//删除条件满足
	{
		Tmp1=Tmp->next;
		if((Tmp->next)->next==NULL)
		{
			Tmp->next=NULL;
		}
		else
		{
			Tmp->next=(Tmp->next)->next;
		}
		GUI_ALLOC_Free(Tmp1->hmem);
	}
}
void SoundList_Mix(void)
{
	int tmpdata;
	pSound_GroupTypeDef tmp=pSound_List;
	if(pSound_List == NULL)
	{
		return;
	}

	while(tmp->next != NULL)
	{
		short i;
		for(i=0;i<MP3_BUFF_SIZE;i++)
		{
			tmpdata = (*tmp->next->ppdata)->Next->BufData[i]+(*pSound_List->ppdata)->Next->BufData[i];
			if(abs(tmpdata)>32767)
			{
				if(tmpdata>0)
				{
					tmpdata=32767;
				}
				else
				{
					tmpdata=-32767;
				}
			}
			(*pSound_List->ppdata)->Next->BufData[i]=tmpdata;
		}
		tmp = tmp->next;
	}
}

short err;
void LHL_MP3_DecoderCreate(pMP3Player_TypeDef *ppMP3Player,const char *filename)
{
	pSound_BuffDef pmp3buffer;
	FRESULT result;
	unsigned int br;
	
	pMP3Player_TypeDef pMP3Player;
	
	GUI_HMEM newmp3hmem = GUI_ALLOC_AllocNoInit(sizeof(MP3Player_TypeDef));
	pMP3Player = (pMP3Player_TypeDef)GUI_ALLOC_h2p(newmp3hmem);
	*ppMP3Player = pMP3Player;
	pMP3Player->hmem = newmp3hmem;

	pMP3Player->MP3_Buff_hmem = GUI_ALLOC_AllocNoInit(2*sizeof(Sound_BuffDef));
	pMP3Player->readBuf_hmem = GUI_ALLOC_AllocNoInit(MP3_READBUF_SIZE * sizeof(char));

	pmp3buffer = (pSound_BuffDef)GUI_ALLOC_h2p(pMP3Player->MP3_Buff_hmem);	
	pmp3buffer->Next = pmp3buffer+1;
	(pmp3buffer+1)->Next = pmp3buffer;
	pMP3Player->nowBuff=pmp3buffer;
	
	pMP3Player->hMP3Decoder = MP3InitDecoder();
	
	pMP3Player->bytesLeft = 0;
	pMP3Player->offset = 0;
	pMP3Player->readPtr = GUI_ALLOC_h2p(pMP3Player->readBuf_hmem);

	while(fatfs_lock)
	{
		vTaskDelay(1);
	}
	fatfs_lock=1;
	result = f_open(&pMP3Player->fsrc,filename, FA_OPEN_EXISTING | FA_READ);
	fatfs_lock=0;
	if(result)
	{
		//文件读取出错
	}
	
	while(fatfs_lock)
	{
		vTaskDelay(1);
	}
	fatfs_lock=1;
	result = f_read(&pMP3Player->fsrc,GUI_ALLOC_h2p(pMP3Player->readBuf_hmem),
	MP3_READBUF_SIZE,&br);
	fatfs_lock=0;
	
	pMP3Player->bytesLeft += br;
	if(result || br == 0)
	{
		//文件读取出错
	}
	
	pMP3Player->ProcessMessageQueue=xQueueCreate(1,2);
  xQueueSend(pMP3Player->ProcessMessageQueue,(void *)pMP3Player,0);
	
	SoundList_ResAdd(&pMP3Player->nowBuff,pMP3Player->ProcessMessageQueue);
	
	err=xTaskCreate(LHL_MP3_DecoderProcess, ( signed portCHAR * ) "GUI_Pro", 256,
		(void *)ppMP3Player, 10, &pMP3Player->taskHandle );
}
void LHL_MP3_DecoderRelease(pMP3Player_TypeDef *ppMP3Player)
{
	if(*ppMP3Player == NULL)
	{
		return;
	}
	
	vTaskDelete((*ppMP3Player)->taskHandle);
	SoundList_ResRelease((*ppMP3Player)->nowBuff);
	vQueueDelete((*ppMP3Player)->ProcessMessageQueue);
	
	MP3FreeDecoder((*ppMP3Player)->hMP3Decoder);
	f_close(&(*ppMP3Player)->fsrc);
	GUI_ALLOC_Free((*ppMP3Player)->readBuf_hmem);
	GUI_ALLOC_Free((*ppMP3Player)->MP3_Buff_hmem);
	GUI_ALLOC_Free((*ppMP3Player)->hmem);
	
	*ppMP3Player = NULL;
}
void LHL_MP3_DecoderReleaseMyself(pMP3Player_TypeDef *ppMP3Player)
{
	if(*ppMP3Player == NULL)
	{
		return;
	}
	
	SoundList_ResRelease((*ppMP3Player)->nowBuff);
	vQueueDelete((*ppMP3Player)->ProcessMessageQueue);
	
	MP3FreeDecoder((*ppMP3Player)->hMP3Decoder);
	f_close(&(*ppMP3Player)->fsrc);
	GUI_ALLOC_Free((*ppMP3Player)->readBuf_hmem);
	GUI_ALLOC_Free((*ppMP3Player)->MP3_Buff_hmem);
	GUI_ALLOC_Free((*ppMP3Player)->hmem);
	
	*ppMP3Player = NULL;
}

void LHL_MP3_memcpy(void *des,void *src,unsigned int len)
{
	char *pdes = des;
	char *psrc = src;
	while(len--)
	{
		*(char *)pdes++=*(char *)psrc++;
	}
}
void LHL_MP3_DecoderFillBuffer(pMP3Player_TypeDef *ppMP3Player)
{
	pMP3Player_TypeDef pMP3Player = *(pMP3Player_TypeDef *)ppMP3Player;
	FRESULT result;
	unsigned int br;
	unsigned int readcnt = 0;

		int bytes_to_read = MP3_READBUF_SIZE-pMP3Player->bytesLeft;
		
		if(pMP3Player->bytesLeft>0)
		{
			LHL_MP3_memcpy(GUI_ALLOC_h2p(pMP3Player->readBuf_hmem),pMP3Player->readPtr,pMP3Player->bytesLeft);
		}
				while(fatfs_lock)
				{
					vTaskDelay(1);
				}
				fatfs_lock = 1;	
				result = f_read(&pMP3Player->fsrc, (char *)GUI_ALLOC_h2p(pMP3Player->readBuf_hmem) + pMP3Player->bytesLeft,
				bytes_to_read, &br);
				fatfs_lock = 0;	

					pMP3Player->bytesLeft=pMP3Player->bytesLeft + br;
			if( result ||br == 0)
			{
				LHL_MP3_DecoderReleaseMyself(ppMP3Player);
				vTaskDelete(NULL);
			}
		
		if(br < MP3_READBUF_SIZE - pMP3Player->bytesLeft)
		{
			memset((char *)GUI_ALLOC_h2p(pMP3Player->readBuf_hmem) + pMP3Player->bytesLeft,0,MP3_READBUF_SIZE-pMP3Player->bytesLeft -br);
		}

		pMP3Player->readPtr=GUI_ALLOC_h2p(pMP3Player->readBuf_hmem);          
		pMP3Player->offset = 0;      
}

void LHL_MP3_DecoderProcess(void *p)
{
	pMP3Player_TypeDef pMP3Player = *(pMP3Player_TypeDef *)p;
	pMP3Player_TypeDef *ppMP3Player = p;
	FRESULT result;
	unsigned int br;
	
	while(1)
	{
		pMP3Player->offset = MP3FindSyncWord(pMP3Player->readPtr,pMP3Player->bytesLeft);
		
		if (pMP3Player->offset < 0)
		{
			pMP3Player->bytesLeft = 0;
			LHL_MP3_DecoderFillBuffer(p);
		}
		else
		{
			int bytesleftbeforedecode = pMP3Player->bytesLeft - pMP3Player->offset;
			pMP3Player->readPtr += pMP3Player->offset;
			pMP3Player->bytesLeft -= pMP3Player->offset;
			MP3Decode(pMP3Player->hMP3Decoder, &pMP3Player->readPtr, &pMP3Player->bytesLeft,
			pMP3Player->nowBuff->BufData, 0);

			if(pMP3Player->bytesLeft == bytesleftbeforedecode)
			{
				pMP3Player->readPtr+=1;
				pMP3Player->bytesLeft-=1;
			}
			else
			{
				{
					void fft_setdata(short *pdata);
					fft_setdata(pMP3Player->nowBuff->BufData);
				}
			
				xQueueReceive(pMP3Player->ProcessMessageQueue,&br,portMAX_DELAY);
			}
			if (pMP3Player->bytesLeft < MP3_READBUF_SIZE)
			{
				LHL_MP3_DecoderFillBuffer(p);	
			}
		}
	}
}

void TIM6_DAC_IRQHandler(void)
{
	Sound_BuffDef* nowbuffer=NULL;
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	
	if(pSound_List == NULL)
	{
		return;
	}
	
	if(playindex >= 2304)
	{
		return;
	}
	
	if(playindex == 0)
	{
		SoundList_Mix();
	}
	
	nowbuffer=(*(pSound_List->ppdata))->Next;
  /* Set DAC Channel1 DHR12L register */
  DAC_SetChannel1Data(DAC_Align_12b_R, (((short)nowbuffer->BufData[playindex++])+32768)>>4);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
	
  DAC_SetChannel2Data(DAC_Align_12b_R, (((short)nowbuffer->BufData[playindex++])+32768)>>4);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);	
	
	if(playindex == 2304)
	{
		char cIn;
		portBASE_TYPE xHigherPriorityTaskWoken;
		pSound_GroupTypeDef tmp=pSound_List;
		
		xHigherPriorityTaskWoken = pdFALSE;		
		if(pSound_List == NULL)
		{
			return;
		}
		do
		{
			*tmp->ppdata = (*tmp->ppdata)->Next;
			xQueueSendFromISR(tmp->ProcessMessageQueue,&cIn,&xHigherPriorityTaskWoken);
			tmp = tmp->next;
		}while(tmp != NULL);
		
		playindex=0;
	}
}

void LHL_MP3_Decoer_Init(void)
{
	DAC_Config();
	InterruptOutput_Config();
}
