/******************************************************
�ļ�����	dhcpClient

������		

Data			  Vasion			author
2018/06/25		  V1.0			    liujian
*******************************************************/
#ifndef		_DHCP_CLIENT_H
#define		_DHCP_CLIENT_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
#define DHCPCLIENT_SCHEDULING_CYCLE	5U//��������

/***********�꺯��***********/
#define DHCP_GET_PHY_LINK_STATUS()  netif_is_link_up(&gnetif)
#define DhcpClient_dhcp_data(netif) ((struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP))
	
/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/******enum definitions*****/
typedef enum
{
	DHCP_ADDRESS_INVALID = 0U,	  /*DHCP��Ч*/
	DHCP_ADDRESS_ASSIGNED	  /*DHCP����*/
}dhcp_Enum;
/*****struct definitions*****/

/******union definitions*****/


/*******************************************************
description�� variable External declaration
*******************************************************/



/*******************************************************
description�� function External declaration
*******************************************************/
extern void InitdhcpClient_parameter(void);
extern void TskdhcpClient_MainFunction(void);
extern uint8_t GetdhcpClient_u_DhcpSt(void);
extern uint32_t GetdhcpClient_w_SrcIPaddr(void);
#endif 
