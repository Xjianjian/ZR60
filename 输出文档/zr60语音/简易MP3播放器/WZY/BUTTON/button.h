#ifndef   __BUTTON_H
#define   __BUTTON_H
#include  "sys.h"

#define   ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)
extern __IO uint16_t ADCConvertedValue[2];
void Button_Init(void);

#endif
