/******************************************************
�ļ�����	ntpclient

������		
Data			Vasion			author
2018/2/1		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"ntpclient.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
//static uint8 ntpRcvBuf[50];
struct udp_pcb *ntpclient_upcb;//��Ƶ�������շ�UDP���ƿ�
//static uint8 Linkflag;
static uint32 Sentpclient_dw_TskTimer;  //���������
static uint32 Sentpclient_dw_Timeout;  //udp�Ͽ����Ӽ�����
//static uint32_t Receive_Timestamp; //Time at the server when the request arrived from the client
static uint32_t SeTransmit_dw_Timestamp; //Time at the server when the response left for the client
static uint8 Sentpclient_u_TSUpdateFlg;//ʱ������±�־

static ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };// Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
static char ntpclient_sever[NTPCLIENT_SEVER_NUM] = 
{
	ntp_one,	  /*ntp������*/
	ntp_two,	  /*ntp������*/
	ntp_three,  /*ntp������*/
	ntp_four,	  /*ntp������*/
	ntp_five,	  /*ntp������*/
	ntp_six	  /*ntp������*/
};
static ntpclient_struct ntpclient;
/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void ntpclient_RenewRTC(void);

static void ntpclient_Rcv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
static void ntpclient_Release(void);
static void ntpclient_Send(char *data,uint16_t Lng);
static uint8 ntpclient_connect(struct ip_addr SrcIPaddr,struct ip_addr DestIPaddr,uint16 udp_LocalPort,uint16 udp_RemotePort);
/******************************************************
description�� function code
******************************************************/

/**********************���ýӿ� BEGIN******************/
/******************************************************
*��������ntpclient_RenewRTC

*��  �Σ�

*����ֵ��

*��  ����У׼RTCʱ��оƬʱ��

*��  ע��
******************************************************/
static void ntpclient_RenewRTC(void)
{
	char Le_u_timeString[15] = {0};
	LocalRTC_timestamp2strBJtime(SeTransmit_dw_Timestamp,Le_u_timeString);//ʱ���ת��Ϊ����ʱ�䣨�ַ�����ʽ��
	NTP_PRINTF_D("��ȡ������ʱ�䣺%s\n",Le_u_timeString);
	SetLocalRTC_datetime(Le_u_timeString,TIME_STAMP_BJT);//У׼ʱ��оƬʱ��
	ntpclient.CalTimeFlag = 1U;
}
/**********************���ýӿ� END*********************/


/******************************************************
*��������Initntpclient_Pramater

*��  �Σ�

*����ֵ��

*��  ��������һ��δ���ӵ�udp���ƿ�

*��  ע��
******************************************************/
void Initntpclient_Pramater(void)
{
	Sentpclient_dw_TskTimer = NTPCLIENT_TSK_PERIOD - (8000U/NTPCLIENT_SCHEDULING_CYCLE);
	Sentpclient_dw_Timeout = 0U;
	ntpclient.SubsIndex = 0U;
	ntpclient.TxBusyFlag = 0U;
	ntpclient.echoFlag = 0U;
	SeTransmit_dw_Timestamp = 0U; //Time at the server when the response left for the client
	Sentpclient_u_TSUpdateFlg = 0U;//ʱ������±�־
	ntpclient.CalTimeFlag = 0U;
}


