#ifndef __RS232_USART_H
#define	__RS232_USART_H

#include "stm32f4xx.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define UART3_USART                             USART3
#define UART3_USART_CLK                         RCC_APB1Periph_USART3

#define UART3_USART_RX_GPIO_PORT                GPIOB
#define UART3_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define UART3_USART_RX_PIN                      GPIO_Pin_11
#define UART3_USART_RX_AF                       GPIO_AF_USART3
#define UART3_USART_RX_SOURCE                   GPIO_PinSource11

#define UART3_USART_TX_GPIO_PORT                GPIOB
#define UART3_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define UART3_USART_TX_PIN                      GPIO_Pin_10
#define UART3_USART_TX_AF                       GPIO_AF_USART3
#define UART3_USART_TX_SOURCE                   GPIO_PinSource10

          
#define UART3_USART_IRQ                 				USART3_IRQn
/************************************************************/


//串口波特率
#define UART3_USART_BAUDRATE                    9600



void USART3_Config(void);
//void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
