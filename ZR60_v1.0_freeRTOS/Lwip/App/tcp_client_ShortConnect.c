/**
  ******************************************************************************
  * @file    tcp_echoclient.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   tcp echoclient application using LwIP RAW API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "memp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netconf.h"
#include "LAN8742A.h"
#include "tcp_client_ShortConnect.h"
#include "lwip/api.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static struct ip_addr ShortConnect_LocalIPaddr;
static struct ip_addr ShortConnect_DestIPaddr;
static char  ShortRecev_buf[1460];
static tcp_client_TxTempBufStruct  TxTempBuf;
static tcp_client_TxFlagStruct client_TxFlag;
static char    Setcp_u_shortCnntTxbuf[CLIENT_SHORTCNNT_LNG] = {0};

//struct tcp_pcb *echoclient_pcb = NULL;

static u8_t   Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_IDLE;//tcp����״̬:0--����
static u8_t   Setcp_client_u_TxBusyFlg = 0U;//����æ��־��0--���У�1--æ
//static u16_t  Setcp_client_w_TxTimer = 0U;//
//static u16_t  Setcp_client_w_ConntTimer = 0U;//�������Ӽ�ʱ��
#ifdef SHORTCNNT_HEART
static u16_t  Setcp_client_w_HeartTimer = 0U;//����������ʱ��
#endif
static u16_t  Setcp_client_w_DoorStTimer = 0U;//�ϱ�����״̬��ʱ��
static u16_t  Se_w_OpenDoorLogTimer = 0U;//�ϱ�������־��ʱ��
static u8_t  Setcp_client_u_TimeoutCnt = 0U;//�����ӳ�ʱ����

static u32_t  Se_dw_BListPullTimer;//��ȡ��������ʱ��
static tcp_client_OpenLogStruct Setcp_h_OpenLog;

char Vetcp_client_u_token[40];
tcp_client_BListPullStruct BListPull;
tcp_client_DeviceInitStruct DeviceInit;
tcp_client_doorStStruct  Se_h_doorSt;
/* ECHO protocol states */
enum echoclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};


/* structure to be passed as argument to the tcp callbacks */
struct echoclient
{
  enum echoclient_states state; /* connection status */
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};

//struct echoclient * echoclient_es = NULL;
static tcp_client_UpdateBListStruct  Se_h_UpdateBList;

/* Exported types ------------------------------------------------------------*/
/* polarSSL */
struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
#define SSL_SERVER_PORT 443 /* Server port number */
#define SSL_SERVER_NAME "118.190.125.215" /* Server address */
typedef struct
{
  uint32_t State;
}rng_state;
rng_state rngs;
ssl_context ssl;
ssl_session ssn;
//static uint8_t Se_u_sslInitFlag = 0U;

/* Private function prototypes -----------------------------------------------*/
static void tcp_client_httpPostRequest(char* postAddr,char* Le_u_in,u16_t * Le_u_len,char* Le_u_out,u8_t Le_u_ConnectionValue);
static void tcp_client_MacRecombine(char* Le_u_mac);
static uint8 tcp_ShortConnect_parseJson(char * pMsg);
static void tcp_shortConnect_StrToHex(char* Le_in, uint8_t* Le_out);
static void tcp_shortConnect_HexIPtoStr(struct ip_addr Le_dw_in,char* Le_u_out);
static uint8_t tcp_ShortConnect_SendMsg( ssl_context *ssl, const unsigned char *buf, size_t len );
static uint8_t tcp_ShortConnect_RcvMsg( ssl_context *ssl, unsigned char *buf, size_t len );
/* Private functions ---------------------------------------------------------*/
static void my_debug(void *ctx, int level, char *str);
static int RandVal(void* arg);
static uint8 Gettcp_client_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
static uint8_t Settcp_shortConnect_UpdataBLTime(void);
static void tcp_shortConnect_UpdataBList(void);
/******************************************************
*��������tcp_ShortConnect_parameter

*��  �Σ�

*����ֵ��

*��  ������ʼ��

*��  ע��
******************************************************/
void tcp_ShortConnect_parameter(void)
{
	//struct rtc_time Le_h_tm;
	uint8_t Letcp_ShortConnect_u_Xor = 0;
	//MemIf_ReadEE(EepromCfg_timestamp_page,BListPull.token,sizeof(BListPull));//��ȡ������ʱ�����ҳ�����Ϣ
	BListPull.timestamp = 0;
	BListPull.page = 1U;
	BListPull.Listtype = 0;
	BListPull.pageSize = SHORTCNNT_BLIST_PULLNUM;//Ĭ�������ȡ��������
	Se_dw_BListPullTimer = SHORTCNNT_PULLBLIST_PERIOD - (90000U/SHORTCNNT_SCHEDULING_CYCLE);
	//client_TxFlag.TskFlag = 0U;//�����־
	Se_h_doorSt.state = 0xff;
	
	client_TxFlag.InitFlag = 0U;//
	Letcp_ShortConnect_u_Xor = GetMemIf_u_CheckSum(EepromCfg_tokenInfo);//��ȡmac��ַ����У���
	if((Letcp_ShortConnect_u_Xor == MemIf_ReadEE(EepromCfg_tokenInfo,Vetcp_client_u_token,sizeof(Vetcp_client_u_token))) && \
			(STD_ACTIVE == GetMemIf_u_DtVild(EepromCfg_tokenInfo)))
	{//token��Ч
		SetMemIf_EEVild(EepromCfg_tokenInfo);//������Ч
		memcpy(BListPull.token,Vetcp_client_u_token,39U);
		memcpy(Setcp_h_OpenLog.token,Vetcp_client_u_token,39U);
		memcpy(Se_h_doorSt.token,Vetcp_client_u_token,39U);
		client_TxFlag.InitFlag = 1U;//����Ҫ�ٳ�ʼ��
		printf("\r\n token��Ϣ��Ч,�豸�������³�ʼ��\r\n");
	}
#ifdef SHORTCNNT_HEART
	client_TxFlag.HeartFlag = 0U;//
#endif
	client_TxFlag.DoorStFlag = 0U;//
	client_TxFlag.ReportFlag = 0U;//
	client_TxFlag.BListFlag = 0U;//
	//client_TxFlag.BusyFlag = 0U;//
	//client_TxFlag.PcktType = Pckt_Unknow;
	DeviceInit.addrtype = 0;//
#if 0	
	DeviceInit.mac[0] = 'e';//����ʹ��
	DeviceInit.mac[1] = '0';
	DeviceInit.mac[2] = '7';
	DeviceInit.mac[3] = '6';
	DeviceInit.mac[4] = 'd';
	DeviceInit.mac[5] = '0';
	DeviceInit.mac[6] = 'f';
	DeviceInit.mac[7] = '8';
	DeviceInit.mac[8] = '1';
	DeviceInit.mac[9] = '5';
	DeviceInit.mac[10] = 'a';
	DeviceInit.mac[11] = '8';
#endif
	/*��ȡ��ǰʱ�䣬����ȫ�����º�������ʱ���*/
	Se_h_UpdateBList.Timer = 0U;
	Se_h_UpdateBList.flag = 0U;	
	
	Se_h_UpdateBList.UpdataTimeSetflag = 0U;
	if(1U == Settcp_shortConnect_UpdataBLTime())
	{
		Se_h_UpdateBList.UpdataTimeSetflag = 1U;
	}
}

