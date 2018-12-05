#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "Include.h"

extern void SysTick_Init(void);
extern void delay_10us(__IO u32 nTime);
extern void delay_ms(__IO u32 mTime);
extern uint64_t GetSysTick_Timer(void);
extern uint32_t GetSysTick_delta_T(uint32_t T1,uint32_t T2);
extern void TimingDelay_Decrement(void);
#endif /* __SYSTICK_H */
