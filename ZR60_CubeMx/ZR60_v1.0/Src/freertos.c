/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "Include.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId lwip_taskHandle;
osThreadId FeedDogHandle;
osThreadId zr60_TaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartLwipTask(void const * argument);
void vTaskFeedDog(void const * argument);
void StartZr60FuncTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
static void get_resert_status(void);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  get_resert_status();     
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of lwip_task */
  osThreadDef(lwip_task, StartLwipTask, osPriorityNormal, 0, 1024);
  lwip_taskHandle = osThreadCreate(osThread(lwip_task), NULL);

  /* definition and creation of FeedDog */
  osThreadDef(FeedDog, vTaskFeedDog, osPriorityHigh, 0, 256);
  FeedDogHandle = osThreadCreate(osThread(FeedDog), NULL);

  /* definition and creation of zr60_Task */
  osThreadDef(zr60_Task, StartZr60FuncTask, osPriorityLow, 0, 1024);
  zr60_TaskHandle = osThreadCreate(osThread(zr60_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartLwipTask function */
void StartLwipTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();

  /* USER CODE BEGIN StartLwipTask */
	InitBtnFltr_Parameter();
	InitdhcpClient_parameter();
	InitdnsAnalysis_parameter();
	Initntpclient_Pramater();
	NetConnIf_Parameter();
	tcp_LngConnect_Parameter();
  /* Infinite loop */
  for(;;)
  {
		TskBtnFltr_MainFunction();
		
		TskdhcpClient_MainFunction();
		TskdnsAnalysis_MainFunction();
		Tskntpclient_MainFunction();
		tcp_LngConnect_MainFunction();
    osDelay(5);
  }
  /* USER CODE END StartLwipTask */
}

/* vTaskFeedDog function */
void vTaskFeedDog(void const * argument)
{
  /* USER CODE BEGIN vTaskFeedDog */
	printf("看门狗开启....\r\n");
	/* Start the IWDG */
	MX_IWDG_Init();

  /* Infinite loop */
  for(;;)
  {
		/* Refresh IWDG: reload counter */
		if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
		{
			/* Refresh Error */
			printf("你不按时喂狗....\r\n");
			Error_Handler();
		}
		
    osDelay(100);
  }
  /* USER CODE END vTaskFeedDog */
}

/* StartZr60FuncTask function */
void StartZr60FuncTask(void const * argument)
{
  /* USER CODE BEGIN StartZr60FuncTask */
	MemIf_Init();
	InitIcUnlock_parameter();
  /* Infinite loop */
  for(;;)
  {
		
		TskMemIf_MainFunction();
		TskIcUnlock_MainFunction();
    osDelay(10);
  }
  /* USER CODE END StartZr60FuncTask */
}

/* USER CODE BEGIN Application */
static void get_resert_status(void)
{
	#if defined(USART_DEBUGE_PRINTF)
    if( READ_BIT(RCC->CSR, RCC_CSR_PINRSTF) != RESET)
    {
        printf("复位按键复位\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_BORRSTF) != RESET)
    {
        printf("低于电压阈值复位\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF) != RESET)
    {
        printf("软件复位\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF) != RESET)
    {
        printf("独立看门狗复位\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF) != RESET)
    {
        printf("窗口看门狗复位\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF) != RESET)
    {
        printf("低功耗非法模式复位\n");
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
		#endif
}		
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
