#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"
#include "stdio.h"


//Òý½Å¶¨Òå
/*******************************************************/
#define KEY1_INT_GPIO_PORT                GPIOA
#define KEY1_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define KEY1_INT_GPIO_PIN                 GPIO_Pin_0
#define KEY1_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOA
#define KEY1_INT_EXTI_PINSOURCE           EXTI_PinSource0
#define KEY1_INT_EXTI_LINE                EXTI_Line0
#define KEY1_INT_EXTI_IRQ                 EXTI0_IRQn

#define KEY1_IRQHandler                   EXTI0_IRQHandler

#define KEY2_INT_GPIO_PORT                GPIOC
#define KEY2_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define KEY2_INT_GPIO_PIN                 GPIO_Pin_2
#define KEY2_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOC
#define KEY2_INT_EXTI_PINSOURCE           EXTI_PinSource2
#define KEY2_INT_EXTI_LINE                EXTI_Line2
#define KEY2_INT_EXTI_IRQ                 EXTI2_IRQn

#define KEY2_IRQHandler                   EXTI2_IRQHandler



#define KEY3_INT_GPIO_PORT                GPIOG
#define KEY3_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOG
#define KEY3_INT_GPIO_PIN                 GPIO_Pin_15
#define KEY3_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOG
#define KEY3_INT_EXTI_PINSOURCE           EXTI_PinSource15
#define KEY3_INT_EXTI_LINE                EXTI_Line15
#define KEY3_INT_EXTI_IRQ                 EXTI15_10_IRQn

#define KEY3_IRQHandler                   EXTI15_10_IRQHandler

/*******************************************************/


//void EXTI_Key_Config(void);

void EXTI_Key_Config(FunctionalState states);
void i2c_int(FunctionalState state);
void EXTI_Key_DISABLE(FunctionalState states);

#endif /* __EXTI_H */