/******************************************************
*��������tcp_ShortConnect_MainFunction

*��  �Σ�

*����ֵ��

*��  ������Ϣ��¼������,5ms���ڵ���

*��  ע��
******************************************************/
void tcp_ShortConnect_MainFunction(void)
{
#ifdef ZR50
	return;
#endif
	//struct rtc_time Le_h_tm;
	uint16 Le_w_i;
	uint16 Le_w_lng;
	//uint8  Le_u_ret;
	char Le_u_TxData[500U] = {0};
	//char Recev_Tempbuf[512] = {0};
	
	if(0 == GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		Setcp_client_w_DoorStTimer = 0U;
		//client_TxFlag.TskFlag = 0U;//�����־
		//client_TxFlag.InitFlag = 0U;//
		client_TxFlag.tokenOverdueFlag = 0U;
#ifdef SHORTCNNT_HEART
		Setcp_client_w_HeartTimer = 0U;
		client_TxFlag.HeartFlag = 0U;//
#endif
		client_TxFlag.DoorStFlag = 0U;//
		client_TxFlag.ReportFlag = 0U;//
		client_TxFlag.BListFlag = 0U;//
		//client_TxFlag.BusyFlag = 0U;//
		Setcp_client_u_TxBusyFlg = 0U;
		//client_TxFlag.EchoFlag = 0U;//
		//client_TxFlag.PcktType = Pckt_Unknow;
		//Setcp_client_w_TxTimer = 0U;
		Se_w_OpenDoorLogTimer = 0U;
		//Setcp_client_w_ConntTimer = 0U;
		Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_IDLE;
		return;
	}
	
/**********************ȫ�����º�����**********************/
	if(0U == Se_h_UpdateBList.UpdataTimeSetflag)
	{
		if(1U == Settcp_shortConnect_UpdataBLTime())
		{
			Se_h_UpdateBList.UpdataTimeSetflag = 1U;/* ȫ�����º�������ʱ���������� */
		}
	}
	
	tcp_shortConnect_UpdataBList();/* ȫ�����º����� */
/**********************************************************/
	
#ifdef SHORTCNNT_HEART	
	Setcp_client_w_HeartTimer++;
	if(Setcp_client_w_HeartTimer >= CLIENT_HEART_PERIOD)//30s
	{
		Setcp_client_w_HeartTimer = 0U;
		client_TxFlag.HeartFlag = 1U;//
		//client_TxFlag.TskFlag = 1U;
	}
#endif
	Setcp_client_w_DoorStTimer++;
	if(Setcp_client_w_DoorStTimer > CLIENT_DOORST_PERIOD)
	{
		Setcp_client_w_DoorStTimer = 0U;
		if(Se_h_doorSt.state != Gettcp_client_u_DoorSt)
		{//����״̬�ı�ʱ���ϱ�����״̬
			Se_h_doorSt.state = Gettcp_client_u_DoorSt;
			client_TxFlag.DoorStFlag = 1U;//
		}	
		//client_TxFlag.TskFlag = 1U;
	}
	
	Se_dw_BListPullTimer++;
	if(Se_dw_BListPullTimer >= (SHORTCNNT_PULLBLIST_PERIOD))//0.5h
	{
		client_TxFlag.BListFlag = 1U;
		Se_dw_BListPullTimer = 0U;
		//client_TxFlag.TskFlag = 1U;
	}

	Se_w_OpenDoorLogTimer++;
	if(Se_w_OpenDoorLogTimer >= CLIENT_LOGRECORD_PERIOD)
	{
		Se_w_OpenDoorLogTimer = 0U;
		if(1U == Gettcp_client_u_LogAvild)
		{
			client_TxFlag.ReportFlag = 1U;//
		}
		//client_TxFlag.TskFlag = 1U;
	}
#ifdef SHORTCNNT_HTTPS	
	int ret;
	int server_fd =0;
#else
	err_t err;
	//sstruct netbuf *recvbuf;
	struct pbuf *q;
	uint32_t data_len = 0;
#endif
	/**********��������**********/
	if((DHCP_ADDRESS_ASSIGNED == CLIENT_SHORTCNNT_DHCP_STATE) && (GetShortCnnt_PerformCondition) \
		&& (CLIENT_SHORTCNNT_IDLE == Setcp_client_u_cnntSt))
	{	
		if((client_TxFlag.DoorStFlag == 1U) || (client_TxFlag.BListFlag == 1U) || \
				(client_TxFlag.ReportFlag == 1U) || (client_TxFlag.InitFlag == 0U))
		{
			//client_TxFlag.TskFlag = 0U;
			if(1U != GetdnsAnalysis_u_ipValidity(ShortConnect))
			{
				return;
			}
#ifdef SHORTCNNT_EXT_NET
			GetdnsAnalysis_ipAddr(ShortConnect,&ShortConnect_DestIPaddr);
#else
			IP4_ADDR(&ShortConnect_DestIPaddr, 10, 0, 0, 222);
#endif
#ifdef SHORTCNNT_HTTPS
			char Le_u_IPaddr[16U] = {0};
			tcp_shortConnect_HexIPtoStr(ShortConnect_DestIPaddr,Le_u_IPaddr);
			/* Start the connection */
			do
			{
				USART_PRINTF_S(( "\n\rSSL : Start the connection \n\r"));
				
				/* Bint the connection to SSL server port */
				ret = net_connect(&server_fd, Le_u_IPaddr, SSL_SERVER_PORT);
				if(ret != 0)
				{
					/* Connection to SSL server failed */
					USART_PRINTF_D("\r\nErrorLogging��failed \n\r ! net_connect returned %d\n\r", -ret);
					
					/* Wait 1s until next retry */
					vTaskDelay(5000);
				} 
			}while(ret!=0);
#else
			do
			{
				USART_PRINTF_S(( "\n\rStart the connection \n\r"));
				
				tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��NETCONN_TCP����
				err = netconn_connect(tcp_clientconn,&ShortConnect_DestIPaddr,8090);//���ӷ�����
				if(err != ERR_OK)  
				{
					USART_PRINTF_S("connection failed \r\n");
					netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
					/* Wait 1s until next retry */
					vTaskDelay(5000);
				}
			}while(err != ERR_OK);
			
#endif

			USART_PRINTF_S((" net_connect Ok \n\r"));
			Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_TXPCKT;//���ӽ����ɹ���׼����������
		}
	}

	/**********��������**********/
	if(CLIENT_SHORTCNNT_TXPCKT == Setcp_client_u_cnntSt)
	{
#ifdef SHORTCNNT_HTTPS
		/* Initialize the SSL context */
		//if(0U == Se_u_sslInitFlag)
		{//sslδ��ʼ��
			ret = ssl_init(&ssl);
			if(ret != 0)
			{
				/* SSL initialization failed */
				USART_PRINTF_D("\r\nErrorLogging��failed \n\r ! ssl_init returned %d\n\r", -ret);
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
			USART_PRINTF_S((" ssl_init ok\n\r"));	
			/* Set the current session as SSL client */
			ssl_set_endpoint(&ssl, SSL_IS_CLIENT);
			/* No certificate verification */
			ssl_set_authmode(&ssl, SSL_VERIFY_NONE);
			/* Set the random number generator callback function */
			ssl_set_rng(&ssl, RandVal, &rngs); 	
			/* Set the debug callback function */
			ssl_set_dbg(&ssl, my_debug, stdout);
			/* Set read and write callback functions */
			ssl_set_bio(&ssl, net_recv, &server_fd, net_send, &server_fd);
			/* Set the session resuming flag, timeout and session context */
			ssl_set_session(&ssl, 1, 1000, &ssn);
			/* The list of ciphersuites to be used in this session */
			ssl_set_ciphersuites(&ssl, ssl_default_ciphersuites);
			//Se_u_sslInitFlag = 1U;
		}
#else
		struct netbuf *recvbuf;		
#endif	
		char *position;
		uint16_t Le_w_temp;
		size_t len;
		if(client_TxFlag.InitFlag == 0U)//�豸δ��ʼ��
		{
			client_TxFlag.InitFlag = 2U;
			Setcp_client_u_TxBusyFlg = 1U;
			USART_PRINTF_S("�豸��ʼ����Ϣ����");
			Json_HexToStr(DeviceInit.doorID,sminfo1.door_id,16);
			memset(Setcp_u_shortCnntTxbuf,0,sizeof(Setcp_u_shortCnntTxbuf));//��0
			if(DeviceInit.addrRecombineFlg == 0U)
			{
				tcp_client_MacRecombine(&DeviceInit.Blemac[0]);
				//ʹ������macʱ���˴�Blemac���Ƶ�mac
				for(Le_w_i = 0U;Le_w_i < 18U;Le_w_i++)
				{
					DeviceInit.mac[Le_w_i] = DeviceInit.Blemac[Le_w_i];
				}
				//tcp_client_MacRecombine(&DeviceInit.mac[0]);
				DeviceInit.addrRecombineFlg = 1U;
			}
			Json_HexToJson(&DeviceInit,&Le_w_lng,JSON_DEVICE_INIT,Le_u_TxData);
			tcp_client_httpPostRequest("POST /device/initForBand HTTP/1.1\n",Le_u_TxData,&Le_w_lng, \
									   Setcp_u_shortCnntTxbuf,1U);
#ifdef SHORTCNNT_HTTPS
			ret = tcp_ShortConnect_SendMsg(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
			//ret = ssl_write(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
			if(0U == ret)
			{//����ʧ��
				USART_PRINTF_S("\r\nErrorLogging��'�豸��ʼ��'���ݷ���ʧ��\r\n");
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
			
			/* Read the HTTP response */
			USART_PRINTF_S("\n\r<= Read from server :");;
			ret = tcp_ShortConnect_RcvMsg(&ssl, ShortRecev_buf, len);
			if(0U == ret)
			{//��ȡ����ʧ��
				USART_PRINTF_S("\r\nErrorLogging��'�豸��ʼ��'ʱδ�յ���������Ӧ\r\n");
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
#else
			err = netconn_write(tcp_clientconn ,Setcp_u_shortCnntTxbuf,Le_w_lng,NETCONN_COPY); //����tcp_server_sentbuf�е�����
			if(err != ERR_OK)
			{
				USART_PRINTF_S("����ʧ��\r\n");
				SetAudioIO_PlayFile(AudioIO_DeInitFail,2U);
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
			
			if((err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
			{	
				memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));//���ݽ��ջ���������
				for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
				{
					//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
					//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
					if(q->len > (sizeof(ShortRecev_buf)-data_len)) 
					{
						memcpy(ShortRecev_buf+data_len,q->payload,(sizeof(ShortRecev_buf)-data_len));//��������
					}
					else 
					{
						memcpy(ShortRecev_buf+data_len,q->payload,q->len);
					}
					data_len += q->len;  	
					if(data_len > sizeof(ShortRecev_buf)) break; //����TCP�ͻ��˽�������,����	
				}
				data_len=0;  //������ɺ�data_lenҪ����
				netbuf_delete(recvbuf);
			}
			else if(err == ERR_CLSD)  //�ر�����
			{
				SetAudioIO_PlayFile(AudioIO_DeInitFail,2U);
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
			else
			{}
#endif
			//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
			//USART_PRINTF_S(ShortRecev_buf);
			//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
			//��http�����н�����������
			position = strchr(ShortRecev_buf, '\{');//�״γ���'\{'�ĵ�ַ
			if(position != NULL)
			{
				Le_w_temp = position - (char*)ShortRecev_buf;//'\{'λ�õ��±�
				USART_PRINTF_S("���յ���Json��ʽ�������£�");
				USART_PRINTF_S(&ShortRecev_buf[Le_w_temp]);
				
				(void)tcp_ShortConnect_parseJson(&ShortRecev_buf[Le_w_temp]);
			}
			Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
			goto shortCnnt_exit;
		}
		else if(client_TxFlag.InitFlag == 1U)//�豸��ʼ�����
		{//�豸�ѳ�ʼ��
#ifdef SHORTCNNT_HEART
			/*��������*/
			//if((client_TxFlag.HeartFlag == 1U) && (0U == client_TxFlag.BusyFlag))
			if(client_TxFlag.HeartFlag == 1U)
			{
				client_TxFlag.HeartFlag = 0U;
				Json_HexToJson(Setcp_Alias.Alias,&Le_w_lng,JSON_HEART_BEAT,Setcp_u_shortCnntTxbuf);
				tcp_ShortConnect_sendMsg(Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
				//client_TxFlag.PcktType = Pckt_Heart;
				//Setcp_client_w_TxTimer = 0U;
				//client_TxFlag.BusyFlag = 1U;//�÷���æ
				//client_TxFlag.EchoFlag = 1U;//�ȴ���Ӧ
			}
#endif	
		
			/*��������״̬*/
			if(1U == client_TxFlag.DoorStFlag)
			{
				USART_PRINTF_S("��������״̬");
				Setcp_client_u_TxBusyFlg = 1U;
				client_TxFlag.DoorStFlag = 0U;
				//�ϱ�����״̬
				memset(Setcp_u_shortCnntTxbuf,0,sizeof(Setcp_u_shortCnntTxbuf));//��0
				//Se_h_doorSt.state = Gettcp_client_u_DoorSt;
				Json_HexToJson(&Se_h_doorSt,&Le_w_lng,JSON_REPORT_DOORST,Le_u_TxData);
				tcp_client_httpPostRequest("POST /door/lockState HTTP/1.1\n",Le_u_TxData, \
										   &Le_w_lng,Setcp_u_shortCnntTxbuf,1U);
#ifdef SHORTCNNT_HTTPS
				ret = tcp_ShortConnect_SendMsg(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
				if(0U == ret)
				{//����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'����״̬'����ʧ��\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				/* Read the HTTP response */
				USART_PRINTF_S("\n\r<= Read from server :");;
				ret = tcp_ShortConnect_RcvMsg(&ssl, ShortRecev_buf, len);
				if(0U == ret)
				{//��ȡ����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'����״̬'ʱδ�յ���������Ӧ\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
#else
				err = netconn_write(tcp_clientconn ,Setcp_u_shortCnntTxbuf,Le_w_lng,NETCONN_COPY); //����tcp_server_sentbuf�е�����
				if(err != ERR_OK)
				{
					USART_PRINTF_S("����ʧ��\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				if((err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));//���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (sizeof(ShortRecev_buf)-data_len)) 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,(sizeof(ShortRecev_buf)-data_len));//��������
						}
						else 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,q->len);
						}
						data_len += q->len;  	
						if(data_len > sizeof(ShortRecev_buf)) break; //����TCP�ͻ��˽�������,����	
					}
					data_len=0;  //������ɺ�data_lenҪ����
					netbuf_delete(recvbuf);
				}
				else if(err == ERR_CLSD)  //�ر�����
				{
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				else
				{}
#endif
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//USART_PRINTF_S(ShortRecev_buf);
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//��http�����н�����������
				position = strchr(ShortRecev_buf, '\{');//�״γ���'\{'�ĵ�ַ
				if(position != NULL)
				{
					Le_w_temp = position - (char*)ShortRecev_buf;//'\{'λ�õ��±�
					USART_PRINTF_S("���յ���Json��ʽ�������£�");
					USART_PRINTF_S(&ShortRecev_buf[Le_w_temp]);
					
					(void)tcp_ShortConnect_parseJson(&ShortRecev_buf[Le_w_temp]);
				}
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
			
			/*������ʱ����������*/
			//if((TxTempBuf.dtAlidity == 1U) && (0U == client_TxFlag.BusyFlag) \
			  && (0U == TxTempBuf.txflag))//��ʱ������������Ч,����δ���͹�
			if(TxTempBuf.dtAlidity == 1U)//��ʱ������������Ч
			{
				USART_PRINTF_S("������ʱ����������");
				Setcp_client_u_TxBusyFlg = 1U;
				TxTempBuf.dtAlidity = 0U;
				USART_PRINTF_S(Setcp_u_shortCnntTxbuf);
#ifdef SHORTCNNT_HTTPS
				ret = tcp_ShortConnect_SendMsg(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
				if(0U == ret)
				{//����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'��ʱ���ݻ�����'����ʧ��\r\n");
					TxTempBuf.dtAlidity = 1U;
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				/* Read the HTTP response */
				USART_PRINTF_S("\n\r<= Read from server :");;
				ret = tcp_ShortConnect_RcvMsg(&ssl, ShortRecev_buf, len);
				if(0U == ret)
				{//��ȡ����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'��ʱ���ݻ�����'����ʱδ�յ���������Ӧ\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
#else
				err = netconn_write(tcp_clientconn ,Setcp_u_shortCnntTxbuf,Le_w_lng,NETCONN_COPY); //����tcp_server_sentbuf�е�����
				if(err != ERR_OK)
				{
					USART_PRINTF_S("����ʧ��\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				if((err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));//���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (sizeof(ShortRecev_buf)-data_len)) 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,(sizeof(ShortRecev_buf)-data_len));//��������
						}
						else 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,q->len);
						}
						data_len += q->len;  	
						if(data_len > sizeof(ShortRecev_buf)) break; //����TCP�ͻ��˽�������,����	
					}
					data_len=0;  //������ɺ�data_lenҪ����
					netbuf_delete(recvbuf);
				}
				else if(err == ERR_CLSD)  //�ر�����
				{
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				else
				{}
#endif			
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//USART_PRINTF_S(ShortRecev_buf);
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//��http�����н�����������
				position = strchr(ShortRecev_buf, '\{');//�״γ���'\{'�ĵ�ַ
				if(position != NULL)
				{
					Le_w_temp = position - (char*)ShortRecev_buf;//'\{'λ�õ��±�
					USART_PRINTF_S("���յ���Json��ʽ�������£�");
					USART_PRINTF_S(&ShortRecev_buf[Le_w_temp]);
					
					if(1U != tcp_ShortConnect_parseJson(&ShortRecev_buf[Le_w_temp]))
					{
						TxTempBuf.dtAlidity = 1U;
					}
				}
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}
	
			/*���Ϳ��ż�¼*/
			//if((1U == client_TxFlag.ReportFlag)&& (0U == client_TxFlag.BusyFlag) \
				&&(TxTempBuf.dtAlidity == 0U))
			if(1U == client_TxFlag.ReportFlag)
			{
				client_TxFlag.ReportFlag = 0U;
				Setcp_client_u_TxBusyFlg = 1U;
				if(1U == Gettcp_client_u_LogAvild)
				{
					USART_PRINTF_S("���Ϳ��ż�¼����");
					for(Le_w_i = 0U;Le_w_i < CLIENT_LOGRECORD_NUM;Le_w_i++)
					{
						if(1U == Gettcp_client_u_LogAvild)
						{
							Gettcp_client_doorLog(&Setcp_h_OpenLog.Log[Le_w_i]);//��ȡ������¼
						}
						else
						{
							break;
						}
					}	
					Setcp_h_OpenLog.LogNum = Le_w_i;
					Json_HexToJson(&Setcp_h_OpenLog,&Le_w_lng,JSON_REPORT_UNLOCKLOG,Le_u_TxData);
					memset(Setcp_u_shortCnntTxbuf,0,sizeof(Setcp_u_shortCnntTxbuf));//��0
					tcp_client_httpPostRequest("POST /door/openLog2 HTTP/1.1\n",Le_u_TxData, \
											   &Le_w_lng,Setcp_u_shortCnntTxbuf,1U);
					TxTempBuf.lng = Le_w_lng;
					TxTempBuf.dtAlidity = 1U;
					for(Le_w_i = 0U;Le_w_i < CLIENT_SHORTCNNT_LNG;Le_w_i++)
					{
						TxTempBuf.data[Le_w_i] = Setcp_u_shortCnntTxbuf[Le_w_i];
					}
					//client_TxFlag.PcktType = Pckt_OpenReport;	
					//tcp_ShortConnect_sendMsg(Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
#ifdef SHORTCNNT_HTTPS
					ret = tcp_ShortConnect_SendMsg(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
					if(0U == ret)
					{//����ʧ��
						USART_PRINTF_S("\r\nErrorLogging������'���ż�¼'����ʧ��\r\n");
						Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
						goto shortCnnt_exit;
					}
					
					/* Read the HTTP response */
					USART_PRINTF_S("\n\r<= Read from server :");;
					ret = tcp_ShortConnect_RcvMsg(&ssl, ShortRecev_buf, len);
					if(0U == ret)
					{//��ȡ����ʧ��
						USART_PRINTF_S("\r\nErrorLogging������'���ż�¼'����ʱδ�յ���������Ӧ\r\n");
						Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
						goto shortCnnt_exit;
					}
#else
					err = netconn_write(tcp_clientconn ,Setcp_u_shortCnntTxbuf,Le_w_lng,NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						USART_PRINTF_S("����ʧ��\r\n");
						Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
						goto shortCnnt_exit;
					}
					
					if((err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
					{	
						memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));//���ݽ��ջ���������
						for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
						{
							//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
							//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
							if(q->len > (sizeof(ShortRecev_buf)-data_len)) 
							{
								memcpy(ShortRecev_buf+data_len,q->payload,(sizeof(ShortRecev_buf)-data_len));//��������
							}
							else 
							{
								memcpy(ShortRecev_buf+data_len,q->payload,q->len);
							}
							data_len += q->len;  	
							if(data_len > sizeof(ShortRecev_buf)) break; //����TCP�ͻ��˽�������,����	
						}
						data_len=0;  //������ɺ�data_lenҪ����
						netbuf_delete(recvbuf);
					}
					else if(err == ERR_CLSD)  //�ر�����
					{
						Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
						goto shortCnnt_exit;
					}
					else
					{}
#endif					
					//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
					//USART_PRINTF_S(ShortRecev_buf);
					//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
					//��http�����н�����������
					position = strchr(ShortRecev_buf, '\{');//�״γ���'\{'�ĵ�ַ
					if(position != NULL)
					{
						Le_w_temp = position - (char*)ShortRecev_buf;//'\{'λ�õ��±�
						USART_PRINTF_S("���յ���Json��ʽ�������£�");
						USART_PRINTF_S(&ShortRecev_buf[Le_w_temp]);
						
						if(1U == tcp_ShortConnect_parseJson(&ShortRecev_buf[Le_w_temp]))
						{
							TxTempBuf.dtAlidity = 0U;
						}
					}
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				else
				{//�޿�������¼
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}	
			}
			
			/*��ȡ������*/
			//if((client_TxFlag.BListFlag == 1U) && (0U == client_TxFlag.BusyFlag) \
			  &&(TxTempBuf.dtAlidity == 0U))
			if(client_TxFlag.BListFlag == 1U)
			{
				USART_PRINTF_S("��ȡ������");
				client_TxFlag.BListFlag = 0U;
				Setcp_client_u_TxBusyFlg = 1U;
				memset(Setcp_u_shortCnntTxbuf,0,sizeof(Setcp_u_shortCnntTxbuf));//��0
				Json_HexToJson(&BListPull,&Le_w_lng,JSON_GET_BLIST,Le_u_TxData);//��ȡ��������װ
				tcp_client_httpPostRequest("POST /doorcard/getPassList HTTP/1.1\n",Le_u_TxData, \
										   &Le_w_lng,Setcp_u_shortCnntTxbuf,1U);
				TxTempBuf.lng = Le_w_lng;
				TxTempBuf.dtAlidity = 1U;
				for(Le_w_i = 0U;Le_w_i < CLIENT_SHORTCNNT_LNG;Le_w_i++)
				{
					TxTempBuf.data[Le_w_i] = Setcp_u_shortCnntTxbuf[Le_w_i];
				}
				//client_TxFlag.PcktType = Pckt_BList;
				//tcp_ShortConnect_sendMsg(Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
#ifdef SHORTCNNT_HTTPS
				ret = tcp_ShortConnect_SendMsg(&ssl,Setcp_u_shortCnntTxbuf,Le_w_lng);//���ݷ���
				if(0U == ret)
				{//����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'��ȡ������'��������ʧ��\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				/* Read the HTTP response */
				USART_PRINTF_S("\n\r<= Read from server :");;
				ret = tcp_ShortConnect_RcvMsg(&ssl, ShortRecev_buf, len);
				if(0U == ret)
				{//��ȡ����ʧ��
					USART_PRINTF_S("\r\nErrorLogging������'��ȡ������'��������ʱδ�յ���������Ӧ\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
	#else
				err = netconn_write(tcp_clientconn ,Setcp_u_shortCnntTxbuf,Le_w_lng,NETCONN_COPY); //����tcp_server_sentbuf�е�����
				if(err != ERR_OK)
				{
					USART_PRINTF_S("����ʧ��\r\n");
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				
				if((err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));//���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (sizeof(ShortRecev_buf)-data_len)) 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,(sizeof(ShortRecev_buf)-data_len));//��������
						}
						else 
						{
							memcpy(ShortRecev_buf+data_len,q->payload,q->len);
						}
						data_len += q->len;  	
						if(data_len > sizeof(ShortRecev_buf)) break; //����TCP�ͻ��˽�������,����	
					}
					data_len=0;  //������ɺ�data_lenҪ����
					netbuf_delete(recvbuf);
				}
				else if(err == ERR_CLSD)  //�ر�����
				{
					Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
					goto shortCnnt_exit;
				}
				else
				{}
#endif
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//USART_PRINTF_S(ShortRecev_buf);
				//USART_PRINTF_S("\n\r---------------------------------------------------------\n\r");
				//��http�����н�����������
				position = strchr(ShortRecev_buf, '\{');//�״γ���'\{'�ĵ�ַ
				if(position != NULL)
				{
					Le_w_temp = position - (char*)ShortRecev_buf;//'\{'λ�õ��±�
					USART_PRINTF_S("���յ���Json��ʽ�������£�");
					USART_PRINTF_S(&ShortRecev_buf[Le_w_temp]);
					
					if(1U == tcp_ShortConnect_parseJson(&ShortRecev_buf[Le_w_temp]))
					{
						TxTempBuf.dtAlidity = 0U;
					}
				}
				Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
				goto shortCnnt_exit;
			}	
		}
		else
		{//�豸δ�ɹ���ʼ��
			Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_DISCNNT;
			goto shortCnnt_exit;
		}
	}
	
shortCnnt_exit:
	if(Setcp_client_u_cnntSt == CLIENT_SHORTCNNT_DISCNNT)
	{
		USART_PRINTF_S("�Ͽ�����...");
#ifdef SHORTCNNT_HTTPS
		net_close(server_fd);
		ssl_free(&ssl);
#else
		netconn_close(tcp_clientconn);
		netconn_delete(tcp_clientconn);
#endif
		//memset(&ssl, 0, sizeof(ssl));
		Setcp_client_u_TxBusyFlg = 0U;
		//client_TxFlag.PcktType = Pckt_Unknow;
		Setcp_client_u_cnntSt = CLIENT_SHORTCNNT_IDLE;
	}
}

/*
	��������
*/
static uint8_t tcp_ShortConnect_SendMsg( ssl_context *ssl, const unsigned char *buf, size_t len )
{
	int ret;
	ret = ssl_write(ssl,buf,len);//���ݷ���
	while(ret <= 0)
	{
		if( ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE )
		{
			/* Write to SSL server failed */
			USART_PRINTF_D( "ErrorLogging��failed \n\r! ssl_write returned %d\n\r", -ret);
			return 0;
		}
	}
	return 1U;
}


/*
	��������
*/
static uint8_t tcp_ShortConnect_RcvMsg( ssl_context *ssl, unsigned char *buf, size_t len )
{
	int ret;
	/* Read decrypted application data */
	do
	{
		len = sizeof(ShortRecev_buf) - 1;
		memset(ShortRecev_buf, 0, sizeof(ShortRecev_buf));
		ret = ssl_read(ssl, buf, len);				
		if( ret == POLARSSL_ERR_NET_WANT_READ || ret == POLARSSL_ERR_NET_WANT_WRITE ) 
		{
			USART_PRINTF_S("SSL: POLARSSL_ERR_NET_WANT_READ/WRITE\n");
			continue;
		}

		if( ret == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY ) 
		{
			USART_PRINTF_S("SSL: POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY\n");
			return 0U;
		} 

		if( ret <= 0 )
		{
			USART_PRINTF_D( "ErrorLogging��failed\n  ! ssl_read returned %d\r\n", ret );
			return 0U;
		}
		break;
	}while(0);
	return 1U;
}


static void my_debug(void *ctx, int level, char *str)
{
	//printf("%s", str); 
}

/**
  * @brief  Returns a 32-bit random number.
  * @param  arg not used
  * @retval 32-bit random number
  */
static int RandVal(void* arg)
{
  uint32_t ret; 

  /* Wait until random number is ready */
  while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);
  
  /* Get the random number */       
  ret = RNG_GetRandomNumber();

  /* Return the random number */ 
  return(ret);
}



u8_t Gettcp_shortCnntTxSt(void)
{
	return Setcp_client_u_TxBusyFlg;
}

/**
  * @brief http��װ
  * @param 
  * @param
  * @retval None
**/
static void tcp_client_httpPostRequest(char* postAddr,char* Le_u_in,u16_t * Le_u_len,char* Le_u_out,u8_t Le_u_ConnectionValue)
{
	char str[5U] = {0};
	char Le_u_IPaddr[16U] = {0};
	sprintf(str, "%d", *Le_u_len);
	strcat(Le_u_out, postAddr);
	strcat(Le_u_out, "Content-Type: application/json;charset=UTF-8\n");
	strcat(Le_u_out, "Host: ");
	tcp_shortConnect_HexIPtoStr(ShortConnect_DestIPaddr,Le_u_IPaddr);
	strcat(Le_u_out, Le_u_IPaddr);
	strcat(Le_u_out, "\n");
	strcat(Le_u_out, "Content-Length: ");
	strcat(Le_u_out, str);
	strcat(Le_u_out, "\n");
	if(Le_u_ConnectionValue == 1U)
	{
		strcat(Le_u_out, "Connection: keep-alive\n");
	}
	else
	{
		strcat(Le_u_out, "Connection: close\n");
	}
	strcat(Le_u_out, "\n");//��Ҫ��һ��
	//Le_u_in ��ֵΪpost������
	strcat(Le_u_out, Le_u_in);
	strcat(Le_u_out, "\r\n\r\n");
	*Le_u_len = strlen(Le_u_out);
	USART_PRINTF_S(Le_u_out);
}

/*
	�����ӽ��ձ��Ľ���
*/
static uint8 tcp_ShortConnect_parseJson(char * pMsg)
{
	uint8_t ret = 0;
	cJSON * pJson = cJSON_Parse(pMsg);
	if(NULL == pJson)                                                                                         
	{
		// parse faild, return
		USART_PRINTF_S("\r\nErrorLogging���������ݽ�����json��ʽʧ��\r\n");
		return 0U;
	}
	// get string from json
	cJSON * pSub_err = cJSON_GetObjectItem(pJson, "error");
	if(NULL == pSub_err)
	{
		USART_PRINTF_S("\r\nErrorLogging����ȡ��Ա error ʧ��\r\n");
		cJSON_Delete(pJson);
		return 0U;
	}
	
	if(pSub_err->type == cJSON_String)
	{
		pSub_err->valueint = (int)(Json_DecNumber(&(pSub_err->valuestring[0])));
	}

	switch(pSub_err->valueint)
	{
		case 0:
		{
			USART_PRINTF_S("״̬����ȷ");
			ret = 1;
			//client_TxFlag.EchoFlag = 2U;
			cJSON * pSub_token = cJSON_GetObjectItem(pJson, "token");
			if(NULL != pSub_token)
			{//�յ���ʼ����Ӧ
				memcpy(BListPull.token,pSub_token->valuestring,39U);
				memcpy(Setcp_h_OpenLog.token,pSub_token->valuestring,39U);
				memcpy(Se_h_doorSt.token,pSub_token->valuestring,39U);
				memcpy(Vetcp_client_u_token,pSub_token->valuestring,39U);
				if(TxTempBuf.dtAlidity == 1U)
				{//��ʱ������������Ч��˵���˴���token���ں����³�ʼ���豸����Ӧ������»��������ݵ�tokenֵ
					char *position;
					u16_t Le_w_temp;
					u8_t Le_u_i;
					position = strchr(TxTempBuf.data, '\{');//�״γ���'\{'�ĵ�ַ
					Le_w_temp = position - TxTempBuf.data;//'\{'λ�õ��±�
					Le_w_temp = Le_w_temp + 10U;//tokenֵ����ʼλ��
					for(Le_u_i = 0;Le_u_i < 39U;Le_u_i++)
					{
						TxTempBuf.data[Le_w_temp + Le_u_i] = pSub_token->valuestring[Le_u_i];
					}
				}
				USART_PRINTF_S("\r\ntoken��Ϣд��flash\r\n");
				(void)MemIf_WriteEE(EepromCfg_tokenInfo,Vetcp_client_u_token,sizeof(Vetcp_client_u_token));//tokenֵд��flash
				client_TxFlag.InitFlag = 1U;//�豸��ʼ��ok
				SetAudioIO_PlayFile(AudioIO_DeInitFinish,2U);
			}
			else
			{		
				cJSON * pSub_list = cJSON_GetObjectItem(pJson, "list");
				if(NULL != pSub_list)
				{//��������Ӧ����
					cJSON * pSub_total = cJSON_GetObjectItem(pJson, "total");
					cJSON * pSub_timestamp = cJSON_GetObjectItem(pJson, "timeStamp");
					if(pSub_total->type == cJSON_String)
					{
						pSub_total->valueint = (int)(Json_DecNumber(&(pSub_total->valuestring[0])));
					}
					
					if(0 != pSub_total->valueint)
					{	
						if(BListPull.page == pSub_total->valueint)
						{
							BListPull.timestamp = pSub_timestamp->valuedouble;
							BListPull.page = 1U;
							BListPull.UpdataFlag = 1U;
							Se_dw_BListPullTimer = 0U;
						}
						else
						{
							BListPull.UpdataFlag = 0U;
							BListPull.page++;
							Se_dw_BListPullTimer = SHORTCNNT_PULLBLIST_PERIOD - (3000U/SHORTCNNT_SCHEDULING_CYCLE);
						}
						//USART_PRINTF_D("��ȡҳ�룺%d\n",BListPull.page);
						/*���緢����ʱ��У׼����ʱ��оƬ*/
						//timestamp_strBJtime((BListPull.timestamp/1000),timelist);//ʱ���ת��Ϊʱ�䣨�ַ�����ʽ��
						//timestamp_timeCalibration(timelist,TIME_STAMP_UNT);//У׼ʱ��оƬʱ��	
						/*��ȡ�������������*/
						unsigned int BList_size = cJSON_GetArraySize(pSub_list);
						//USART_PRINTF_D("����������������%d\n",BList_size);
						unsigned int BList_i = 0;
						unsigned char BList_j = 0;
						uint8_t BList_cardNum[CLIENT_SHORTCNNT_CARDNUM_LNG];
						char BList_cardNumString[8U];
						char BList_cardNumLen;
						cJSON *BList_item;
						for(BList_i=0; BList_i< BList_size; BList_i++)
						{
							BList_item = cJSON_GetArrayItem(pSub_list, BList_i);
							BList_cardNumLen = strlen(BList_item->valuestring);
							if(BList_cardNumLen != 8U)
							{
								if(BList_cardNumLen > 8U)
								{
									//USART_PRINTF_D("���ű�� %s���Ȳ���\n��",BList_item->valuestring);
									continue;
								}
								else
								{//���ų�������4�ֽڣ�ǰ�油0
									memset(BList_cardNumString,'0',8U);//��'0'
									for(BList_j = 0U;BList_j < BList_cardNumLen;BList_j++)
									{
										BList_cardNumString[8-BList_cardNumLen+BList_j] = BList_item->valuestring[BList_j];
									}
									tcp_shortConnect_StrToHex(BList_cardNumString,BList_cardNum);
								}
							}
							else
							{
								tcp_shortConnect_StrToHex(BList_item->valuestring,BList_cardNum);
							}
							WrBListCache_BListQueue(BList_cardNum);
							//USART_PRINTF_D("���ű�� %d ת��ΪHex��ʽ���ţ�",BList_i);
							//USART_PRINTF_CARD_NUM("%x%x%x%x\n",BList_cardNum[0U],BList_cardNum[1U],BList_cardNum[2U], \
												  BList_cardNum[3U]);
						}
					}
					else
					{
						Se_dw_BListPullTimer = 0U;
					}
#if 0					
					if((BListPull.timestamp != BListPull.Oldtimestamp) || \
							(BListPull.page != BListPull.Oldpage))
					{
						BListPull.Oldtimestamp = BListPull.timestamp;
						BListPull.Oldpage = BListPull.page;
						(void)MemIf_WriteEE(EepromCfg_timestamp_page,&BListPull,sizeof(BListPull));//������ʱ�����ҳ�����Ϣд�����ʧ�Դ洢��
					}
#endif
				}
			}
		}
		break;
		case 308:
		{//token ����
			USART_PRINTF_S("\r\nErrorLogging��token����\r\n");
			//client_TxFlag.tokenOverdueFlag = 1U;
			client_TxFlag.InitFlag = 0U;//�����豸���³�ʼ��
			ret = 0;
		}
		break;
		case 406:
		{//��idδ�ں�̨���豸
			USART_PRINTF_S("\r\nErrorLogging����idδ�ں�̨���豸\r\n");
			SetAudioIO_PlayFile(AudioIO_DeInitFailLostId,2U);
			ret = 0;
		}
		break;
		case 428:
		{//��id���ں�̨���豸
			USART_PRINTF_S("\r\nErrorLogging����id���ں�̨���豸\r\n");
			SetAudioIO_PlayFile(AudioIO_DeInitFailExistId,2U);
			ret = 0;
		}
		break;
		case 319:
		{//���ݲ���ȷ�����ݲ�����
			USART_PRINTF_S("\r\nErrorLogging�����ݲ���ȷ�����ݲ�����\r\n");
			TxTempBuf.dtAlidity = 0U;
			ret = 0;
		}
		break;
		default:
		{
			USART_PRINTF_D( "\r\nErrorLogging��״̬����󣬵�ǰ״̬��Ϊ %d\r\n",pSub_err->valueint);
			ret = 0;
		}
		break;
	}
	cJSON_Delete(pJson);
	return ret;
}

/*
	�������³�ʼ���豸
*/
void Settcp_client_DeviceInit(void)
{
	client_TxFlag.InitFlag = 0U;//
}


/*
	����������״̬
*/
uint8_t tcp_client_BListUpdataSt(void)
{
	return BListPull.UpdataFlag;
}

/******************************************************
*��������Gettcp_client_u_XOR

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
******************************************************/
static uint8 Gettcp_client_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng)
{
	uint16 Le_w_i;
	uint8 Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return Le_u_Xor;
}


/**
  * @brief mac��ַ����':'
  * @param 
  * @param 
  * @retval None
  */
static void tcp_client_MacRecombine(char* Le_u_mac)
{
	char Le_u_i;
	char La_u_TempMac[13U] = {0};
	for(Le_u_i=0U;Le_u_i < 13U;Le_u_i++)
	{
		La_u_TempMac[Le_u_i] = Le_u_mac[Le_u_i];
	}
	for(Le_u_i=0U;Le_u_i < 6U;Le_u_i++)
	{
		Le_u_mac[(3*(Le_u_i+1)-3)] = La_u_TempMac[(2*Le_u_i)];
		Le_u_mac[(3*(Le_u_i+1)-2)] = La_u_TempMac[(2*Le_u_i+1)];
		if(Le_u_i == 5U)
		{
			Le_u_mac[(3*(Le_u_i+1)-1)] = '\0';
		}
		else
		{
			Le_u_mac[(3*(Le_u_i+1)-1)] = ':';
		}
	}
}

/*
	�ַ���תHex��ʽ:AABBCCDDתΪ0xAA,0xBB,0xCC,0xDD
*/
static void tcp_shortConnect_StrToHex(char* Le_in, uint8_t* Le_out)
{
	uint8_t Le_u_i;
	for(Le_u_i = 0U;Le_u_i < 4U;Le_u_i++)
	{
		if((Le_in[2*Le_u_i] >= '0') && (Le_in[2*Le_u_i] <= '9'))
		{
			Le_out[Le_u_i] = ((Le_in[2*Le_u_i]-0x30) << 4U);
		}
		else
		{
			Le_out[Le_u_i] = ((Le_in[2*Le_u_i]-0x37) << 4U);
		}
		
		if((Le_in[2*Le_u_i+1] >= '0') && (Le_in[2*Le_u_i+1] <= '9'))
		{
			Le_out[Le_u_i] |= ((Le_in[2*Le_u_i + 1] -0x30) & 0x0f);
		}
		else
		{
			Le_out[Le_u_i] |= ((Le_in[2*Le_u_i + 1] -0x37) & 0x0f);
		}
	}
}


/*
//	����ȫ�����º�������ʱ��
*/
static uint8_t Settcp_shortConnect_UpdataBLTime(void)
{ 
	struct rtc_time Le_h_tm;
	uint8_t ret = 0U;
#ifdef  HYM8563
	(void)hym8563_read_datetime(&Le_h_tm);
#else
	Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
	//tm.tm_year, tm.tm_mon, tm.tm_mday, tm.wday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	if(Le_h_tm.tm_year >= 2018U)
	{
		Se_h_UpdateBList.hour = Le_h_tm.tm_sec%10;
		if(Se_h_UpdateBList.hour >= 5U)
		{
			Se_h_UpdateBList.hour = Se_h_UpdateBList.hour -5U;
		}
		Se_h_UpdateBList.min = Le_h_tm.tm_min;
		//Se_h_UpdateBList.sec = Le_h_tm.tm_sec;
		ret = 1U;
#ifdef UART_DEBUG
		printf("\nȫ�����º�����ʱ�䣺%d:%d:%d\n",Se_h_UpdateBList.hour,Se_h_UpdateBList.min,Se_h_UpdateBList.sec);
#endif
	}
	return ret;
}


/*
//	ȫ�����º�����
*/
static void tcp_shortConnect_UpdataBList(void)
{ 
	struct rtc_time Le_h_tm;
	Se_h_UpdateBList.Timer++;
	if(Se_h_UpdateBList.Timer >= (25000U/SHORTCNNT_SCHEDULING_CYCLE))
	{
		Se_h_UpdateBList.Timer = 0U;
#ifdef  HYM8563
		(void)hym8563_read_datetime(&Le_h_tm);/*��ȡ��ǰʱ��*/
#else
		Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
		if((Se_h_UpdateBList.hour == Le_h_tm.tm_hour) && (Le_h_tm.tm_min == Se_h_UpdateBList.min))
		{//ȫ�����º�������[ע��ʱ������ͬʱ������Уʱ����»��������ȫ�����º���������]
			if(0U == Se_h_UpdateBList.flag)
			{
				ClrBListMng_ListData();//��������б�
				Se_h_UpdateBList.flag = 1U;
				BListPull.timestamp = 0;
				BListPull.page = 1U;
				Se_dw_BListPullTimer = SHORTCNNT_PULLBLIST_PERIOD;
			}
		}
		else
		{
			Se_h_UpdateBList.flag = 0U;
		}
	}
}


/*
//hex ipת����x.x.x.x  
*/
static void tcp_shortConnect_HexIPtoStr(struct ip_addr Le_dw_in,char* Le_u_out)
{ 
	uint8_t ip_int_index[4U];
	uint8_t ip_temp_numbr = 24U;
	uint8_t Le_u_j;
	for(Le_u_j=0;Le_u_j<4;Le_u_j++)  
	{  
		ip_int_index[Le_u_j]=((Le_dw_in.addr>>ip_temp_numbr)&0xFF);  
		ip_temp_numbr-=8;  
	}  

	sprintf(Le_u_out,"%d.%d.%d.%d",ip_int_index[3],ip_int_index[2],ip_int_index[1],ip_int_index[0]); 
}
#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
