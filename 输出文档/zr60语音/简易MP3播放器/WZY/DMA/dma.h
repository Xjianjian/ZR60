#ifndef   __DMA_H
#define   __DMA_H
#include  "sys.h"
//ADC1
void  DMA2_Stream0_Channel0_Init(void);
void  DMA1_Stream4_Channel0_Init(uint8_t* buf0,uint8_t* buf1,uint16_t num);
#endif
