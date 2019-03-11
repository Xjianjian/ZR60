/**
  ******************************************************************************
  * @file    http_client_iap.c
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
#include "http_client_iap.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static struct ip_addr LocalIPaddr;
static struct ip_addr DestIPaddr;
//static char   Recev_buf[1460];
//static http_client_TxTempBufStruct   TxBuf;
static http_client_TxFlagStruct      TxFlag;
static char   Se_u_Txbuf[CLIENT_CNNT_LNG] = {0};

struct tcp_pcb *httpclient_pcb = NULL;

static u8_t   Sehttp_client_u_cnntSt = CLIENT_CNNT_IDLE;//tcp����״̬:0--����
static u8_t   Se_u_BusyFlg = 0U;//æ��־��0--���У�1--æ
//static u16_t  Se_w_TxTimer = 0U;//
static u16_t  Se_w_DownloadTimer = 0U;//
static u16_t  Se_w_ConntTimer = 0U;//�������Ӽ�ʱ��
static u8_t   Se_u_TimeoutCnt = 0U;//�������ӳ�ʱ����
static u32_t  Se_dw_byteCnt = 0U;//�����ֽ�������
/* ECHO protocol states */
enum httpclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};

/* structure to be passed as argument to the tcp callbacks */
struct httpclient
{
  enum httpclient_states state; /* connection status */
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};

struct httpclient * httpclient_es = NULL;

static http_client_UpdataInfoStruct UpdataInfo;



/* Private function prototypes -----------------------------------------------*/
static err_t http_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void http_client_connection_close(struct tcp_pcb *tpcb, struct httpclient * es);
static err_t http_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t http_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static void http_client_PostRequest(char* postAddr,char* Le_u_in,u16_t * Le_u_len,char* Le_u_out,u8_t Le_u_ConnectionValue);
static void http_client_MacRecombine(char* Le_u_mac);
static void http_ConnectErr(void *arg,err_t err);
#if 0
static uint8 http_Connect_parseJson(char * pMsg);
static void http_Connect_StrToHex(char* Le_in, uint8_t* Le_out);
static void http_Connect_HexIPtoStr(struct ip_addr Le_dw_in,char* Le_u_out);
#endif
static void http_EraseFlash(uint32_t FLASH_Sector);
static void http_wrFlash(uint32_t FLASH_addr,uint8_t* Le_u_Dt,uint16_t Le_w_lng);
static void http_rdFlash(uint32_t FLASH_addr,uint8_t* Le_u_Dt,uint16_t Le_w_lng);
static uint32 Gethttpclient_dw_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
/* Private functions ---------------------------------------------------------*/


/******************************************************
*��������http_client_iap_parameter

*��  �Σ�

*����ֵ��

*��  ������ʼ��

*��  ע��
******************************************************/
void http_client_iap_parameter(void)
{
	TxFlag.TskFlag = 0U;//�����־
	TxFlag.downloadFlag = DOWNLOAD_IDLE;//
	http_rdFlash(HTTP_CLIENT_UPLOAD_INFO_SEC,(uint8_t*)(&UpdataInfo.Xor),sizeof(UpdataInfo));
	//printf("UpdataInfo.IapFlag = %x\n",UpdataInfo.IapFlag);
	if(1U == UpdataInfo.IapFlag)
	{//���ر�־��Ч��������ش洢����
		printf("iap��־��Ч��������ش洢����\n");
		UpdataInfo.IapFlag = 0U;
		TxFlag.TskFlag = 1U;//�����־
		http_EraseFlash(FLASH_Sector_2);
		http_EraseFlash(FLASH_Sector_9);
		http_EraseFlash(FLASH_Sector_10);
		http_EraseFlash(FLASH_Sector_11);
	}
}

