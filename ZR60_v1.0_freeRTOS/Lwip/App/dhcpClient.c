/******************************************************
文件名：	dhcpClient

描述：	
Data			Vasion			author
2018/04/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "dhcpClient.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitdhcpClient_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitdhcpClient_parameter(void)
{
	
}

/******************************************************
*函数名：TskdhcpClient_MainFunction

*形  参：

*返回值：

*描  述：dhcp主任务,5ms周期调用

*备  注：
******************************************************/
void TskdhcpClient_MainFunction(void)
{
	if(0 == GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		return;
	}
#ifdef USE_DHCP	
	if((DHCP_state == DHCP_TIMEOUT) || (DHCP_state == DHCP_LINK_DOWN))
	{//DHCP申请超时，重新申请ip
		DHCP_state = DHCP_START;
	}
#if 0
	static uint8 dhcpState_old = 0U;
	if(dhcpState_old != DHCP_state)
	{
		dhcpState_old = DHCP_state;
		USART_PRINTF_D("DHCP状态:%d\n",DHCP_state);
	}
#endif
#endif	
}
