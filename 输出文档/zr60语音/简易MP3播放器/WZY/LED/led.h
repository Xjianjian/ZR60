#ifndef       __LED_H
#define       __LED_H
#include "sys.h"
//#define  LED0(x)  x ? GPIO_SetBits(GPIOF,GPIO_Pin_9):GPIO_ResetBits(GPIOF,GPIO_Pin_9)
//#define  LED1(x)  x ? GPIO_SetBits(GPIOF,GPIO_Pin_10):GPIO_ResetBits(GPIOF,GPIO_Pin_10)

#define  LED0     PFout(9)
#define  LED1     PFout(10)
void LED_Init(void);
#endif

