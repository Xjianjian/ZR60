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
	if(0 == GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		return;
	}
#ifdef USE_DHCP	
	if((DHCP_state == DHCP_TIMEOUT) || (DHCP_state == DHCP_LINK_DOWN))
	{//DHCP���볬ʱ����������ip
		DHCP_state = DHCP_START;
	}
#if 0
	static uint8 dhcpState_old = 0U;
	if(dhcpState_old != DHCP_state)
	{
		dhcpState_old = DHCP_state;
		USART_PRINTF_D("DHCP״̬:%d\n",DHCP_state);
	}
#endif
#endif	
}
