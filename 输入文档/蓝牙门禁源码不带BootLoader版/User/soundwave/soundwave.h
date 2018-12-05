#ifndef __SOUNDWAVE_H
#define	__SOUNDWAVE_H

#include "stm32f4xx.h"

#define CONST_MAX_LOCK_LENGTH 8
union  Ctrl_Flag
{
    struct
    {
        uint8_t b_First_Flag : 1;//check 0x24($)
        uint8_t b_Ctr_Flag  : 1;//data except $,\r,\n
				uint8_t b_Len_Flag  : 1;//data except $,\r,\n
        uint8_t b_Data_Flag : 1; //check 0x0D (\r)
        uint8_t b_Echir_Flag : 1; // check 0x0A
    }bits;
    uint8_t ucProtocolFlag; // 
};

union SoundWaveBuf
{
	uint8_t ucArray_Comm[CONST_MAX_LOCK_LENGTH];
	struct
	{
		uint8_t ucFirstHead;
    uint8_t ucSentHead;
		uint8_t ucData[CONST_MAX_LOCK_LENGTH-4];
		uint8_t ucEnd;
	}bytes;	
};

void inset_data(uint8_t ucSBufTmp);

#endif
