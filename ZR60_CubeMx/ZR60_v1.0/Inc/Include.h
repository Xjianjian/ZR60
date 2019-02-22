/******************************************************
鏂囦欢鍚嶏細	

鎻忚堪锛

Data			  Vasion			author
2018/1/4		  V1.0			    liujian
*******************************************************/
#ifndef		INCLUDE_H
#define		INCLUDE_H


/*******************************************************
description锛歮acro definitions
*******************************************************/
/**********瀹忓紑鍏冲畾涔*******/
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
description：Often use of macro definitions
*******************************************************/
/************表示开/关状态***************/
#ifndef   STD_ON
#define		STD_ON		  1
#endif

#ifndef   STD_OFF
#define		STD_OFF		0
#endif
/***********表示电平的高/低**************/
#ifndef   STD_HIGH
#define		STD_HIGH		1
#endif

#ifndef   STD_LOW
#define		STD_LOW		0
#endif

/***********表示有效/无效***************/
#ifndef   STD_ACTIVE
#define		STD_ACTIVE		  1
#endif

#ifndef   STD_INACTIVE
#define		STD_INACTIVE		0
#endif

/***********表示真/假***************/
#ifndef   STD_TRUE
#define		STD_TRUE			1
#endif

#ifndef   STD_FALSE
#define		STD_FALSE		0
#endif

/***********表示清0***************/
#ifndef   STD_NULL
#define		STD_NULL		0
#endif

/***********空指针***************/
#ifndef   STD_NULL_PTR
#define   STD_NULL_PTR   ((void*)0)
#endif



/*******************************************************
description锛歩nclude the header file
*******************************************************/
/*******之前固件库写的代码能与当前HAL库代码兼容*******/
#define ip_addr ip4_addr
#define IPaddress GetdhcpClient_w_SrcIPaddr()
extern struct netif gnetif;
/***************************************************/

/*C标准库文件*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "iwdg.h"

//stm32 库
#include "usart.h"


//lwip 库
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/ip4_addr.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"



//ECU layer
#include	"JsonIf/cJSON.h"
#include	"JsonIf/JsonIf.h"



//lwip app
#include "dhcpClient.h"
#include "dnsAnalysis.h"
#include "ntpclient.h"
#include "./NetConnIf/NetConnIf.h"
#include "tcp_client_LngConnect.h"

//user app
#include	"IcCardUnlock/IcUnlock.h"

/*******************************************************
description锛歴truct definitions
*******************************************************/

/*******************************************************
description锛歵ypedef definitions
*******************************************************/
/*****struct definitions*****/

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
