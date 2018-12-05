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
#include "./tim/bsp_basic_tim.h"

#include "./usart/bsp_debug_usart.h"
#include "./usart3/usart3.h"
#include "./usart4/usart4.h"
#include "./usart5/usart5.h"
#include "./led/bsp_led.h"
#include "string.h"
#include "./wm8978/bsp_wm8978.h" 

#include "./soundwave/soundwave.h"
#include "./readcard/readcard.h"





extern void TimingDelay_Decrement(void);
uint8_t key_int;
uint8_t update_sec;

uint8_t b_500ms_flag;

u8 card_set = 0;


void delayMS(u16 time)
{
		u16 i,j;
	  for(i = 0; i < time;i ++)
		{
				for(j = 0; j < 1000;j++)
				{
					 ;
				}
		}
	
}
//串口3

uint8_t r_card[30];
uint8_t readnum = 0;
uint8_t sum;
extern RebackInfo  rebackinfo;
uint8_t read_card_flag = 0;


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
  {}
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
		TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * 
  */ 
u8 card_set_num = 0;
u8 b_3s_flag = 0;
//u8 b_card_sound = 0;
//u8 b_3s_count = 0;
void KEY1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{	
			//设置标志位赋值
	//StartPlay("1:/设置模式.wav",1);
		
		EXTI_ClearITPendingBit(EXTI_Line0);
		card_set_num ++;
		if(card_set_num ==	3)
		{
			card_set_num = 0;
			card_set = 1;
		//	b_3s_count = 0;
			b_3s_flag = 1;
			StartPlay("1:/刷卡配置设备.wav",1);
		}
	}
}






void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		// LED2 取反	


    //清除中断标志位
		key_int = 1;
		printf("key int\n");

		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);

			
	}  
}
u8 inderopen_flag = 0;
u8 inder_num = 0;
void KEY3_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY3_INT_EXTI_LINE) != RESET) 
	{
		// LED2 取反	
	EXTI_ClearITPendingBit(KEY3_INT_EXTI_LINE);

    //清除中断标志位
	//	key_int = 1;
		delayMS(20);
			
		printf("INTT3\n");
//		inder_num ++;
//		if(inder_num >= 2)
//		{
//			inder_num = 0;
			inderopen_flag = 1;
	//	}
	

			
	}  
}


