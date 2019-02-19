/******************************************************
文件名：	dhcpClient

描述：		

Data			  Vasion			author
2018/06/25		  V1.0			    liujian
*******************************************************/
#ifndef		_DHCP_CLIENT_H
#define		_DHCP_CLIENT_H
/*******************************************************
description： include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/

/**********宏常量定义*********/
#define DHCPCLIENT_SCHEDULING_CYCLE	5U//调度周期

/***********宏函数***********/
#define DHCP_GET_PHY_LINK_STATUS()  netif_is_link_up(&gnetif)
#define DhcpClient_dhcp_data(netif) ((struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP))
	
/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/******enum definitions*****/
typedef enum
{
	DHCP_ADDRESS_INVALID = 0U,	  /*DHCP无效*/
	DHCP_ADDRESS_ASSIGNED	  /*DHCP分配*/
}dhcp_Enum;
/*****struct definitions*****/

/******union definitions*****/


/*******************************************************
description： variable External declaration
*******************************************************/



/*******************************************************
description： function External declaration
*******************************************************/
extern void InitdhcpClient_parameter(void);
extern void TskdhcpClient_MainFunction(void);
extern uint8_t GetdhcpClient_u_DhcpSt(void);
extern uint32_t GetdhcpClient_w_SrcIPaddr(void);
#endif 
