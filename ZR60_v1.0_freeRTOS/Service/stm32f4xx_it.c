/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./key/bsp_exti.h"
//#include "./tim/bsp_basic_tim.h"

#include "./usart/bsp_debug_usart.h"
#include "./usart3/usart3.h"
#include "./usart4/usart4.h"
#include "./usart5/usart5.h"
#include "./led/bsp_led.h"
#include "string.h"
#include "./wm8978/bsp_wm8978.h" 

//#include "./soundwave/soundwave.h"
#include "./readcard/readcard.h"
#include "stm32f4xx_tim.h"

//uint8_t key_int;
uint8_t volatile update_sec;

//uint8_t b_500ms_flag;
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
#ifdef USART_PRINTF
	printf("NMI_Handler\n");
#endif
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {//出现硬件错误中断，考虑是否堆栈溢出
#ifdef USART_PRINTF
	  printf("HardFault_Handler\n");
#endif
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
#ifdef USART_PRINTF
	  printf("MemManage_Handler\n");
#endif
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
#ifdef USART_PRINTF
	  printf("BusFault_Handler\n");
#endif
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  { 
#ifdef USART_PRINTF	  
	printf("UsageFault_Handler\n");
#endif
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
#ifdef USART_PRINTF
	printf("DebugMon_Handler\n");
#endif
}

#if (0 == FREE_RTOS)
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
#ifdef USART_PRINTF
	printf("SVC_Handler\n");
#endif
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
#ifdef USART_PRINTF
	printf("PendSV_Handler\n");
#endif
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}
#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * 
  */ 
void KEY2_IRQHandler(void)
{
	//确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		SetZR60Ctrl_u_PasswordKey();
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);		
	} 
}


/**
  * @brief  This function handles ethernet DMA interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
    /* Handles all the received frames */
    /* check if any packet received */
    while(ETH_CheckFrameReceived())
	{ 
        /* process received ethernet packet */
		LwIP_Pkt_Handle();
    }
    /* Clear the Eth DMA Rx IT pending bits */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}


#ifdef freeRTOS_RUN_STATUS_DEBUG
volatile uint64_t ulHighFrequencyTimerTicks = 0U;
/* 定时器6中断 */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
void  BASIC_TIM_IRQHandler (void)
{//10us中断
	if( TIM_GetITStatus( TIM6, TIM_IT_Update) != RESET ) 
	{	
		ulHighFrequencyTimerTicks++;
		TimingDelay_Decrement();
		TIM_ClearITPendingBit(TIM6 , TIM_IT_Update);  		
	}		 	
}
#endif


//串口3
void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET) 
	{
		UartCmn_Rx_Msg();
	}
	
	if(USART_GetITStatus(USART3,USART_IT_TXE) != RESET) 
	{		
		UartCmn_Tx_Msg();
	}
}


//串口5中断函数
void UART5_IRQHandler()
{
	ClrZR60Ctrl_BleCnntTimeout();
	if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET) 
    {
		UartCmn_Rx_BleMsg();
	}
	
	if(USART_GetITStatus(UART5,USART_IT_TXE) != RESET) 
	{		
		UartCmn_Tx_BluetoothMsg();
	}
	
}


//串口4中断函数
static uint8_t ucTemp[20];
static uint8_t num = 0;
u8 volatile receive_flag;	
uint32_t ulong = 0;
void UART4_IRQHandler()
{
	uint8_t i;
	if(USART_GetITStatus(UART4, USART_IT_RXNE ) != RESET)
	{		
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		if(num > 20)
		{
			num = 0;	
		}
		ucTemp[num++] = USART_ReceiveData(UART4);
	}
	
	for(i = 0;i < num; i++)
	{
		if( (ucTemp[0+i] == 0xFF ) && (ucTemp[1+i] == 0xFD )  && (ucTemp[7+i] == 0xFF) &&( (num - i) == 8 ))
		{		
			uint32_t  ulong1,ulong2;
			num = 0;
			ulong1 = ucTemp[4];
			ulong2 = ucTemp[5];
			ulong |= ulong1 << 16 | ulong2 <<8 | ucTemp[6] ;
			receive_flag = 1;
		}
	}

	if(USART_GetFlagStatus(UART4,USART_FLAG_ORE) == SET) //溢出 
	{ 	
		USART_ClearFlag(UART4,USART_FLAG_ORE);  //读SR 
		USART_ReceiveData(UART4);		//读DR   
	}
	
}