/**
  * 定时器6中断
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


u8 b_10ms_flag = 0;
u8 b_10ms_count = 0;

u8 b_50ms_flag = 0;
u8 b_50ms_count = 0;
u8 b_500ms_count = 0;
u8 b_500ms_flag = 0;
u8 b_1500ms_count = 0;
u8 b_1s_flag = 0;
u8 b_1500ms_flag = 0;
u32 timenew = 0;
u32 timeold= 100;
void  BASIC_TIM_IRQHandler (void)
{
	
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	

		TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);  		

			b_10ms_flag = 1;
			b_10ms_count ++;
		

			if(b_10ms_count >=5)
			{
				b_10ms_count = 0;
				b_50ms_count ++;
				b_50ms_flag = 1;
				timenew++;
				
				if(timenew >= 120000)
				{
					timenew = 0;
					timeold = 100;
				}
					if(b_50ms_count >= 10)
					{
						b_50ms_count = 0;
						 b_500ms_flag = 1;

							b_500ms_count ++;
							b_1500ms_count ++;
							if(b_500ms_count >= 2)
							{
								b_500ms_count = 0;
								 b_1s_flag = 1;
							}

					}
			}

	}		 	
}
#if 0
uint8_t debug[12];
u8 debugbum = 0;

void DEBUG_USART_IRQHandler()
{
	uint8_t i;
	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
	{		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	//	ucTemp = USART_ReceiveData(DEBUG_USART );
	//	printf("ucTemp is %#x\n",ucTemp);

		if(debugbum > 11)
			{
				debugbum = 0;
				
			}
		debug[debugbum] = USART_ReceiveData(DEBUG_USART );
			debugbum++;
			      debugbum &= 0xFF; 
		//printf("ucTemp is %s\n",ucTemp);
    //USART_SendData(DEBUG_USART,ucTemp);   
		//printf("num is %d\n",num);
		}
		
			for(i = 0;i < debugbum; i++)
	{
		if( (debug[0+i] == 0x68 ) && (debug[10+i] == 0x16 )  &&( (debugbum - i) == 11 ))
			{
				
					debugbum = 0;
					switch(debug[1+i])
					{
						case 0x12:
								
								break;
						case 0x13:
								if(strncmp(debug+2+i,"FFFFFFFF",8) == 0)
								{
									build_type = 1;
								}
								else
								{
									build_type = 2;
								}
							
								break;
						
					}
					
					
			}
		}

		


		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) //溢出 

	{ 	USART_ClearFlag(USART1,USART_FLAG_ORE);  //读SR 
			USART_ReceiveData(USART1);		//读DR   
		}
	
	
}	
#endif
//串口3

void USART3_IRQHandler()
{
	int i;
	if(USART_GetITStatus(USART3, USART_IT_RXNE ) != RESET)
	{	
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		if(readnum >=25)
		{
			readnum = 0;
		}
		r_card[readnum] = USART_ReceiveData(USART3);
		readnum++;
		readnum &= 0xFF;
	}

	if(r_card[0] == 0x20)
	{
	
		//printf("%d \n",readnum);
			sum = r_card[1];
		 for(i = 2;i< readnum -2;i++)
		{
			 sum^=r_card[i];
			
		}
		if(r_card[readnum - 2] == (uint8_t)(~sum) && r_card[readnum -1] == 0x03)
		{
				printf("1:");
			 		for(i = 0;i < readnum;i++)
					{
					printf("%#x ",r_card[i]);
					}
					printf("readnum is %d\n ",readnum );
					printf("\n");
					memset(&rebackinfo,0,sizeof(RebackInfo));
					rebackinfo.seqnr = r_card[1];
					rebackinfo.status = r_card[2];
					rebackinfo.length = r_card[3];
					
					for(i = 0;i <r_card[3];i++ )
					{
						rebackinfo.data[i] = r_card[4+i];
					}
					/*for(i = 0;i < r_card[3];i++)
					{
					printf("%#x ",rebackinfo.data[i]);
					}
					printf("\n");*/
					printf(" timenew is  %d \r\n",timenew );
					if(readnum == 14)
					{
						printf(" timeold is %d \r\n",timeold ); 
						 if(timenew - timeold > 20)
						 {
								timeold = timenew;
								read_card_flag  = 1;
						 }
						 else
						 {
									timeold = timenew;
									read_card_flag  = 0;
						 }
						// read_card_flag  = 1; 
					}
					else
					{
						read_card_flag  = 1;
					}
					

				readnum = 0;
				memset(r_card,0,30);
					
		}

		
	}
	else if (r_card[0] == 0x15)
	{
			printf("no ack\n");
					readnum = 0;
		memset(r_card,0,30);
		
	}
	
		if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) == SET) //溢出 
	{ 	USART_ClearFlag(USART3,USART_FLAG_ORE);  //读SR 
			USART_ReceiveData(USART3);		//读DR   
	}
}

//串口5中断函数