/******************************************************
*��������http_client_iap_MainFunction

*��  �Σ�

*����ֵ��

*��  ������Ϣ��¼������,5ms���ڵ���

*��  ע��
******************************************************/
void http_client_iap_MainFunction(void)
{
	//uint16 Le_w_i;
	uint16 Le_w_lng;
	if(0 == GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		TxFlag.TskFlag = 0U;//�����־
		TxFlag.downloadFlag = DOWNLOAD_IDLE;//
		Se_u_BusyFlg = 0U;
		Se_w_ConntTimer = 0U;
		Se_w_DownloadTimer = 0U;
		http_client_iap_disconnect();
		Sehttp_client_u_cnntSt = CLIENT_CNNT_IDLE;
		return;
	}
	
	if(GetZR60Ctrl_u_MastCtrlIapSt() == 1U)//�̼�������־��Ч
	{
		ClrZR60Ctrl_MastCtrlIapSt();
		SetAudioIO_PlayFile(AudioIO_SoftUpdata,2U);//
		//TxFlag.TskFlag = 1U;
		TxFlag.downloadFlag = DOWNLOAD_IDLE;
		UpdataInfo.IapFlag = 1U;
		http_wrFlash(HTTP_CLIENT_UPLOAD_INFO_SEC,(uint8_t*)(&UpdataInfo.Xor),sizeof(UpdataInfo));
	}
	
	/**********��������**********/
	if((DHCP_ADDRESS_ASSIGNED == CLIENT_CNNT_DHCP_STATE) && \
			(CLIENT_CNNT_IDLE == Sehttp_client_u_cnntSt))
	{	
		if(TxFlag.TskFlag == 1U)
		{
			if(1U != GetdnsAnalysis_u_ipValidity(http_download))
			{
				return;
			}
			GetdnsAnalysis_ipAddr(http_download,&DestIPaddr);
			if(ERR_OK == http_client_iap_connect())
			{
				SHORTCONN_PRINTF_S("������������...");
				TxFlag.TskFlag = 0U;
				Se_u_BusyFlg = 1U;
				Se_w_ConntTimer = 0U;
				Sehttp_client_u_cnntSt = CLIENT_CNNT_WAIT;
			}
		}
	}
	if(CLIENT_CNNT_WAIT == Sehttp_client_u_cnntSt)
	{
		Se_w_ConntTimer++;
		if(Se_w_ConntTimer <= (600/(IAP_SCHEDULING_CYCLE/5U)))
		{
			if((httpclient_es->state == ES_CONNECTED) &&(ESTABLISHED == httpclient_pcb->state))
			{//���ӽ����ɹ�
				SHORTCONN_PRINTF_S("�������ӽ����ɹ�!\n");
				Se_w_ConntTimer = 0U;
				Sehttp_client_u_cnntSt = CLIENT_CNNT_LOADPCKT;//
			}
		}
		else
		{//���ӽ���ʧ��
			SHORTCONN_PRINTF_S("\r\nErrorLogging�������������ӳ�ʱ\r\n");
			Se_u_TimeoutCnt++;
			if(Se_u_TimeoutCnt >= 3U)
			{
				Se_u_TimeoutCnt = 0U;
				SetdnsAnalysis_ipUpdate(http_download);//����ip
			}
			Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;
		}	
	}
	
	/**********��������**********/
	if(CLIENT_CNNT_LOADPCKT == Sehttp_client_u_cnntSt)
	{
		if(TxFlag.downloadFlag == DOWNLOAD_IDLE)
		{
			TxFlag.downloadFlag = DOWNLOAD_UNDERWAY;
			TxFlag.firstFrameFlag = 0U;
			SHORTCONN_PRINTF_S("������������");
			memset(Se_u_Txbuf,0,sizeof(Se_u_Txbuf));//��0
			Le_w_lng = 0U;
			Se_dw_byteCnt = 0U;
			Se_w_DownloadTimer = 0U;//�����س�ʱ��ʱ��
			http_client_PostRequest("GET /ZR60.bin HTTP/1.1\n",NULL,&Le_w_lng,Se_u_Txbuf,1U);
			//http_client_PostRequest("GET /ZR60.bin\n",NULL,&Le_w_lng,Se_u_Txbuf,1U);
			http_client_iap_sendMsg(Se_u_Txbuf,Le_w_lng);//���ݷ���	
		}
		
		Se_w_DownloadTimer++;
		if(Se_w_DownloadTimer >= (15000U/IAP_SCHEDULING_CYCLE))//���س�ʱ
		{
			SHORTCONN_PRINTF_S("\r\nErrorLogging�����س�ʱ\r\n");	
			SetAudioIO_PlayFile(AudioIO_SoftUpdataFail,2U);//
			Se_w_DownloadTimer = 0U;
			Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����
		}
		else
		{
			if(TxFlag.downloadFlag == DOWNLOAD_FINISH)//�������
			{
				SetAudioIO_PlayFile(AudioIO_SoftUpdataFinish,2U);//
				Se_w_DownloadTimer = 0U;
				Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����
			}
			else if(TxFlag.downloadFlag == DOWNLOAD_FAIL)//����ʧ��
			{
				SetAudioIO_PlayFile(AudioIO_SoftUpdataFail,2U);//
				Se_w_DownloadTimer = 0U;
				Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����
			}
			else
			{}
		}
	}
	
	/**********�Ͽ�����**********/
	if(CLIENT_CNNT_DISCNNT == Sehttp_client_u_cnntSt)
	{
		http_client_iap_disconnect();
		SHORTCONN_PRINTF_S("�Ͽ�����...");
		Se_u_BusyFlg = 0U;
		Se_w_DownloadTimer = 0U;
		Sehttp_client_u_cnntSt = CLIENT_CNNT_IDLE;
	}
	
	/***********���������������������ɣ���λ����*********/
	if((TxFlag.downloadFlag == DOWNLOAD_FINISH) || (1U == UpdataInfo.IapFlag))
	{	
		if((CLIENT_CNNT_IDLE == Sehttp_client_u_cnntSt) && (0U == Gettcp_shortCnntTxSt()) && (GetAudioIO_u_PlaySt() == 0U))
		{
			SHORTCONN_PRINTF_S("���������������������ɣ���λ����");
			NVIC_SystemReset();//�����λ�������ʼ�����̣��ڳ�ʼ����ִ�в���flash����
		}
	}
}


