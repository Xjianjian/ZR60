/******************************************************
�ļ�����	dnsAnalysis

������		

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		_DNS_ANALYSIS_H
#define		_DNS_ANALYSIS_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/
#define DNS_ANALY_DEBUG
#ifdef 	DNS_ANALY_DEBUG
#define DNS_PRINTF_S(x)   	 printf("%s\n",x)
#define DNS_PRINTF_D(x,d)   	 printf(x,d)
#define DNS_PRINTF_IP(x,d1,d2,d3,d4)    			printf(x,d1,d2,d3,d4)
#else
#define DNS_PRINTF_S(x)    
#define DNS_PRINTF_D(x,d)  
#define DNS_PRINTF_IP(x,d1,d2,d3,d4)
#endif

/**********�곣������*********/
#define DNS_ANALYSIS_SCHEDULING_CYCLE		5U//��������
#define DNS_ANALYSIS_TASK_PERIOD   			(5500U/DNS_ANALYSIS_SCHEDULING_CYCLE)//���5.5s


#ifdef 	USE_DHCP
#define DNS_DHCP_STATE    GetdhcpClient_u_DhcpSt()
#else
#define DNS_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif
/***********�꺯��***********/
#define DNS_GET_PHY_LINK_STATUS() netif_is_link_up(&gnetif)

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	char* name;
}dnsAnalysis_Struct;

/******enum definitions******/
typedef enum
{
	ShortConnect = 0U,	  /*������*/
	LngConnect,	  /*������*/
	ntp_one,	  /*ntp������*/
	ntp_two,	  /*ntp������*/
	ntp_three,	  /*ntp������*/
	ntp_four,	  /*ntp������*/
	ntp_five,	  /*ntp������*/
	ntp_six,	  /*ntp������*/
	http_download,	  /*http�ļ����ط�����*/
	//ntp_eight,	  /*ntp������*/
	domainNum
}dnsAnalysis_Enum;
/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/



/*******************************************************
description�� function External declaration
*******************************************************/
extern void InitdnsAnalysis_parameter(void);
extern void TskdnsAnalysis_MainFunction(void);
extern void GetdnsAnalysis_ipAddr(uint8 index, struct ip_addr *ipaddr);
extern uint8 GetdnsAnalysis_u_ipValidity(uint8 index);
extern void SetdnsAnalysis_ipUpdate(uint8 index);
#endif 
