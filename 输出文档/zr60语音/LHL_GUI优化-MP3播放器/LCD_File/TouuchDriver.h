#ifndef __TOUCH_DRIVER_
#define __TOUCH_DRIVER_
#include "stm32f4xx.h"
#define TOUCH_CS_LOW()			GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define TOUCH_CS_HIGH()			GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define	CHX 	0x90//0x90 	
#define	CHY 	0xD0//0xd0	

#define TOUCHLEFT				3650
#define	TOUCHRIGHT			30000
#define TOUCHTOP				3100
#define	TOUCHBUTTOM			27900

void Touch_Init(void);
u16 Touch_TPReadX(void);
u16 Touch_TPReadY(void);

#endif