char BleTemp[120];
char BleValue[6];
uint8_t  timelist[14];
uint8_t blenum = 0;
uint8_t ble_flag;
uint8_t set_time_flag;
uint8_t ble_flag_num = 0;
u8 build_type;
void UART5_IRQHandler()
{
	int i,inum,j;

		char cCh;

  char * pCh, *pCh1,*pCh2;
	

	
		  if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET) 
    { 
      USART_ClearITPendingBit(UART5,USART_IT_RXNE);
			if(blenum >= 110)
			{
				blenum = 0;
			}
      BleTemp[blenum] = USART_ReceiveData(UART5); 
      blenum++; 
      blenum &= 0xFF; 
			
    }
		if((strcmp(BleTemp,"OK+DISAS") == 0 || strcmp(BleTemp,"OK+DISCE") == 0) && strlen(BleTemp) == 8)
		{	
			printf("size is %d\n",strlen(BleTemp));
			blenum = 0;
			memset(BleTemp,0,sizeof(BleTemp));
		}
		else if (( pCh = strstr (BleTemp, "OK+DISA:" ) ) != 0)
		{
						if(strncmp(pCh+(*(pCh+16)+17),"\r\n",2) == 0)
						{
									switch(*(pCh+16))
									{
												case 0x1D:
																			if( *( pCh+17) == 0x02 && *( pCh+20) == 0x09 && *( pCh+22) == 0x57 && *( pCh+23) == 0x5A && *( pCh+30) == 0x05 && *( pCh+31) == 0x03 && *( pCh+36) == 0x09)
																			{
																				
																							  printf("u8 build_type is %d\n", build_type);
																								for(i = 0;i < 6;i++)
																								{
																										BleValue[i] = *(pCh+40+i);
																								}
																								ble_flag = 1;
																								printf("android sate\n");
																			}
																			break;
												case 0x08:		if(*( pCh+17) == 0x07 && *( pCh+18) == 0x09)
																			{
																					printf("u8 build_type is %d\n", build_type);
																						for(i = 0;i < 6;i++)
																						{
																									 BleValue[i] = *(pCh+19+i);
																						}
																						printf("IOS0 sate\n");
																						ble_flag = 1;
																			}
																			break;
											  case 0x18:
																			if(*( pCh+17) == 0x11)
																			{
																				if(strncmp(pCh+19,"IT",2) == 0 || strncmp(pCh+19,"TL",2) == 0)
																				{
																							strncpy(timelist,pCh+21,14);
																							set_time_flag = 1;
																							printf("ANdroid time sate\n");
																				}
																			}
																			break;
												case 0x12:
																		if(*( pCh+17) == 0x11 && *( pCh+18) == 0x09)
																		{
																					if(strncmp(pCh+19,"TI",2) == 0 || strncmp(pCh+19,"LT",2) == 0)
																					{
																								strncpy(timelist,pCh+21,14);
																								set_time_flag = 1;
																					}
																					printf("IOS time sate\n");
																					break;
																		}
												default:
													           for(i = 0;i< *(pCh+16);i++)
																		{
																				if(*(pCh+17+i) == 0x09 && *(pCh+18+i) == 0xFF && *(pCh+19+i) == 0x57 && *(pCh+20+i) == 0x5A )
																				{
																						printf("u8 build_type is %d\n", build_type);
																						for(j = 0;j < 6;j++)
																						{
																									 BleValue[j] = *(pCh+21+j);
																						}
																						printf("qita shebei\n");
																						ble_flag = 1;
																						break;
																				}
																		}
																		break;
																	
														 
											}
											blenum = 0;
											memset(BleTemp,0,sizeof(BleTemp));									
						}
											
							
		 }
								
	if(USART_GetFlagStatus(UART5,USART_FLAG_ORE) == SET) //溢出 
	{ 	USART_ClearFlag(UART5,USART_FLAG_ORE);  //读SR 
			USART_ReceiveData(UART5);		//读DR   
	}


	
	
}
  uint8_t ucTemp[20];
	uint8_t num = 0;

					u8 receive_flag;	
uint32_t ulong = 0;
//串口4中断函数
void UART4_IRQHandler()
{
 //	uint8_t ucSBufTmp;
	uint8_t i;
	if(USART_GetITStatus(UART4, USART_IT_RXNE ) != RESET)
	{		
		//	ucSBufTmp = USART_ReceiveData(UART4);
		
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
					printf("receive data is %ld\n",ulong);
					receive_flag = 1;
			}
		}

		if(USART_GetFlagStatus(UART4,USART_FLAG_ORE) == SET) //溢出 

	{ 	USART_ClearFlag(UART4,USART_FLAG_ORE);  //读SR 
			USART_ReceiveData(UART4);		//读DR   
		}
	
}