/**
* @brief  Connects to the TCP echo server
* @param  None
* @retval None
*/
char http_client_iap_connect(void)
{
	s8_t ret;
	/* create new tcp pcb */
	httpclient_pcb = tcp_new();
	if (httpclient_pcb != NULL)
	{
		LocalIPaddr.addr = IPaddress;
		ret = tcp_bind(httpclient_pcb, &LocalIPaddr, 0); 
		SHORTCONN_PRINTF_D("tcp_bind ret is: %d\n",ret);
		/* connect to destination address/port */
		ret = tcp_connect(httpclient_pcb,&DestIPaddr,CLIENT_CNNT_REMOTEPORT,http_client_connected);
		SHORTCONN_PRINTF_D("tcp_connect ret is: %d\n",ret);

		if(ERR_OK != ret)
		{
			memp_free(MEMP_TCP_PCB, httpclient_pcb);
			httpclient_pcb = NULL;
		}
		else
		{
			tcp_err(httpclient_pcb,http_ConnectErr); 
		}
	}
	else
	{
		ret = ERR_MEM;
		/* deallocate the pcb */
		SHORTCONN_PRINTF_S("\r\nErrorLogging����������tcp���ƿ����ʧ��\r\n");
	}
	return ret;
}

/*
	����ص�����
*/
static void http_ConnectErr(void *arg,err_t err)
{  
	SHORTCONN_PRINTF_S("\r\nErrorLogging����������err����\r\n");
    switch(err)  
    {   
        case ERR_MEM:                                            /* Out of memory error.     */  
            SHORTCONN_PRINTF_S("\r\n ERR_MEM   \r\n");  
        break;    
        case ERR_BUF:                                            /* Buffer error.            */  
            SHORTCONN_PRINTF_S("\r\n ERR_BUF   \r\n");  
        break;  
        case  ERR_TIMEOUT:                                       /* Timeout.                 */  
            SHORTCONN_PRINTF_S("\r\n ERR_TIMEOUT   \r\n");  
        break;  
        case ERR_RTE:                                            /* Routing problem.         */        
			SHORTCONN_PRINTF_S("\r\n ERR_RTE   \r\n");  
        break;  
		case ERR_ISCONN:                                          /* Already connected.       */  
			SHORTCONN_PRINTF_S("\r\n ERR_ISCONN   \r\n");  
        break;  
        case ERR_ABRT:                                           /* Connection aborted.      */  
            SHORTCONN_PRINTF_S("\r\n ERR_ABRT   \r\n");  
			httpclient_pcb = NULL;
        break;  
        case ERR_RST:                                            /* Connection reset.        */       
            SHORTCONN_PRINTF_S("\r\n ERR_RST   \r\n");  
			httpclient_pcb = NULL;
        break;  
        case ERR_CONN:                                           /* Not connected.           */  
			USART_PRINTF_S("\r\n ERR_CONN   \r\n");  
            break;  
        case ERR_CLSD:                                           /* Connection closed.       */  
            USART_PRINTF_S("\r\n ERR_CLSD   \r\n");  
			httpclient_pcb = NULL;
        break;  
        case ERR_VAL:                                            /* Illegal value.           */  
			USART_PRINTF_S("\r\n ERR_VAL   \r\n");  
			return;  
        case ERR_ARG:                                            /* Illegal argument.        */  
            USART_PRINTF_S("\r\n ERR_ARG   \r\n");  
			return;  
        case ERR_USE:                                            /* Address in use.          */  
			USART_PRINTF_S("\r\n ERR_USE   \r\n");  
			return;   
        case ERR_IF:                                             /* Low-level netif error    */  
            USART_PRINTF_S("\r\n ERR_IF   \r\n");  
        break;  
        case ERR_INPROGRESS:                                     /* Operation in progress    */  
            USART_PRINTF_S("\r\n ERR_INPROGRESS   \r\n");  
        break;  
		default:
		break;
    }  
}  

