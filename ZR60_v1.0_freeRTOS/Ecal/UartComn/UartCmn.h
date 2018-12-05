/******************************************************
�ļ�����	UartCmn

������	��˫������ͨ��	

Data			  Vasion			author
2018/04/1		  V1.0			    liujian
*******************************************************/
#ifndef		_UARTCMN_H
#define		_UARTCMN_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
/*���պ궨��*/
/*TX522BЭ��֡*/
#define   UARTCMN_RX_HEADER    	    	0x20/*���ձ���ͷ��ʼ��*/
#define   UARTCMN_RX_PCKTHEAD_LNG       1U/*����ͷ���ȣ���λ���ֽڣ�*/

#define   UARTCMN_RX_PCKTHEAD_SEQNR     1U/*���ĺų��ȣ���λ���ֽڣ�*/
#define   UARTCMN_RX_PCKTHEAD_CMD     	1U/*���������֣���λ���ֽڣ�*/
#define   UARTCMN_RX_DATALNG_LNG        1U/*��Ч���ݳ��ȱ�ʾ�ֽ�:�ޣ�����0*/
#define   UARTCMN_RX_PCKTDATA_LNG       56U/*������Ч������󳤶�*/
#define   UARTCMN_RX_CHECK_LNG          1U/*����У���ֽڳ���*/
#define   UARTCMN_RX_ETX_LNG            1U/*���Ľ����ֽڳ���*/
#define   UARTCMN_RX_ETC    	    	0x03/*���ձ�����ֹ��*/


#define   UARTCMN_RX_SEQNR_STRTBYTE		UARTCMN_RX_PCKTHEAD_LNG 
#define   UARTCMN_RX_DATALNG_STRTBYTE	(UARTCMN_RX_PCKTHEAD_LNG + UARTCMN_RX_PCKTHEAD_SEQNR + UARTCMN_RX_PCKTHEAD_CMD)/*������Ч���ݳ��ȱ�ʾ�ֽڿ�ʼ�ֽ�*/
#define   UARTCMN_RX_DATA_STRTBYTE      (UARTCMN_RX_PCKTHEAD_LNG + UARTCMN_RX_PCKTHEAD_SEQNR + UARTCMN_RX_PCKTHEAD_CMD + UARTCMN_RX_DATALNG_LNG)/*������Ч���ݿ�ʼ�ֽ�*/
#define   UARTCMN_RX_CHCK_STRTBYTE      (UARTCMN_RX_PCKTHEAD_LNG + UARTCMN_RX_PCKTHEAD_SEQNR + UARTCMN_RX_PCKTHEAD_CMD + UARTCMN_RX_DATALNG_LNG + UARTCMN_RX_PCKTDATA_LNG)     /*����У���ֽ���ʼλ*/
#define   UARTCMN_RX_SCLD_LNG           (UARTCMN_RX_PCKTHEAD_SEQNR + UARTCMN_RX_PCKTHEAD_CMD +UARTCMN_RX_DATALNG_LNG + UARTCMN_RX_PCKTDATA_LNG)/*����+����+��Ч���ݳ����ֽ�+��Ч����*/
#define   UARTCMN_RX_PCKT_LNG           (UARTCMN_RX_CHCK_STRTBYTE + UARTCMN_RX_CHECK_LNG + UARTCMN_RX_ETX_LNG)/*�����ܳ�*/
/*���ͺ궨��*/
#define   UARTCMN_TX_HEADER    	    	0x20/*����ͷ��ʼ��*/
#define   UARTCMN_TX_ETC    	    	0x03/*������ֹ��*/
#define   UARTCMN_TX_PCKT_LNG           72U/*���ͱ����ܳ���󳤶�*/




/*SCIͨ������*/





/***********�꺯��***********/
#define	 UARTCMN_USART		USART3
#define  DISABLE_TIEN       USART_ITConfig(UART3_USART, USART_IT_TXE, DISABLE)/* ��ֹ���ͼĴ������ж� */
#define  ENABLE_TIEN	 	USART_ITConfig(UART3_USART, USART_IT_TXE, ENABLE)/* ʹ�ܷ��ͼĴ������ж� */	

#define  DISABLE_TEN    	(UARTCMN_USART->CR1 &= (uint8_t)(~USART_Mode_Tx))/* ��ֹ���� */
#define  ENABLE_TEN		    (UARTCMN_USART->CR1 |= (uint8_t)USART_Mode_Tx) /* ������ */

#define	 ENABLE_RIEN    	USART_ITConfig(UART3_USART, USART_IT_RXNE, ENABLE)/* ʹ�ܽ����ж����� */
#define	 DISABLE_RIEN		USART_ITConfig(UART3_USART, USART_IT_RXNE, DISABLE)/* �رս����ж����� */

