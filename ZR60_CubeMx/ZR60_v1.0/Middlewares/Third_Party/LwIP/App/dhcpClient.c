/******************************************************
�ļ�����	dhcpClient

������	
Data			Vasion			author
2018/04/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "dhcpClient.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint32_t SedhcpClient_w_addr = 0;
static unsigned char SedhcpClient_u_DhcpSt = DHCP_ADDRESS_INVALID;

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitdhcpClient_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitdhcpClient_parameter(void)
{
	
}

/******************************************************
*��������TskdhcpClient_MainFunction

*��  �Σ�

*����ֵ��

*��  ����dhcp������,5ms���ڵ���

*��  ע��
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
*��������GetdhcpClient_u_DhcpSt

*��  �Σ�

*����ֵ��

*��  ������ȡdhcp��������ip��ַ��״̬

*��  ע��
******************************************************/
uint8_t GetdhcpClient_u_DhcpSt(void)
{
	return SedhcpClient_u_DhcpSt;
}


/******************************************************
*��������GetdhcpClient_w_SrcIPaddr

*��  �Σ�

*����ֵ��

*��  ������ȡdhcp���������ip��ַ

*��  ע��
******************************************************/
uint32_t GetdhcpClient_w_SrcIPaddr(void)
{
	return SedhcpClient_w_addr;
}


