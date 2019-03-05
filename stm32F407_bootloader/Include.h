/******************************************************
文件名：	

描述�

Data			  Vasion			author
2018/1/4		  V1.0			    liujian
*******************************************************/
#ifndef		INCLUDE_H
#define		INCLUDE_H
/*******************************************************
description：include the header file
*******************************************************/

/*C��׼���ļ�*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*STM32���ļ�*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_flash.h"

/*�û��ļ�*/
//Ecal
#include "MyType.h"
#include "./usart/bsp_debug_usart.h"
#include "MemIf.h"
#include "crc32.h"

/*bsp�ļ�*/

#include "./systick/bsp_SysTick.h"
#include "./iwdg/bsp_iwdg.h"
#include "stm32f4xx_iwdg.h"

/*******************************************************
description：macro definitions
*******************************************************/
/**********宏开关定�*******/

/*******************************************************
description��Often use of macro definitions
*******************************************************/
/************��ʾ��/��״̬***************/




/**********宏常量定�*******/
#define UART_DEBUG
//#define TIME_DEBUG


#define USER_FLASH_FIRST_PAGE_ADDRESS   ADDR_FLASH_SECTOR_6
/***********宏函�*********/

#ifdef UART_DEBUG
#define USART_PRINTF_S(x)   	 printf("%s\n",x)
#define USART_PRINTF_D(x,d)   	 printf(x,d)
#define USART_PRINTF_CARD_NUM(x,d1,d2,d3,d4)    	printf(x,d1,d2,d3,d4)
#define USART_PRINTF_IP(x,d1,d2,d3,d4)    			printf(x,d1,d2,d3,d4)
#define USART_PRINTF_MAC(x,d1,d2,d3,d4,d5,d6)    	printf(x,d1,d2,d3,d4,d5,d6)
#define USART_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)   	printf(x,d1,d2,d3,d4,d5,d6)
#else
#define USART_PRINTF_S(x)    
#define USART_PRINTF_D(x,d)  
#define USART_PRINTF_CARD_NUM(x,d1,d2,d3,d4)
#define USART_PRINTF_IP(x,d1,d2,d3,d4)
#define USART_PRINTF_MAC(x,d1,d2,d3,d4,d5,d6)
#define USART_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)
#endif


/*******************************************************
description：struct definitions
*******************************************************/

/*******************************************************
description：typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint32 Xor;/*�����������ȡ��У���*/
	uint32 DtFirstAddr;/*���ݶ��׵�ַ*/
	uint32 DtendAddr;/*���ݶ�ĩ��ַ*/
	uint32 Lng;/*���ݳ���*/
	uint32 CheckSum;/*����������У���*/
	uint32 IapFlag;/*����������־*/
}Include_UpdataInfoStruct;//������Ϣ

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description：variable External declaration
*******************************************************/
//extern uint16 Vemain_w_5msTskTimer;

/*******************************************************
description：function External declaration
*******************************************************/

#endif