/******************************************************
*��������Tskntpclient_MainFunction

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void Tskntpclient_MainFunction(void)
{
	char Le_u_i;
	if((0 == NTP_GET_PHY_LINK_STATUS()) || (DHCP_ADDRESS_ASSIGNED != NTP_DHCP_STATE))/* Get Ethernet link status*/
	{
		Sentpclient_dw_TskTimer = NTPCLIENT_TSK_PERIOD - (8000U/NTPCLIENT_SCHEDULING_CYCLE);
		Sentpclient_dw_Timeout = 0U;
		ntpclient.CalTimeFlag = 0U;
		return;
	}
	
	if((Sentpclient_dw_TskTimer >= NTPCLIENT_TSK_PERIOD)&&(0U == ntpclient.TxBusyFlag))
	{
		ntpclient.CalTimeFlag = 0U;//by liujian ,20181029
		if(1U == GetdnsAnalysis_u_ipValidity(ntpclient_sever[ntpclient.SubsIndex]))
		{//��ǰntp���������������ɹ�
			ntpclient.SrcIP.addr = IPaddress;
			GetdnsAnalysis_ipAddr(ntpclient_sever[Le_u_i],(struct ip_addr*)(&ntpclient.remoteIP.addr));//��ȡntp������ip
			NTP_PRINTF_D("ntp������ %d ip��ַ��\n",ntpclient.SubsIndex);
			NTP_PRINTF_IP("%u.%u.%u.%u\n",(uint8_t)ntpclient.remoteIP.addr, \
							(uint8_t)(ntpclient.remoteIP.addr >> 8),	\
							(uint8_t)(ntpclient.remoteIP.addr >> 16),	\
							(uint8_t)(ntpclient.remoteIP.addr >> 24));
			if(1U == ntpclient_connect(ntpclient.SrcIP,ntpclient.remoteIP,NTPCLIENT_SRC_PORT,NTPCLIENT_REMOTE_PORT))
			{//�������ӣ�����ntpЭ�鱨��
				memset(&(packet.li_vn_mode), 0, NTP_PKT_LEN);
				packet.li_vn_mode = NTP_LI_NO_WARNING|NTP_VERSION|NTP_MODE_CLIENT;
				ntpclient_Send((char*)(&(packet.li_vn_mode)),NTP_PKT_LEN);
				ntpclient.TxBusyFlag = 1U;
				ntpclient.echoFlag = 1U;
				Sentpclient_dw_Timeout = 0U;
			}
		}
		else
		{//��ǰntp����������δ�ɹ�����
			ntpclient.SubsIndex++;//������һ̨ntp������
		}
	}
	else
	{
		Sentpclient_dw_TskTimer++;
	}
	
	if(1U == ntpclient.echoFlag)
	{//�ȴ���Ӧ
		Sentpclient_dw_Timeout++;
		if(Sentpclient_dw_Timeout >= NTP_CONNECT_TIMEOUT)
		{//��Ӧ��ʱ
			Sentpclient_dw_Timeout = 0U;
			ntpclient.TxBusyFlag = 0U;
			ntpclient.echoFlag = 0U;
			NTP_PRINTF_D("\r\nErrorLogging��ntp������ %d ��Ӧ��ʱ\r\n",ntpclient.SubsIndex);
			SetdnsAnalysis_ipUpdate(ntpclient_sever[ntpclient.SubsIndex]);//����ip
			ntpclient.SubsIndex++;//��ǰntp��������Ӧ��ʱ���´η�����һ��ntp������
			ntpclient_Release();
		}
	}
	else
	{
		if(2U == ntpclient.echoFlag)
		{//�յ���Ӧ
			Sentpclient_dw_Timeout = 0U;
			ntpclient.TxBusyFlag = 0U;
			ntpclient.echoFlag = 0U;
			Sentpclient_dw_TskTimer = 0U;
			ntpclient_Release();	
		}
	}
	
	if(NTPCLIENT_SEVER_NUM == ntpclient.SubsIndex)
	{
		ntpclient.SubsIndex = 0U;
		Sentpclient_dw_TskTimer = 0U;//�������ʱ��������Ƶ������
	}
	
	/* У׼ʱ��оƬʱ�� */
	if(1U == Sentpclient_u_TSUpdateFlg)
	{
		NTP_PRINTF_S("\n\r����ͬ������ʱ��оƬʱ��");
		Sentpclient_u_TSUpdateFlg = 0U;
		ntpclient_RenewRTC();
	}
}

/******************************************************
*��������ntpclient_connect

*��  �Σ�

*����ֵ��

*��  ��������udp����

*��  ע��
******************************************************/
uint8 ntpclient_connect(struct ip_addr SrcIPaddr,struct ip_addr DestIPaddr,uint16 udp_LocalPort,uint16 udp_RemotePort)
{
   err_t err;
   uint8 Le_u_Ok = 0;
   
   ntpclient_upcb = udp_new();
   if (NULL != ntpclient_upcb)
   {
     /* Bind the ntpclient_upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the ntpclient_upcb to be used by any local interface */
      err = udp_bind(ntpclient_upcb, &SrcIPaddr, udp_LocalPort);//�󶨱���IP�ͱ��ض˿ں�
	  ip_addr_set(&ntpclient_upcb->remote_ip, &DestIPaddr);
	  ntpclient_upcb->remote_port = udp_RemotePort;
	  ntpclient_upcb->flags |= UDP_FLAGS_UNCONNECTED;//PCBk���ƿ����ӱ�־��Ч
      if(err == ERR_OK)
      {
		/* Set a receive callback for the ntpclient_upcb */
		udp_recv(ntpclient_upcb,(udp_recv_fn)ntpclient_Rcv_callback,NULL);
		Le_u_Ok = 1U;
		NTP_PRINTF_S("Start the ntp server application");
      }
      else
      {
        udp_remove(ntpclient_upcb);
		ntpclient_upcb = NULL;
        NTP_PRINTF_S("\r\nErrorLogging��can not bind udp pcb\r\n");
      }
   }
   else
   {
     NTP_PRINTF_S("\r\nErrorLogging��can not create udp pcb\r\n");
   } 
	 return Le_u_Ok;
}


