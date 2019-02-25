/******************************************************
文件名：	

描述�

Data			  Vasion			author
2018/1/4		  V1.0			    liujian
*******************************************************/
#ifndef		INCLUDE_H
#define		INCLUDE_H


/*******************************************************
description：macro definitions
*******************************************************/
/**********宏开关定�*******/
typedef	unsigned  char			    uint8;
typedef	signed  char			      int8;
typedef	unsigned  short			    uint16;
typedef			      short			    int16;
typedef	unsigned  long 		  		uint32;
typedef			      long 		  		int32;

typedef unsigned long long int  uint64;

typedef signed char 		SC; 	
typedef unsigned char 	UC;	
typedef short 					SS;	
typedef unsigned short 	US;	
typedef long 						SL;	
typedef unsigned long 	UL;
typedef unsigned int 		UI;
typedef signed int 			SI;

/*******************************************************
description��Often use of macro definitions
*******************************************************/
/************��ʾ��/��״̬***************/
#ifndef   STD_ON
#define		STD_ON		  1
#endif

#ifndef   STD_OFF
#define		STD_OFF		0
#endif
/***********��ʾ��ƽ�ĸ�/��**************/
#ifndef   STD_HIGH
#define		STD_HIGH		1
#endif

#ifndef   STD_LOW
#define		STD_LOW		0
#endif

/***********��ʾ��Ч/��Ч***************/
#ifndef   STD_ACTIVE
#define		STD_ACTIVE		  1
#endif

#ifndef   STD_INACTIVE
#define		STD_INACTIVE		0
#endif

/***********��ʾ��/��***************/
#ifndef   STD_TRUE
#define		STD_TRUE			1
#endif

#ifndef   STD_FALSE
#define		STD_FALSE		0
#endif

/***********��ʾ��0***************/
#ifndef   STD_NULL
#define		STD_NULL		0
#endif

/***********��ָ��***************/
#ifndef   STD_NULL_PTR
#define   STD_NULL_PTR   ((void*)0)
#endif



/*******************************************************
description：include the header file
*******************************************************/

#define USART_DEBUGE_PRINTF   //���ڵ��Դ�ӡ����ܿ���

/*******֮ǰ�̼���д�Ĵ������뵱ǰHAL��������*******/
#define ip_addr ip4_addr
#define IPaddress GetdhcpClient_w_SrcIPaddr()
extern struct netif gnetif;
/***************************************************/

/*C��׼���ļ�*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//stm32 ��
#include "iwdg.h"
#include "usart.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "stm32f4xx_hal_gpio.h"


//lwip ��
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/ip4_addr.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"



//Ecal layer
#include	"xor/xor.h"
#include	"JsonIf/cJSON.h"
#include	"JsonIf/JsonIf.h"
#include	"MemIf/MemIf.h"
#include	"BtnFltr/BtnFltr.h"

//lwip app
#include "dhcpClient.h"
#include "dnsAnalysis.h"
#include "ntpclient.h"
#include "./NetConnIf/NetConnIf.h"
#include "tcp_client_LngConnect.h"

//user app
#include	"IcCardUnlock/IcUnlock.h"

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