#define	 ENABLE_REN			(UARTCMN_USART->CR1 |= (uint8_t)USART_Mode_Rx) /* ������� */	
#define  DISABLE_REN		(UARTCMN_USART->CR1 &= (uint8_t)(~USART_Mode_Rx)) /* ��ֹ���� */

#define UartCmn_GetFlagStatus(flag)   	USART_GetFlagStatus(UARTCMN_USART,flag)     
#define UartCmn_ReceiveData8()			USART_ReceiveData(UARTCMN_USART)
#define	UartCmn_SendData8(data)			USART_SendData(UARTCMN_USART,data) 

#define UARTCMN_FLAG_OE					USART_FLAG_ORE//֡�����־
#define	UARTCMN_FLAG_FE					USART_FLAG_FE//֡�����־
#define	UARTCMN_FLAG_RXNE				USART_FLAG_RXNE//���ռĴ����ǿձ�־
#define	UARTCMN_FLAG_TXE				USART_FLAG_TXE//���ͼĴ�����
#define	UARTCMN_IT_TC					USART_IT_TC//�������
/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{		
	unsigned char UartRxNm;/* ���������±� */
	unsigned char UartRXFlag;/* ������ɱ�־ */ 
	unsigned char RxBusyFlg;/* ���Ľ���æ��־ */
	unsigned char UartHeaderFlag;/* ��ͷ������ɱ�־ */
	unsigned char UartNewFlag;/* ���ܵ������ݱ�־ */
	union
	{
		unsigned char UartPckt[UARTCMN_RX_PCKT_LNG];/* �����ܳ�������ͷ+����+У��+֡���� */
		struct
		{
			unsigned char UartHeader[UARTCMN_RX_PCKTHEAD_LNG];/* ���ձ�ͷ */
			unsigned char UartSeqnr[UARTCMN_RX_PCKTHEAD_SEQNR];/* ����֡����*/
			unsigned char UartCmd[UARTCMN_RX_PCKTHEAD_CMD];/* ����֡����*/
			unsigned char UartRxDataLng[UARTCMN_RX_DATALNG_LNG];/* ������Ч���ݶγ��ȱ�ʾ*/
			unsigned char UartRxData[UARTCMN_RX_PCKTDATA_LNG];/* ���յ���Ч���ݶΣ�����󳤶ȶ���洢�ռ�*/
			unsigned char UartCheck[UARTCMN_RX_CHECK_LNG];/* ���ռ����ֽ� */
			unsigned char UartETX[UARTCMN_RX_ETX_LNG];/* ֡����*/
		}Pckt;
	}UartRx;
}Uart_Rx_Msg; /*���Ľ��սṹ��*/ 

typedef struct
{
    uint8  ucTxData[UARTCMN_TX_PCKT_LNG];/* ���ͱ��Ļ��� */
    uint8  ucLength;/* ���ĳ��� */
    uint8  ucTxCnt;/*���ͼ���*/
	uint8  ucTxFlg;/*���ͱ�־��1����������Ҫ�����ڷ��ͣ�0����û�����ݷ���*/
}Uart_TX_Msg;/*���ķ��ͽṹ��*/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/
extern uint8_t volatile RcvFlag;
extern uint8_t volatile blenum;
extern uint8_t volatile ble_flag;
extern uint8_t volatile set_time_flag;
extern char  timelist[15];
extern char BleValue[6];
extern uint8_t Se_u_MacAddr[12U];//mac��ַ
extern uint8_t volatile Se_u_BleMacFlg;//��ȡ����mac��־
extern uint8_t volatile Se_u_PhoneNum[12U];//�ֻ���
extern uint8_t volatile Se_u_PhoneNumFlg;//��ȡ�ֻ��ű�־
/*******************************************************
description�� function External declaration
*******************************************************/
extern void  InitUartCmn_parameter(void);
extern void  UartCmn_Rx_Msg(void);
extern void  UartCmn_Tx_Msg(void);
extern void  SetUartCmn_TxMsg(uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng);
extern uint8 GetUartCmn_u_RxMsgRenew(void);
extern void  GetUartCmn_u_RxMsg(uint8* LeUartCmn_RxData);
extern uint8 GetUartCmn_u_TxIdle(void);
extern uint8 GetUartCmn_u_RxIdle(void);
extern void Uart_Send(uint8* Data,uint8 Length);
extern void Uart_PrintStr(uint8* Str);


extern void SetUartCmn_BluetoothTxMsg(char* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng);
extern void UartCmn_Tx_BluetoothMsg(void);
extern void UartCmn_Rx_BleMsg(void);
#endif 
