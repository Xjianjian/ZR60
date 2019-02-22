#ifndef __RS232_USART5_H
#define	__RS232_USART5_H

#include "stm32f4xx.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define UART5_USART                             UART5
#define UART5_USART_CLK                         RCC_APB1Periph_UART5

#define UART5_USART_RX_GPIO_PORT                GPIOD
#define UART5_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define UART5_USART_RX_PIN                      GPIO_Pin_2
#define UART5_USART_RX_AF                       GPIO_AF_UART5
#define UART5_USART_RX_SOURCE                   GPIO_PinSource2

#define UART5_USART_TX_GPIO_PORT                GPIOC
#define UART5_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define UART5_USART_TX_PIN                      GPIO_Pin_12
#define UART5_USART_TX_AF                       GPIO_AF_UART5
#define UART5_USART_TX_SOURCE                   GPIO_PinSource12

          
#define UART5_USART_IRQ                 				UART5_IRQn 
/************************************************************/


//串口波特率
#define UART5_USART_BAUDRATE                    115200



void USART5_Config(void);
//void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

//int fputc(int ch, FILE *f);
//void bledata_deel(char *BleTemp,u8 blenum);

#endif /* __USART1_H */
