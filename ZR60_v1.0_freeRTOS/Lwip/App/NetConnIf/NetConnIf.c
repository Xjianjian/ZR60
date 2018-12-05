/**************************************************************
  * @file    NetConnIf.c
  * @author  MCD Application Team
  * @version V1.0
  * @date    22-11-2018
  * @brief   tcp client application using LwIP RAW API
***************************************************************/

/* Includes ---------------------------------------------------*/
#include "./NetConnIf/NetConnIf.h"

/* Private typedef --------------------------------------------*/
/* Private define ---------------------------------------------*/
/* Private macro ----------------------------------------------*/
/* Private variables ------------------------------------------*/


/* Private function prototypes ---------------------------------*/
static err_t NetConnIf_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  NetConnIf_connection_close(struct tcp_pcb *tpcb, NetConnIf_arg* arg);
static err_t NetConnIf_poll(void *arg, struct tcp_pcb *tpcb);
static err_t NetConnIf_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
//static uint8 tcp_LngConnect_parseJson(char * pMsg);
static void NetConnIf_Err(void *arg,err_t err);

/* Private functions --------------------------------------------*/
/******************************************************
*函数名：NetConnIf_Parameter

*形  参：

*返回值：

*描  述：初始化

*备  注：
******************************************************/
void NetConnIf_Parameter(void)
{
	char Le_u_i;
	for(Le_u_i = 0;Le_u_i < NETCONN_NUM;Le_u_i++)
	{
		NetConnIf[Le_u_i].arg.state = NETCONN_NOT_CONNECTED;
		NetConnIf[Le_u_i].arg.Object = 0xFF;
		NetConnIf[Le_u_i].pcb = NULL;
		NetConnIf[Le_u_i].rcvCallback = NULL;
		NetConnIf[Le_u_i].p_rx = NULL;	
		NetConnIf[Le_u_i].rxlng = 0;
	}
}

/******************************************************
*函数名：NetConnIf_Connect

*形  参：

*返回值：

*描  述：创建tcp连接

*备  注：
******************************************************/
char NetConnIf_Connect(char Le_u_obj,ip_addr_t *Le_u_DestIPaddr, u16_t Le_w_port)
{
	char ret;
	//struct ip_addr DestIPaddr;
	/* create new tcp pcb */
	NetConnIf[Le_u_obj].pcb = tcp_new();
	if (NetConnIf[Le_u_obj].pcb != NULL)
	{
		tcp_bind(NetConnIf[Le_u_obj].pcb, IP_ADDR_ANY, 0); 
		/* connect to destination address/port */
		NetConnIf[Le_u_obj].arg.Object = Le_u_obj;
		//NetConnIf[Le_u_obj].arg.state = ES_NOT_CONNECTED;
		tcp_arg(NetConnIf[Le_u_obj].pcb, &(NetConnIf[Le_u_obj].arg));
		ret = tcp_connect(NetConnIf[Le_u_obj].pcb,Le_u_DestIPaddr,Le_w_port,NetConnIf_connected);
		if(ERR_OK != ret)
		{
			memp_free(MEMP_TCP_PCB, NetConnIf[Le_u_obj].pcb);
			NetConnIf[Le_u_obj].pcb = NULL;
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 创建tcp连接时发生（ERR_OK != ret）错误\r\n",Le_u_obj);
		}
		else
		{
			tcp_err(NetConnIf[Le_u_obj].pcb,NetConnIf_Err); 
		}
	}
	else
	{
		ret = (char)ERR_MEM;
		/* deallocate the pcb */
		NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d tcp控制块分配失败\r\n",Le_u_obj);
	}
	return ret;
}


/*
	错误回调函数NetConnIf_Err
*/
static void NetConnIf_Err(void *arg,err_t err)
{     
	NETCONNIF_PRINTF_S("\r\nErrorLogging：连接err处理\r\n");
    switch(err)  
    {   
        case ERR_MEM:                                            /* Out of memory error */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_MEM错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;    
        case ERR_BUF:                                            /* Buffer error */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_BUF错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case  ERR_TIMEOUT:                                       /* Timeout */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_TIMEOUT错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case ERR_RTE:                                            /* Routing problem */        
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_RTE错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
		case ERR_ISCONN:                                          /* Already connected */  
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_ISCONN错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case ERR_ABRT:                                           /* Connection aborted */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_ABRT错误\r\n",((NetConnIf_arg*)arg)->Object);
			NetConnIf[((NetConnIf_arg*)arg)->Object].pcb = NULL;
        break;  
        case ERR_RST:                                            /* Connection reset */       
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_RST错误\r\n",((NetConnIf_arg*)arg)->Object);
			NetConnIf[((NetConnIf_arg*)arg)->Object].pcb = NULL;
        break;  
        case ERR_CONN:                                           /* Not connected */  
			 NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_CONN错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case ERR_CLSD:                                           /* Connection closed */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_CLSD错误\r\n",((NetConnIf_arg*)arg)->Object);
			NetConnIf[((NetConnIf_arg*)arg)->Object].pcb = NULL;
        break;  
        case ERR_VAL:                                            /* Illegal value */  
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_VAL错误\r\n",((NetConnIf_arg*)arg)->Object);
		break;  
        case ERR_ARG:                                            /* Illegal argument */  
            NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_ARG错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case ERR_USE:                                            /* Address in use */  
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_USE错误\r\n",((NetConnIf_arg*)arg)->Object);
		break;   
        case ERR_IF:                                             /* Low-level netif error */  
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_IF错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
        case ERR_INPROGRESS:                                     /* Operation in progress */ 
			NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 发生ERR_INPROGRESS错误\r\n",((NetConnIf_arg*)arg)->Object);
        break;  
		default:
		break;
    } 
}

