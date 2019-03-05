/******************************************************
文件名：	ntpclient

描述：		
Data			Vasion			author
2018/2/1		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"ntpclient.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
//static uint8 ntpRcvBuf[50];
struct udp_pcb *ntpclient_upcb;//音频流数据收发UDP控制块
//static uint8 Linkflag;
static uint32 Sentpclient_dw_TskTimer;  //任务计数器
static uint32 Sentpclient_dw_Timeout;  //udp断开连接计数器
//static uint32_t Receive_Timestamp; //Time at the server when the request arrived from the client
static uint32_t SeTransmit_dw_Timestamp; //Time at the server when the response left for the client
static uint8 Sentpclient_u_TSUpdateFlg;//时间戳更新标志

static ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };// Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
static char ntpclient_sever[NTPCLIENT_SEVER_NUM] = 
{
	ntp_one,	  /*ntp服务器*/
	ntp_two,	  /*ntp服务器*/
	ntp_three,  /*ntp服务器*/
	ntp_four,	  /*ntp服务器*/
	ntp_five,	  /*ntp服务器*/
	ntp_six	  /*ntp服务器*/
};
static ntpclient_struct ntpclient;
/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void ntpclient_RenewRTC(void);

static void ntpclient_Rcv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
static void ntpclient_Release(void);
static void ntpclient_Send(char *data,uint16_t Lng);
static uint8 ntpclient_connect(struct ip_addr SrcIPaddr,struct ip_addr DestIPaddr,uint16 udp_LocalPort,uint16 udp_RemotePort);
/******************************************************
description： function code
******************************************************/

/**********************配置接口 BEGIN******************/
/******************************************************
*函数名：ntpclient_RenewRTC

*形  参：

*返回值：

*描  述：校准RTC时钟芯片时间

*备  注：
******************************************************/
static void ntpclient_RenewRTC(void)
{
	char Le_u_timeString[15] = {0};
	LocalRTC_timestamp2strBJtime(SeTransmit_dw_Timestamp,Le_u_timeString);//时间戳转换为北京时间（字符串格式）
	NTP_PRINTF_D("获取的网络时间：%s\n",Le_u_timeString);
	SetLocalRTC_datetime(Le_u_timeString,TIME_STAMP_BJT);//校准时钟芯片时间
	ntpclient.CalTimeFlag = 1U;
}
/**********************配置接口 END*********************/


/******************************************************
*函数名：Initntpclient_Pramater

*形  参：

*返回值：

*描  述：创建一个未连接的udp控制块

*备  注：
******************************************************/
void Initntpclient_Pramater(void)
{
	Sentpclient_dw_TskTimer = NTPCLIENT_TSK_PERIOD - (8000U/NTPCLIENT_SCHEDULING_CYCLE);
	Sentpclient_dw_Timeout = 0U;
	ntpclient.SubsIndex = 0U;
	ntpclient.TxBusyFlag = 0U;
	ntpclient.echoFlag = 0U;
	SeTransmit_dw_Timestamp = 0U; //Time at the server when the response left for the client
	Sentpclient_u_TSUpdateFlg = 0U;//时间戳更新标志
	ntpclient.CalTimeFlag = 0U;
}


/******************************************************
*函数名：Tskntpclient_MainFunction

*形  参：

*返回值：

*描  述：

*备  注：
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
		{//当前ntp服务器域名解析成功
			ntpclient.SrcIP.addr = IPaddress;
			GetdnsAnalysis_ipAddr(ntpclient_sever[Le_u_i],(struct ip_addr*)(&ntpclient.remoteIP.addr));//获取ntp服务器ip
			NTP_PRINTF_D("ntp服务器 %d ip地址：\n",ntpclient.SubsIndex);
			NTP_PRINTF_IP("%u.%u.%u.%u\n",(uint8_t)ntpclient.remoteIP.addr, \
							(uint8_t)(ntpclient.remoteIP.addr >> 8),	\
							(uint8_t)(ntpclient.remoteIP.addr >> 16),	\
							(uint8_t)(ntpclient.remoteIP.addr >> 24));
			if(1U == ntpclient_connect(ntpclient.SrcIP,ntpclient.remoteIP,NTPCLIENT_SRC_PORT,NTPCLIENT_REMOTE_PORT))
			{//创建连接，发送ntp协议报文
				memset(&(packet.li_vn_mode), 0, NTP_PKT_LEN);
				packet.li_vn_mode = NTP_LI_NO_WARNING|NTP_VERSION|NTP_MODE_CLIENT;
				ntpclient_Send((char*)(&(packet.li_vn_mode)),NTP_PKT_LEN);
				ntpclient.TxBusyFlag = 1U;
				ntpclient.echoFlag = 1U;
				Sentpclient_dw_Timeout = 0U;
			}
		}
		else
		{//当前ntp服务器域名未成功解析
			ntpclient.SubsIndex++;//访问下一台ntp服务器
		}
	}
	else
	{
		Sentpclient_dw_TskTimer++;
	}
	
	if(1U == ntpclient.echoFlag)
	{//等待回应
		Sentpclient_dw_Timeout++;
		if(Sentpclient_dw_Timeout >= NTP_CONNECT_TIMEOUT)
		{//响应超时
			Sentpclient_dw_Timeout = 0U;
			ntpclient.TxBusyFlag = 0U;
			ntpclient.echoFlag = 0U;
			NTP_PRINTF_D("\r\nErrorLogging：ntp服务器 %d 响应超时\r\n",ntpclient.SubsIndex);
			SetdnsAnalysis_ipUpdate(ntpclient_sever[ntpclient.SubsIndex]);//更新ip
			ntpclient.SubsIndex++;//当前ntp服务器响应超时，下次访问下一个ntp服务器
			ntpclient_Release();
		}
	}
	else
	{
		if(2U == ntpclient.echoFlag)
		{//收到回应
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
		Sentpclient_dw_TskTimer = 0U;//清任务计时器，避免频繁请求
	}
	
	/* 校准时钟芯片时间 */
	if(1U == Sentpclient_u_TSUpdateFlg)
	{
		NTP_PRINTF_S("\n\r网络同步更新时钟芯片时间");
		Sentpclient_u_TSUpdateFlg = 0U;
		ntpclient_RenewRTC();
	}
}