/******************************************************
*��������ntpclient_udp_Rcv_callback

*��  �Σ�

*����ֵ��

*��  ����udp���ݽ��ջص�����,���ݶ˿ںŶԽ�����������Ӧ����

*��  ע��
******************************************************/
static void ntpclient_Rcv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{	
	//char Le_u_arg = *((char*)arg);
	if(p !=NULL)
	{
		//check ntp packet
		if((p->len==NTP_PKT_LEN) &&(upcb->remote_port == NTPCLIENT_REMOTE_PORT))
		{
			ntpclient.echoFlag = 2U;
			NTP_PRINTF_D("\n\r�յ�ntp������ %d ��Ӧ\n",ntpclient.SubsIndex);
			if(((((char*)p->payload)[0]& NTP_MODE_MASK) == NTP_MODE_SERVER)|| ((((char*)p->payload)[0] & NTP_MODE_MASK) == NTP_MODE_BROADCAST))
			{
				// extract time from packet
				//Receive_Timestamp = ((uint32_t)(((char*)p->payload)[RECEIVE_TS_OFFSET]) << 24) | ((uint32_t)(((char*)p->payload)[RECEIVE_TS_OFFSET+1]) <<16) |   \
				((uint32_t)(((char*)p->payload)[RECEIVE_TS_OFFSET+2]) << 8) | (uint32_t)(((char*)p->payload)[RECEIVE_TS_OFFSET+3]);
				SeTransmit_dw_Timestamp = ((uint32_t)(((char*)p->payload)[TRANSMIT_TS_OFFSET]) << 24) | ((uint32_t)(((char*)p->payload)[TRANSMIT_TS_OFFSET+1]) << 16) | \
										  ((uint32_t)(((char*)p->payload)[TRANSMIT_TS_OFFSET+2]) << 8) | (uint32_t)(((char*)p->payload)[TRANSMIT_TS_OFFSET+3]);
			}
			//minus the difference value of 1900 epoch and 1970 epoch
			SeTransmit_dw_Timestamp = SeTransmit_dw_Timestamp - DIFF_SEC_1900_1970;
			Sentpclient_u_TSUpdateFlg = 1U;
			//ntpclient.CalTimeFlag = 1U;
			//consider the time zone
			//Transmit_Timestamp += SEC_TIME_ZONE;
			NTP_PRINTF_D("\n\r ntp��������ȡ��ʱ���Ϊ %lu\n",SeTransmit_dw_Timestamp);
		}
		pbuf_free(p);
	}
}


/******************************************************
*��������ntpclient_Send

*��  �Σ�

*����ֵ��

*��  ����udp���ݷ��ͻص�����

*��  ע��
******************************************************/
void ntpclient_Send(char *data,uint16_t Lng)
{
	struct pbuf *p;
	
	p = pbuf_alloc(PBUF_RAW,Lng, PBUF_RAM);//���뷢�����ݻ�����/* allocate pbuf from pool*/	
	if(p != NULL)
	{
		pbuf_take(p,data, Lng); /* copy data to pbuf *///��������������p->payload 	
		udp_sendto(ntpclient_upcb, p, &ntpclient.remoteIP,NTPCLIENT_REMOTE_PORT);/* send udp data */
		NTP_PRINTF_S("\n\r����ntp����\n");
		pbuf_free(p); /* free pbuf */
	}
	else
	{		  
		NTP_PRINTF_S("\n\rcan not allocate send pbuf");
	}
}
/******************************************************
*��������ntpclient_Release

*��  �Σ�

*����ֵ��

*��  ����udp�����ͷţ��ͷ�upcb��

*��  ע��
******************************************************/
void ntpclient_Release(void)
{
	NTP_PRINTF_S("\n\rDisconnect to UDP echo server\n");
	 /* free the UDP connection, so we can accept new clients */
	udp_remove(ntpclient_upcb);
}

/******************************************************
*��������ntpclient_breakTimer

*��  �Σ�

*����ֵ��

*��  ����udp�Ͽ����Ӽ�ʱ��

*��  ע��
******************************************************/
void ntpclient_breakTimer(void)
{
	Sentpclient_dw_Timeout++;
}

/******************************************************
*��������Getntpclient_u_TSUpdate

*��  �Σ�

*����ֵ��

*��  ������ȡʱ�������״̬

*��  ע��
******************************************************/
uint8 Getntpclient_u_TSUpdate(void)
{
	return Sentpclient_u_TSUpdateFlg;
}

/******************************************************
*��������Getntpclient_dw_NetTimestamp

*��  �Σ�

*����ֵ��

*��  ������ȡʱ���

*��  ע��
******************************************************/
uint32 Getntpclient_dw_NetTimestamp(void)
{
	Sentpclient_u_TSUpdateFlg = 0U;
	return SeTransmit_dw_Timestamp;
}


/******************************************************
*��������Getntpclient_u_NtpCalTimeSt

*��  �Σ�

*����ֵ��

*��  ������ȡntpУ׼ʱ��״̬��־

*��  ע��
******************************************************/
uint8 Getntpclient_u_NtpCalTimeSt(void)
{
	return ntpclient.CalTimeFlag;
}