/**
* @brief  Disconnects to the TCP echo server
* @param  None
* @retval None
*/
void  NetConnIf_disconnect(char Le_u_obj)
{
	/* close connection */
	NetConnIf_connection_close(NetConnIf[Le_u_obj].pcb,&(NetConnIf[Le_u_obj].arg));
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
static err_t NetConnIf_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	if (err == ERR_OK)   
	{
		/* allocate structure es to maintain tcp connection informations */
		((NetConnIf_arg*)arg)->state = NETCONN_CONNECTED;
		/* initialize LwIP tcp_recv callback function */ 
		tcp_recv(tpcb, NetConnIf_recv);

		/* initialize LwIP tcp_poll callback function */
		tcp_poll(tpcb, NetConnIf_poll, 1);
		NETCONNIF_PRINTF_S("连接建立成功\r\n");
		return ERR_OK;
	}
	else
	{
		NETCONNIF_PRINTF_D("\r\nErrorLogging：连接回调函数 err: %d\n",err);
		/* close connection */
		NetConnIf_connection_close(tpcb, (NetConnIf_arg*)arg);
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
static err_t NetConnIf_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 	
	err_t ret_err;
	/* if we receive an empty tcp frame from server => close connection */
	if(p == NULL)
	{
		/* remote host closed connection */
		((NetConnIf_arg*)arg)->state  = NETCONN_CLOSING;

		NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 远端关闭连接\r\n",((NetConnIf_arg*)arg)->Object);
		ret_err = ERR_OK;
	}  
  /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
	else if(err != ERR_OK)
	{
		NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 接收函数：err!=ERR_OK\r\n",((NetConnIf_arg*)arg)->Object);
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len); 
		/* free received pbuf*/
		pbuf_free(p);

		ret_err = err;
	}
	else if(((NetConnIf_arg*)arg)->state == NETCONN_CONNECTED)
	{
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);  
		memset(NetConnIf[((NetConnIf_arg*)arg)->Object].p_rx,0,NetConnIf[((NetConnIf_arg*)arg)->Object].rxlng);//清0
		memcpy(NetConnIf[((NetConnIf_arg*)arg)->Object].p_rx,p->payload,p->len);
		NETCONNIF_PRINTF_S(NetConnIf[((NetConnIf_arg*)arg)->Object].p_rx);
		NetConnIf[((NetConnIf_arg*)arg)->Object].rcvCallback();
		/* free received pbuf*/
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	/* data received when connection already closed */
	else
	{
		NETCONNIF_PRINTF_D("\r\nErrorLogging：连接对象 %d 接收到未知数据\r\n",((NetConnIf_arg*)arg)->Object);
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len); 
		/* free pbuf and do nothing */
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	return ret_err;
}


/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t NetConnIf_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err = ERR_OK;
	//NETCONNIF_PRINTF_D("连接对象 %d 调用了NetConnIf_poll\r\n",((NetConnIf_arg*)arg)->Object);
	return ret_err;
}


/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on client structure
  * @retval None
  */
static void NetConnIf_connection_close(struct tcp_pcb *tpcb, NetConnIf_arg* arg )
{
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
		//NETCONNIF_PRINTF_S("tpcb == NULL\r\n");
	}
	
	((NetConnIf_arg*)arg)->state = NETCONN_CLOSED;	
}

/*
	发送报文
*/
void NetConnIf_sendMsg(char Le_u_obj,char* data,uint16_t len)
{
	tcp_write(NetConnIf[Le_u_obj].pcb,(char*)data ,len, 1);
}

/*
	配置数据接收的buf，回调函数
	应用层创建tcp连接后或者初始化时调用
*/
void NetConnIf_rxConfig(char Le_u_obj,char* Le_p_rxbuf,uint16_t Le_w_lng,void* rx_callback_fn)
{
	NetConnIf[Le_u_obj].p_rx = Le_p_rxbuf;
	NetConnIf[Le_u_obj].rxlng = Le_w_lng;
	NetConnIf[Le_u_obj].rcvCallback = (Netconn_RcvCallback)rx_callback_fn;
}

/*
	tcp建立连接状态
	返回值：
	0 -- 未建立连接
	1 -- 建立连接成功
*/
char NetConnIf_ConnectStatus(char Le_u_obj)
{
	if((NetConnIf[Le_u_obj].arg.state == NETCONN_CONNECTED) && (ESTABLISHED == NetConnIf[Le_u_obj].pcb->state))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
	tcp远端连接关闭状态
	返回值：
	1 -- 远端关闭连接（提示需关闭本地连接）
*/
char NetConnIf_RemoteConnectClose(char Le_u_obj)
{
	if(NetConnIf[Le_u_obj].arg.state == NETCONN_CLOSING)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
