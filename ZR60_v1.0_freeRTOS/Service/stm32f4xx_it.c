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

#include "./usart/bsp_debug_usart.h"
#include "string.h"
#include "./wm8978/bsp_wm8978.h" 
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
	//if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		/* user code begin */
		
		/* user code end */
		//EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);		
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
		/* user rx code begin */
		
		/* user rx code end */
	}
	
	if(USART_GetITStatus(USART3,USART_IT_TXE) != RESET) 
	{		
		/* user tx code begin */
		
		/* user tx code end */
	}
}