u8_t Gethttp_CnntTxSt(void)
{
	return Se_u_BusyFlg;
}

/**
* @brief  Disconnects to the TCP echo server
* @param  None
* @retval None
*/
void http_client_iap_disconnect(void)
{
	/* close connection */
	http_client_connection_close(httpclient_pcb,httpclient_es);
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
static err_t http_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	if(err == ERR_OK)   
	{
		/* allocate structure es to maintain tcp connection informations */
		httpclient_es = (struct httpclient *)mem_malloc(sizeof(struct httpclient));
		if(httpclient_es != NULL)
		{
			httpclient_es->state = ES_CONNECTED;
			httpclient_es->pcb = NULL;
			httpclient_es->p_tx = NULL;
			/* pass newly allocated es structure as argument to tpcb */
			tcp_arg(tpcb, httpclient_es);

			/* initialize LwIP tcp_recv callback function */ 
			tcp_recv(tpcb, http_client_recv);

			/* initialize LwIP tcp_sent callback function */
			//tcp_sent(tpcb, tcp_echoclient_sent);

			/* initialize LwIP tcp_poll callback function */
			tcp_poll(tpcb, http_client_poll, 1);

			return ERR_OK;
		}
		else
		{
			USART_PRINTF_S("\r\nErrorLogging����������structure es����ʧ��\r\n");
			/* close connection */
			http_client_connection_close(tpcb, httpclient_es);
			/* return memory allocation error */
			return ERR_MEM;  
		}
	}
	else
	{
		USART_PRINTF_D("\r\nErrorLogging�������������ӻص����� err: %d\r\n",err);
		/* close connection */
		http_client_connection_close(tpcb, httpclient_es);
	}
	return err;
}
    
/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback 
  * @param tpcb: tcp connection control block 
  * @param err: receive error code 
  * @retval err_t: retuned error  
  */
static err_t http_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
	struct httpclient *es;
	err_t ret_err;
	u16_t Le_w_i;
	char *position;
	char *endposition;
