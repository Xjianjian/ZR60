#ifndef __LHL_MP3_DECODER_H_
#define __LHL_MP3_DECODER_H_

#include "mp3dec.h"
#include "mp3common.h"
#include "LHL_GUIAlloc.h"
#include "ff.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define MP3_BUFF_SIZE 2304
#define MP3_READBUF_SIZE 4608
typedef struct buffs
{
	short BufData[MP3_BUFF_SIZE];
	struct buffs * Next;
}Sound_BuffDef,*pSound_BuffDef;

typedef struct mp3player_struct
{
	GUI_HMEM hmem;
	GUI_HMEM MP3_Buff_hmem;
	
	pSound_BuffDef nowBuff;
	
	HMP3Decoder hMP3Decoder;
	int bytesLeft;
	GUI_HMEM readBuf_hmem;
	unsigned char *readPtr;
	FIL fsrc;
	int offset;
	
	xTaskHandle taskHandle;
	xQueueHandle ProcessMessageQueue;
	
}MP3Player_TypeDef,*pMP3Player_TypeDef;

typedef struct sound_group
{
	pSound_BuffDef *ppdata;
	xQueueHandle ProcessMessageQueue;
	struct sound_group *next;
	GUI_HMEM hmem;
}Sound_GroupTypeDef,*pSound_GroupTypeDef;

void LHL_MP3_Decoer_Init(void);

void LHL_MP3_DecoderCreate(pMP3Player_TypeDef *ppMP3Player,const char *filename);

void LHL_MP3_DecoderRelease(pMP3Player_TypeDef *ppMP3Player);
#endif
