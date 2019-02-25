/******************************************************
文件名：	ntpclient

描述：		

Data			  Vasion			author
2018/6/12		V1.0			  liujian
*******************************************************/
#ifndef		NTPCLIENT_H
#define		NTPCLIENT_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/
#define NTP_DEBUG
#if (defined(NTP_DEBUG) && defined(USART_DEBUGE_PRINTF))
#define NTP_PRINTF_S(x)   	 printf("%s\n",x)
#define NTP_PRINTF_D(x,d)   	 printf(x,d)
#define NTP_PRINTF_IP(x,d1,d2,d3,d4)    			printf(x,d1,d2,d3,d4)
#else
#define NTP_PRINTF_S(x)    
#define NTP_PRINTF_D(x,d)  
#define NTP_PRINTF_IP(x,d1,d2,d3,d4)
#endif


/**********宏常量定义*********/
#define NTPCLIENT_SCHEDULING_CYCLE	5U//调度周期
#define NTPCLIENT_TSK_PERIOD        (3600000U/NTPCLIENT_SCHEDULING_CYCLE)//大概1h

#define NTPCLIENT_SRC_PORT          0
#define NTPCLIENT_REMOTE_PORT       123//ntp服务器对应的udp端口号
#define NTPCLIENT_SEVER_NUM         6//ntp服务器数

#define NTP_PKT_LEN        		    48   //48bytes
#define NTP_LI_NO_WARNING          (0x00<<6) //no warning
#define NTP_VERSION                (4/* NTP Version 4*/<<3) 
#define NTP_MODE_CLIENT            0x03
#define NTP_MODE_SERVER            0x04
#define NTP_MODE_BROADCAST         0x05
#define NTP_MODE_MASK              0x07
#define RECEIVE_TS_OFFSET          32 // Receive_Timestamp offset
#define TRANSMIT_TS_OFFSET         40 //Transmit_Timestamp offset
/* number of seconds between 1900 and 1970 */
#define DIFF_SEC_1900_1970         (2208988800)
#define SEC_TIME_ZONE              (8*60*60) //Beijing,GMT+8

#define UDP_FLAGS_UNCONNECTED  0U
#define  NTP_FLGS_CONNECTED    0x04U
#define  NTP_CONNECT_TIMEOUT   (2500U/NTPCLIENT_SCHEDULING_CYCLE)


#ifdef USE_DHCP
#define NTP_DHCP_STATE    GetdhcpClient_u_DhcpSt()
#else
#define NTP_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif



/***********宏函数***********/
#define NTP_GET_PHY_LINK_STATUS() netif_is_link_up(&gnetif)

/*******************************************************
description： struct definitions
*******************************************************/
typedef struct
{
	char SubsIndex;//下标索引
	struct ip_addr SrcIP;//源IP
	struct ip_addr remoteIP;//目的IP
	char TxBusyFlag;//发送忙标志:0--空闲；1--忙
	char echoFlag;//响应标志：0--空闲；1--等待回应；2--收到回应
	unsigned char CalTimeFlag;//时间校准标志：0--ntp未校准时间；1--ntp校准时间
}ntpclient_struct;


typedef struct
{

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
	// li.   Two bits.   Leap indicator.
	// vn.   Three bits. Version number of the protocol.
	// mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.




/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/

/*******************************************************
description： function External declaration
*******************************************************/
extern void Initntpclient_Pramater(void);
extern void Tskntpclient_MainFunction(void);
extern void ntpclient_breakTimer(void);
extern uint8 Getntpclient_u_TSUpdate(void);
extern uint32 Getntpclient_dw_NetTimestamp(void);
extern uint8 Getntpclient_u_NtpCalTimeSt(void);
#endif