//	u16_t Le_w_temp;
	u8_t Le_u_CnntLngVal[7U] = {0};
	LWIP_ASSERT("arg != NULL",arg != NULL);

	USART_PRINTF_S("\r\n => http_client_recv\r\n");
	es = (struct httpclient *)arg;

	/* if we receive an empty tcp frame from server => close connection */
	if (p == NULL)
	{
		USART_PRINTF_S("\r\nErrorLogging����������Զ�˹ر�����\r\n");
		ret_err = ERR_OK;
	}   
	/* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
	else if(err != ERR_OK)
	{
		USART_PRINTF_S("\r\nErrorLogging���������ӽ��պ�����err!=ERR_OK\r\n");
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);
		/* free received pbuf*/
		pbuf_free(p);

		ret_err = err;
	}
	else if(es->state == ES_CONNECTED)
	{
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);// ������ã�����������ʹ���Ϊ0�������ݲ��ܷ��� 
		if(CLIENT_CNNT_REMOTEPORT == tpcb->remote_port)
		{//��http�����н�����������
			//USART_PRINTF_S((char*)p->payload);
			if(p->tot_len < 1460U)
			{
				if((TxFlag.firstFrameFlag == 0U) && (NULL != strstr((char*)p->payload,"HTTP/1.1")))
				{//��һ֡����
					if(NULL != strstr((char*)p->payload,"HTTP/1.1 200 OK"))
					{//�ޱ���ʱ������Content-Length��Content-Length�ֶε�ֵ�������ص������ܳ����ֽ���
						position = strstr((char*)p->payload,"Content-Length: ") + 16U;//Content-Length�ֶ���ֵ����ʼλ��
						endposition = strchr(&(((char*)p->payload)[position - (char*)p->payload]), '\r\n');//�״γ���'\r\n'�ĵ�ַ
						if((endposition - position) <= sizeof(Le_u_CnntLngVal))
						{
							for(Le_w_i = 0;Le_w_i < (endposition - position);Le_w_i++)
							{
								Le_u_CnntLngVal[Le_w_i] = ((char*)p->payload)[(position - (char*)p->payload) + Le_w_i];
							}
							Se_dw_byteCnt = 0U;
							UpdataInfo.Lng = atoi(Le_u_CnntLngVal);/*���ݳ���*/
							if(UpdataInfo.Lng > CLIENT_DOWNLOAD_DATA_LNG)
							{//���ݳ��ȳ�����������С
								USART_PRINTF_S("\r\nErrorLogging�����ݳ��ȳ����������洢�ռ�����\r\n");
								TxFlag.downloadFlag = DOWNLOAD_FAIL;
								//Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����	
							}
						}
						else
						{
							USART_PRINTF_S("\r\nErrorLogging�����ݳ�������\r\n");
							TxFlag.downloadFlag = DOWNLOAD_FAIL;
							//Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����
						}
						TxFlag.firstFrameFlag = 1U;
						USART_PRINTF_D("UpdataInfo.Lng = %d\n",UpdataInfo.Lng);
					}
					else
					{
						USART_PRINTF_S("\r\nErrorLogging��http������Ӧ�д���\r\n");
						TxFlag.downloadFlag = DOWNLOAD_FAIL;
						//Sehttp_client_u_cnntSt = CLIENT_CNNT_DISCNNT;//�Ͽ�����	
					}
				}
				else
				{//�û�app����
					if(TxFlag.downloadFlag == DOWNLOAD_UNDERWAY)
					{
						http_wrFlash((HTTP_CLIENT_USERCODE_SEC1+Se_dw_byteCnt),(uint8_t*)p->payload,p->tot_len);
						Se_dw_byteCnt += p->tot_len;
						if(Se_dw_byteCnt >= UpdataInfo.Lng)
						{
							if(Se_dw_byteCnt == UpdataInfo.Lng)
							{//�������һ֡���ݣ��������
								//UpdataInfo.Lng = UpdataInfo.Lng - 4;//��Ч���ݳ��� = ���յ������ܳ� - 4�ֽ�У��
								UpdataInfo.DtFirstAddr = HTTP_CLIENT_USERCODE_SEC1;/*���ݶ��׵�ַ*/
								UpdataInfo.DtendAddr =  HTTP_CLIENT_USERCODE_SEC1 + Se_dw_byteCnt -1;/*���ݶ�ĩ��ַ*/
								UpdataInfo.CheckSum = 0U;/*����������У���*/
								UpdataInfo.IapFlag = 0U;/*����������־*/
								UpdataInfo.Xor = Gethttpclient_dw_XOR((uint8*)(&UpdataInfo.DtFirstAddr),(sizeof(UpdataInfo)-4));
								http_wrFlash(HTTP_CLIENT_UPLOAD_INFO_SEC,(uint8_t*)(&UpdataInfo.Xor),sizeof(UpdataInfo));//������Ϣд��flash
								TxFlag.downloadFlag = DOWNLOAD_FINISH;
								USART_PRINTF_S("�������");
							}
							else
							{//�����ֽ�������
								USART_PRINTF_S("\r\nErrorLogging�����յ������ֽ�������\r\n");	
								TxFlag.downloadFlag = DOWNLOAD_FAIL;
							}
						}
					}
				}
			}
			else
			{
				/*�����������*/
				USART_PRINTF_S("\r\nErrorLogging����������������ݳ������\r\n");
				TxFlag.downloadFlag = DOWNLOAD_FAIL;
			}
		}
		/* free received pbuf*/
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	/* data received when connection already closed */
	else
	{
		USART_PRINTF_S("\r\nErrorLogging���������ӽ��յ�δ֪����\r\n");
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);
		/* free pbuf and do nothing */
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	
	USART_PRINTF_S("\r\n <= http_client_recv\r\n");
	return ret_err;
}


