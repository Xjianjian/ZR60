/******************************************************
鏂囦欢鍚嶏細	

鎻忚堪锛

Data			  Vasion			author
2018/1/4		  V1.0			    liujian
*******************************************************/
#ifndef		INCLUDE_H
#define		INCLUDE_H
/*******************************************************
description锛歩nclude the header file
*******************************************************/

/*C标准库文件*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*STM32库文件*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_flash.h"

/*用户文件*/
//Ecal
#include "MyType.h"
#include "./usart/bsp_debug_usart.h"
#include "MemIf.h"
#include "crc32.h"

/*bsp文件*/

#include "./systick/bsp_SysTick.h"
#include "./iwdg/bsp_iwdg.h"
#include "stm32f4xx_iwdg.h"

/*******************************************************
description锛歮acro definitions
*******************************************************/
/**********瀹忓紑鍏冲畾涔*******/

/*******************************************************
description：Often use of macro definitions
*******************************************************/
/************表示开/关状态***************/




/**********瀹忓父閲忓畾涔*******/
#define UART_DEBUG
//#define TIME_DEBUG


#define USER_FLASH_FIRST_PAGE_ADDRESS   ADDR_FLASH_SECTOR_6
/***********瀹忓嚱鏁*********/

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
description锛歴truct definitions
*******************************************************/

/*******************************************************
description锛歵ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint32 Xor;/*本段数据异或取反校验和*/
	uint32 DtFirstAddr;/*数据段首地址*/
	uint32 DtendAddr;/*数据段末地址*/
	uint32 Lng;/*数据长度*/
	uint32 CheckSum;/*升级区数据校验和*/
	uint32 IapFlag;/*在线升级标志*/
}Include_UpdataInfoStruct;//升级信息

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description锛歷ariable External declaration
*******************************************************/
//extern uint16 Vemain_w_5msTskTimer;

/*******************************************************
description锛歠unction External declaration
*******************************************************/

#endif
