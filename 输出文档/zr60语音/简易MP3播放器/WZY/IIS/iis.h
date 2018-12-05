#ifndef   __IIS_H
#define   __IIS_H

#include  "sys.h"

extern void  (*I2S_TX_CallBack)(void);//I2S_TX»Øµ÷º¯Êý
uint8_t I2S2_SamplingRate_Set(uint32_t samplingrate);
void I2S2_Init(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat);
void  I2S_Play_Start(void);
void  I2S_Play_Stop(void);
#endif