/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t http_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct echoclient *es;
	ret_err = ERR_OK;
	es = (struct echoclient*)arg;
	if (es == NULL)
	{
		USART_PRINTF_S("\r\nErrorLogging��download connection tcp_poll �ص����� ret_err = ERR_ABRT\r\n");
		//if(tpcb != NULL)
		{
			tcp_abort(tpcb);
		}
		ret_err = ERR_ABRT;
	}
	return ret_err;
}


/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on echoclient structure
  * @retval None
  */
static void http_client_connection_close(struct tcp_pcb *tpcb, struct httpclient * es )
{
	if (es != NULL)
	{
		mem_free(es);
		httpclient_es = NULL;
		USART_PRINTF_S("free httpclient_es\n");
	}
	
	if(tpcb != NULL)
	{
		/* remove callbacks */
		tcp_arg(tpcb,NULL);  
		tcp_recv(tpcb,NULL);
		tcp_err(tpcb,NULL);
		//tcp_sent(tpcb, NULL);
		tcp_poll(tpcb, NULL,0);
		/* close tcp connection */
		tcp_close(tpcb);
	}
	else
	{
		//USART_PRINTF_S("httpclient_pcb == NULL");
	}
}

/*
	���ͱ���
*/
void http_client_iap_sendMsg(char* data,u16_t len)
{
	tcp_write(httpclient_pcb,(char*)data ,len, 1);
	//tcp_output(httpclient_pcb);
}

/**
  * @brief http��װ
  * @param 
  * @param
  * @retval None
  */
static void http_client_PostRequest(char* postAddr,char* Le_u_in,u16_t * Le_u_len,char* Le_u_out,u8_t Le_u_ConnectionValue)
{
	strcat(Le_u_out, postAddr);
	strcat(Le_u_out, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n");
	//strcat(Le_u_out, "Accept-Encoding: gzip, deflate\r\n");
	//strcat(Le_u_out, "Accept-Language: zh-CN,zh;q=0.9\r\n");
	if(Le_u_ConnectionValue == 1U)
	{
		strcat(Le_u_out, "Connection: keep-alive\r\n");
	}
	else
	{
		strcat(Le_u_out, "Connection: close\r\n");
	}
	strcat(Le_u_out, "Host: mobile.zzwtec.com\r\n");
	strcat(Le_u_out, "\r\n\r\n");
	*Le_u_len = strlen(Le_u_out);
	USART_PRINTF_S(Le_u_out);
}


/*
//����flash����
*/
static void http_EraseFlash(uint32_t FLASH_Sector)
{
	__disable_irq() ; //�ر����ж�
	//IWDG_Feed();//ι��
	FLASH_Unlock();
	//���������flash��״̬��־����Ҫ�����û������ܻᵼ�²���flashʧ�ܻ��߲��ܲ���
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	(void)FLASH_EraseSector(FLASH_Sector,VoltageRange_3);
	FLASH_Lock();
	//IWDG_Feed();//ι��
	__enable_irq() ; //�����ж�
}


/*
//дflash
*/
static void http_wrFlash(uint32_t FLASH_addr,uint8_t* Le_u_Dt,uint16_t Le_w_lng)
{
	uint16_t Le_w_i;
	//__disable_irq() ; //�ر����ж�
	FLASH_Unlock();
	for(Le_w_i = 0;Le_w_i < Le_w_lng;Le_w_i++)
	{
		FLASH_ProgramByte((FLASH_addr+Le_w_i), Le_u_Dt[Le_w_i]);
	}
	FLASH_Lock();
	//__enable_irq() ; //�����ж�
}

/*
//��flash
*/
static void http_rdFlash(uint32_t FLASH_addr,uint8_t* Le_u_Dt,uint16_t Le_w_lng)
{
	uint16_t Le_w_i;
	__disable_irq() ; //�ر����ж�
	for(Le_w_i=0; Le_w_i < Le_w_lng; Le_w_i++)
	{
		Le_u_Dt[Le_w_i] = *(__IO uint8*)FLASH_addr;//��FLASH�е����ݣ�ֱ�Ӹ�����ַ�����ˡ������ڴ��ж�����һ��
		FLASH_addr++;
	}
	__enable_irq() ; //�����ж�
}

/******************************************************
*��������

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
******************************************************/
static uint32 Gethttpclient_dw_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng)
{
	uint16 Le_w_i;
	uint8 Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return (uint32)Le_u_Xor;
}

#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
