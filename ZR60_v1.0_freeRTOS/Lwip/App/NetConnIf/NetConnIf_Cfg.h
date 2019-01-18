/******************************************************
�ļ�����	NetConnIf_Cfg

������		 

Data			  Vasion			author
2018/11/22		V1.0			  liujian         
*******************************************************/
#ifndef		_NET_CONN_IF_CFG_H
#define		_NET_CONN_IF_CFG_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"


/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/
//#define  NETCONN_NUM	1U//�������Ӷ�������
#define  NETCONNIF_DEBUG
/**********�곣������*********/


/***********�꺯��***********/
typedef void (*Netconn_RcvCallback)(void);


#ifdef NETCONNIF_DEBUG
#define NETCONNIF_PRINTF_S(x)   	 printf("%s\n",x)
#define NETCONNIF_PRINTF_D(x,d)   	 printf(x,d)
#else
#define NETCONNIF_PRINTF_S(x)
#define NETCONNIF_PRINTF_D(x,d)
#endif
/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/******enum definitions******/
typedef enum
{
	TcpLngConnect = 0,//tcp������
	NETCONN_NUM
}NetConnIf_obj;//�������Ӷ���

typedef enum 
{
  NETCONN_NOT_CONNECTED = 0, //δ����
  NETCONN_CONNECTED,			//��������
  NETCONN_RECEIVED,			//���յ�����
  NETCONN_CLOSING,			//���ӹر���
  NETCONN_CLOSED			    //�����ѹر�
}NetConn_states;
/*****struct definitions*****/
/* structure to be passed as argument to the tcp callbacks */
typedef struct
{
  NetConn_states state; /* connection status */
  char Object;/* connection object */
}NetConnIf_arg;

typedef struct
{
  NetConnIf_arg arg; /* connection status */
  struct tcp_pcb *pcb;/* pointer on the current tcp_pcb */
  Netconn_RcvCallback rcvCallback;/* callback function of data received */
  char* p_rx;/* pointer of received */
  uint16_t rxlng;/* �������ݳ��� */
}NetConnIf_Struct;

extern NetConnIf_Struct  NetConnIf[NETCONN_NUM];

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/

    
#endif

