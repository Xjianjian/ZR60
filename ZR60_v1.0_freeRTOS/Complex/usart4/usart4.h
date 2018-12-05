#ifndef __RS232_USART4_H
#define	__RS232_USART4_H

#include "stm32f4xx.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define UART4_USART                             UART4
#define UART4_USART_CLK                         RCC_APB1Periph_UART4

#define UART4_USART_RX_GPIO_PORT                GPIOC
#define UART4_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define UART4_USART_RX_PIN                      GPIO_Pin_11
#define UART4_USART_RX_AF                       GPIO_AF_UART4
#define UART4_USART_RX_SOURCE                   GPIO_PinSource11

#define UART4_USART_TX_GPIO_PORT                GPIOC
#define UART4_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define UART4_USART_TX_PIN                      GPIO_Pin_10
#define UART4_USART_TX_AF                       GPIO_AF_UART4
#define UART4_USART_TX_SOURCE                   GPIO_PinSource10

          
#define UART4_USART_IRQ                 				UART4_IRQn
/************************************************************/



//串口波特率
#define UART4_USART_BAUDRATE                    115200



void USART4_Config(void);
//void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
