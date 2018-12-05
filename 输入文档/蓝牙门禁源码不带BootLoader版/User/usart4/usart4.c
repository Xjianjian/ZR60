/**
  ******************************************************************************
  * @file    bsp_rs232_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief  使用串口2，重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
   
#include "./usart4/usart4.h"


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}


 /**
  * @brief  RS232_USART GPIO 配置,工作模式配置。115200 8-N-1 ，中断接收模式
  * @param  无
  * @retval 无
  */
void USART4_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd( UART4_USART_RX_GPIO_CLK|UART4_USART_TX_GPIO_CLK, ENABLE);

  /* 使能 UART 时钟 */
  RCC_APB1PeriphClockCmd(UART4_USART_CLK, ENABLE);
  
  /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(UART4_USART_RX_GPIO_PORT,UART4_USART_RX_SOURCE, UART4_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(UART4_USART_TX_GPIO_PORT,UART4_USART_TX_SOURCE,UART4_USART_TX_AF);

  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = UART4_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(UART4_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = UART4_USART_RX_PIN;
  GPIO_Init(UART4_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
  /* 配置串口RS232_USART 模式 */
  USART_InitStructure.USART_BaudRate = UART4_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4_USART, &USART_InitStructure); 
	
	NVIC_Configuration();
	/*配置串口接收中断*/
	USART_ITConfig(UART4_USART, USART_IT_RXNE, ENABLE);
	
  USART_Cmd(UART4_USART, ENABLE);
}






/*********************************************END OF FILE**********************/
