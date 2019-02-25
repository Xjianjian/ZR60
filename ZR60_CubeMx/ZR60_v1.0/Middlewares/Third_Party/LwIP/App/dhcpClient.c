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
static uint32_t SedhcpClient_w_addr = 0;
static unsigned char SedhcpClient_u_DhcpSt = DHCP_ADDRESS_INVALID;

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
	struct dhcp* Le_dhcp;
	static uint8_t ip_displayed = 0;
	if(0 == DHCP_GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		SedhcpClient_u_DhcpSt = DHCP_ADDRESS_INVALID;
		return;
	}

	if (dhcp_supplied_address(&gnetif))
  {
		Le_dhcp = DhcpClient_dhcp_data(&gnetif);
		SedhcpClient_w_addr = Le_dhcp->offered_ip_addr.addr;
		SedhcpClient_u_DhcpSt = DHCP_ADDRESS_ASSIGNED;
		if(!ip_displayed)
		{
			ip_displayed = 1;
			#if defined(USART_DEBUGE_PRINTF)
			printf("IP address: %s\n", ip4addr_ntoa(&Le_dhcp->offered_ip_addr));
			printf("Subnet mask: %s\n", ip4addr_ntoa(&Le_dhcp->offered_sn_mask));
			printf("Default gateway: %s\n", ip4addr_ntoa(&Le_dhcp->offered_gw_addr));
			#endif
		}
  }
	else
	{
		SedhcpClient_u_DhcpSt = DHCP_ADDRESS_INVALID;
		ip_displayed = 0;
	}
}


/******************************************************
*函数名：GetdhcpClient_u_DhcpSt

*形  参：

*返回值：

*描  述：获取dhcp分配主机ip地址的状态

*备  注：
******************************************************/
uint8_t GetdhcpClient_u_DhcpSt(void)
{
	return SedhcpClient_u_DhcpSt;
}


/******************************************************
*函数名：GetdhcpClient_w_SrcIPaddr

*形  参：

*返回值：

*描  述：获取dhcp分配的主机ip地址

*备  注：
******************************************************/
uint32_t GetdhcpClient_w_SrcIPaddr(void)
{
	return SedhcpClient_w_addr;
}


