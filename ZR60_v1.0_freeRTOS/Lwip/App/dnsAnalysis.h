/******************************************************
文件名：	dnsAnalysis

描述：		

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		_DNS_ANALYSIS_H
#define		_DNS_ANALYSIS_H
/*******************************************************
description： include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/

/**********宏常量定义*********/
#define DNS_ANALYSIS_TASK_PERIOD   5000U//大概30s


#ifdef USE_DHCP
#define DNS_DHCP_STATE    DHCP_state
#else
#define DNS_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif
/***********宏函数***********/


/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	char* name;
}dnsAnalysis_Struct;

/******enum definitions******/
typedef enum
{
	ShortConnect = 0U,	  /*短连接*/
	LngConnect,	  /*长连接*/
	ntp_one,	  /*ntp服务器*/
	ntp_two,	  /*ntp服务器*/
	ntp_three,	  /*ntp服务器*/
	ntp_four,	  /*ntp服务器*/
	ntp_five,	  /*ntp服务器*/
	ntp_six,	  /*ntp服务器*/
	http_download,	  /*http文件下载服务器*/
	//ntp_eight,	  /*ntp服务器*/
	domainNum
}dnsAnalysis_Enum;
/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/



/*******************************************************
description： function External declaration
*******************************************************/
extern void InitdnsAnalysis_parameter(void);
extern void TskdnsAnalysis_MainFunction(void);
extern void GetdnsAnalysis_ipAddr(uint8 index, struct ip_addr *ipaddr);
extern uint8 GetdnsAnalysis_u_ipValidity(uint8 index);
extern void SetdnsAnalysis_ipUpdate(uint8 index);
#endif 