/******************************************************
*函数名：ntpclient_connect

*形  参：

*返回值：

*描  述：创建udp连接

*备  注：
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
      err = udp_bind(ntpclient_upcb, &SrcIPaddr, udp_LocalPort);//绑定本地IP和本地端口号
	  ip_addr_set(&ntpclient_upcb->remote_ip, &DestIPaddr);
	  ntpclient_upcb->remote_port = udp_RemotePort;
	  ntpclient_upcb->flags |= UDP_FLAGS_UNCONNECTED;//PCBk控制块连接标志有效
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
        NTP_PRINTF_S("\r\nErrorLogging：can not bind udp pcb\r\n");
      }
   }
   else
   {
     NTP_PRINTF_S("\r\nErrorLogging：can not create udp pcb\r\n");
   } 
	 return Le_u_Ok;
}


/******************************************************
*函数名：ntpclient_udp_Rcv_callback

*形  参：

*返回值：

*描  述：udp数据接收回调函数,根据端口号对接收数据做相应处理

*备  注：
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
			NTP_PRINTF_D("\n\r收到ntp服务器 %d 响应\n",ntpclient.SubsIndex);
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
			NTP_PRINTF_D("\n\r ntp服务器获取的时间戳为 %lu\n",SeTransmit_dw_Timestamp);
		}
		pbuf_free(p);
	}
}


/******************************************************
*函数名：ntpclient_Send

*形  参：

*返回值：

*描  述：udp数据发送回调函数

*备  注：
******************************************************/
void ntpclient_Send(char *data,uint16_t Lng)
{
	struct pbuf *p;
	
	p = pbuf_alloc(PBUF_RAW,Lng, PBUF_RAM);//申请发送数据缓冲区/* allocate pbuf from pool*/	
	if(p != NULL)
	{
		pbuf_take(p,data, Lng); /* copy data to pbuf *///待发送数据载入p->payload 	
		udp_sendto(ntpclient_upcb, p, &ntpclient.remoteIP,NTPCLIENT_REMOTE_PORT);/* send udp data */
		NTP_PRINTF_S("\n\r发送ntp报文\n");
		pbuf_free(p); /* free pbuf */
	}
	else
	{		  
		NTP_PRINTF_S("\n\rcan not allocate send pbuf");
	}
}
/******************************************************
*函数名：ntpclient_Release

*形  参：

*返回值：

*描  述：udp连接释放（释放upcb）

*备  注：
******************************************************/
void ntpclient_Release(void)
{
	NTP_PRINTF_S("\n\rDisconnect to UDP echo server\n");
	 /* free the UDP connection, so we can accept new clients */
	udp_remove(ntpclient_upcb);
}

/******************************************************
*函数名：ntpclient_breakTimer

*形  参：

*返回值：

*描  述：udp断开连接计时器

*备  注：
******************************************************/
void ntpclient_breakTimer(void)
{
	Sentpclient_dw_Timeout++;
}

/******************************************************
*函数名：Getntpclient_u_TSUpdate

*形  参：

*返回值：

*描  述：获取时间戳更新状态

*备  注：
******************************************************/
uint8 Getntpclient_u_TSUpdate(void)
{
	return Sentpclient_u_TSUpdateFlg;
}

/******************************************************
*函数名：Getntpclient_dw_NetTimestamp

*形  参：

*返回值：

*描  述：获取时间戳

*备  注：
******************************************************/
uint32 Getntpclient_dw_NetTimestamp(void)
{
	Sentpclient_u_TSUpdateFlg = 0U;
	return SeTransmit_dw_Timestamp;
}


/******************************************************
*函数名：Getntpclient_u_NtpCalTimeSt

*形  参：

*返回值：

*描  述：获取ntp校准时间状态标志

*备  注：
******************************************************/
uint8 Getntpclient_u_NtpCalTimeSt(void)
{
	return ntpclient.CalTimeFlag;
}
