#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void delay_us(__IO u32 nTime);
void delay_ms(__IO u32 mTime);

#endif /* __SYSTICK_H */
