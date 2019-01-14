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
//#define ZR50			//����ʱ��Ϊzr50�豸��δ����ʱ��Ϊzr60�豸
//#define HYM8563		//����ʱ��ʹ���ⲿhym8563ʱ��оƬ
#define NETWORK_ON     	//����ʱ���������繦��
#define UART_DEBUG		//����ʱ���������ڴ�ӡ
//#define WM8978_DEBUG
//#define NET_DEBUG
//#define TIME_DEBUG
//#define freeRTOS_RUN_STATUS_DEBUG


/*C��׼���ļ�*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*bsp�ļ�*/
#include "rtc.h"
#include "./usart/bsp_debug_usart.h"
#include "./usart3/usart3.h"
#include "./usart4/usart4.h"
#include "./usart5/usart5.h"
#include "./led/bsp_led.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "time.h"
#include "./systick/bsp_SysTick.h"
#include "./key/bsp_exti.h"
#include "./hym8563/hym8563.h"
#include "./rtc/bsp_rtc.h"
#include "./display/display.h"
#include "./sm3/SM3.h"
#include "./beep/bsp_beep.h" 
#include "./wm8978/bsp_wm8978.h"
#include "./FATFS/ff.h" 
#include "./AudioIO/AudioIO.h"
#include "./timeStamp/time_stamp.h"
#include "./FATFS/tm_stm32f4_fatfs.h"
#include "./FATFS/diskio.h"	
#include "./flash/fatfs_flash_spi.h"
#include "./adc/bsp_adc.h"
#include "./wdata/wdata.h" 
#include "./iwdg/bsp_iwdg.h"   
#include "./rcc/bsp_clkconfig.h"
#include "stm32f4xx_iwdg.h"
#include "./tim/bsp_basic_tim.h"
//#include "polarssl/ssl.h"

/*STM32���ļ�*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_exti.h"
#include "stm32f429_eth.h"
#include "LAN8742A.h"
#include "stm32f4xx_adc.h"
#include "misc.h"
#include "stm32f4xx_rng.h"
/*�û��ļ�*/
//Ecal
#include "MyType.h"
#include "Mcu_Init.h"
#include "BtnFltr.h"
#include "UartCmn.h"
#include "MemIf.h"
#include "./BListCache/BListCache.h"
#include "./UnlockLogCache/UnlockLogCache.h"
#include "./NetConnIf/NetConnIf.h"
#include "tcp_client_ShortConnect.h"
#include "tcp_client_LngConnect.h"
#include "dnsAnalysis.h"
#include "ntpclient.h"
#include "dhcpClient.h"
#include "JsonIf.h"
#include "cJSON.h"
#include "./STD_ADFliter/STD_ADFliter.h"
#include "./PeriStFliter/PSFltr.h"
#include "./crc32/crc32.h"
#include "http_client_iap.h"
#include "STM8_FM17550_iap.h"
#include "BnryTree.h"

//service
#include "./SleepMng/SleepMng.h"

//App
#include "./ZR60_Ctrl/ZR60_Ctrl.h"
#include "BListMng.h"
#include "./readcard/readcard.h"

/*Lwip���ļ�*/
#include "memp.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netconf.h"
#include "lwip/pbuf.h"
//#include "lwipopts.h"
//#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/ip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"

//freeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "event_groups.h"

/* PolarSSL includes */
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/havege.h"
#include "polarssl/certs.h"
#include "polarssl/x509.h"
/*******************************************************
description：macro definitions
*******************************************************/
/**********宏开关定�*******/

/*******************************************************
description��Often use of macro definitions
*******************************************************/
/***********宏函�*********/

//#define USART_PRINTF

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
