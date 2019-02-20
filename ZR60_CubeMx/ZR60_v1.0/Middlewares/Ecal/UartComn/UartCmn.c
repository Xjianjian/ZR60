/******************************************************
�ļ�����	UartCmn

������	��˫������ͨ��	TX522B	
Data			Vasion			author
2018/04/1		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "UartCmn.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static Uart_Rx_Msg   Uart_RecMsg;/*�������ݽṹ���Ա*/
static Uart_TX_Msg   Uart_TxMsg;/*�������ݽṹ���Ա*/

static Uart_TX_Msg   Uart_BluetoothTxMsg;/*�������ݽṹ���Ա(����ģ��)*/
static uint8 		 SaUartCmn_u_RxBuff[UARTCMN_RX_PCKT_LNG];/*���ձ��Ļ�����*/
/*******************************************************
description�� global variable definitions
*******************************************************/
static char BleTemp[120];
char BleTempPassword[12];//����������ʱ���룺0~5���ſڻ����룻6~11��Χǽ������
char  timelist[15] = {0};
uint8_t volatile blenum = 0;
uint8_t volatile ble_flag = 0U;
uint8_t volatile set_time_flag =0U;
uint8_t volatile RcvFlag = 0U;
uint8_t Se_u_MacAddr[12U] = {0};//mac��ַ
uint8_t volatile Se_u_BleMacFlg = 0U;//��ȡ����mac��־
uint8_t volatile Se_u_PhoneNum[12U] = {0};//�ֻ���
uint8_t volatile Se_u_PhoneNumFlg = 0U;//��ȡ�ֻ��ű�־
/*******************************************************
description�� static variable definitions
*******************************************************/
static void UartCmn_u_RxMsgBuff(void);
static uint8 GetUartCmn_u_XOR(void);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������void InitUartCmn_parameter(void)

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitUartCmn_parameter(void)
{
	Uart_TxMsg.ucTxFlg = 0U;/*��ʼ�������ݷ��ͱ�־��Ч*/
	Uart_TxMsg.ucTxCnt = 0U;
	Uart_RecMsg.RxBusyFlg = 0U;/*��ʼ�������ݽ���æ��־��Ч*/
}

/******************************************************
*��������UartCmn_Rx_Msg

*��  �Σ�

*����ֵ��

*��  �������ڽ���

*��  ע�����ڽ����жϵ���
******************************************************/
void UartCmn_Rx_Msg(void)
{   
    unsigned char Buff;/* �������ݻ��� */
         
    if (UartCmn_GetFlagStatus(UARTCMN_FLAG_OE) || UartCmn_GetFlagStatus(UARTCMN_FLAG_FE))/* ֡�����־λ ���� ���������־λ */
    {
        Buff = UartCmn_ReceiveData8();/* ���Break�����ݼ�֡�������� */         
    }
	else if (UartCmn_GetFlagStatus(UARTCMN_FLAG_RXNE))/* �������ݼĴ�������־λ */
    {
		Uart_RecMsg.RxBusyFlg = 1U;/*�����ݽ���æ��־��Ч*/
        Buff = UartCmn_ReceiveData8();/*��ȡ���ݼĴ�������*/        
        switch(Uart_RecMsg.UartHeaderFlag) 
        {
			case 0U:/*����֡ͷ*/                           		
				Uart_RecMsg.UartRx.Pckt.UartHeader[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartHeader[0] == UARTCMN_RX_HEADER)   /*�ж�֡ͷ�Ƿ���ȷ*/
				{
					Uart_RecMsg.UartHeaderFlag=1U; /*����������֡β����*/
				} 
				else 
				{
					/*֡ͷ�������½���֡ͷ*/
				}
			break;		
			case 1U:/*���հ���*/
				Uart_RecMsg.UartRx.Pckt.UartSeqnr[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 2;
			break;
			case 2U:/*���������ֽ�*/
				Uart_RecMsg.UartRx.Pckt.UartCmd[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 3;
			break;
			case 3U:/*������Ч���ݳ��ȱ�ʾ�ֽ�*/
				Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] == 0U)
				{
					Uart_RecMsg.UartHeaderFlag = 5;
				}
				else if(Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] <= UARTCMN_RX_PCKTDATA_LNG)
				{/*���ݳ�����Ч*/
					Uart_RecMsg.UartHeaderFlag = 4;
				}
				else/*���ݳ��ȴ���*/
				{
					Uart_RecMsg.UartHeaderFlag = 0;
				}
			break;
			case 4U:/*������Ч����*/ 
				Uart_RecMsg.UartRx.Pckt.UartRxData[Uart_RecMsg.UartRxNm++]=Buff;
				if(Uart_RecMsg.UartRxNm >= Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0])/*��Ч���ݽ������*/
				{
					Uart_RecMsg.UartRxNm = 0U;/*������Ч���ݼ���������*/
					Uart_RecMsg.UartHeaderFlag = 5U;	
				}
				else
				{
					/*��Ч���ݽ�����*/
				}
			break;
			case 5U:/*����У����ֽ�*/
				Uart_RecMsg.UartRx.Pckt.UartCheck[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 6;
			break;
			case 6U:/*���ս����ֽ�*/
				Uart_RecMsg.UartRx.Pckt.UartETX[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartETX[0] == UARTCMN_RX_ETC)/*��֡���Ľ������*/
				{
					Uart_RecMsg.UartRXFlag = 1U;/*���ճɹ���־*/
					Uart_RecMsg.RxBusyFlg = 0U;/*�����ݽ���æЧ*/
					Uart_RecMsg.UartHeaderFlag = 0;
				}
				else/*������ֹ������*/
				{
					Uart_RecMsg.UartHeaderFlag = 0;
				}
				
			break;
			default:
				Uart_RecMsg.UartHeaderFlag = 0U;
			break;
        }
    }
	else
	{}
	
	if(1U == Uart_RecMsg.UartRXFlag)
	{
		Uart_RecMsg.UartRXFlag = 0U;
		UartCmn_u_RxMsgBuff();/*�������ݻ���*/
	}
}


/******************************************************
*��������UartCmn_Tx_Msg

*��  �Σ�

*����ֵ��

*��  �������ڷ���

*��  ע��
******************************************************/
void UartCmn_Tx_Msg(void)
{
	//uint32 Le_dw_Timer = 0xffffffff; 
	if(1U == Uart_TxMsg.ucTxFlg)
	{
		//if(UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE))/*���ͼĴ�����*/
		{
			if(Uart_TxMsg.ucTxCnt < Uart_TxMsg.ucLength)
			{    
				UartCmn_SendData8(Uart_TxMsg.ucTxData[Uart_TxMsg.ucTxCnt]) ;/* �������ݳ� */
				Uart_TxMsg.ucTxCnt++;		
			}
			else if(Uart_TxMsg.ucTxCnt >= Uart_TxMsg.ucLength)
			{
				//while(!(UartCmn_GetFlagStatus(UARTCMN_IT_TC)) && (Le_dw_Timer--));
				Uart_TxMsg.ucTxCnt = 0U;/*�巢�ͼ���ֵ*/
				Uart_TxMsg.ucTxFlg = 0U;/*�巢�ͱ�־*/
				DISABLE_TIEN;/* ��ֹ���ͼĴ������ж� */
				//DISABLE_TEN;  /* ��ֹ���� */
				//ENABLE_RIEN; /* �����ж����� */
				//ENABLE_REN; /* ������� */	
			}
			else
			{}
		}  		
	}
}


/******************************************************
*��������UartCmn_u_RxMsgBuff

*��  �Σ�

*����ֵ��

*��  �����������ݻ���

*��  ע��
******************************************************/
static void UartCmn_u_RxMsgBuff(void)
{ 
	uint8 LeUartCmn_u_Index;
	for(LeUartCmn_u_Index = 0U;LeUartCmn_u_Index < UARTCMN_RX_PCKT_LNG; LeUartCmn_u_Index++)
	{
		SaUartCmn_u_RxBuff[LeUartCmn_u_Index] = Uart_RecMsg.UartRx.UartPckt[LeUartCmn_u_Index];
	}
	Uart_RecMsg.UartNewFlag =1U;
}


/******************************************************
*��������GetUartCmn_u_RxMsgRenew

*��  �Σ�

*����ֵ��

*��  ������ȡ�������ݸ��±�־

*��  ע��9600�����ʣ�10ms�������ڣ�115200�����ʣ�5ms��������
******************************************************/
uint8 GetUartCmn_u_RxMsgRenew(void)
{
	uint8 LeUartCmn_u_Flg = 0U;
	if(Uart_RecMsg.UartNewFlag == 1U)/*�������ݻ���������*/
	{
		Uart_RecMsg.UartNewFlag = 0U;
		if(1U == GetUartCmn_u_XOR())/*У��ͨ��*/
		{
			LeUartCmn_u_Flg =1U;/*�������ݸ��±�־����Ч*/
		}
	}
	return LeUartCmn_u_Flg;
}


/******************************************************
*��������GetUartCmn_u_XOR

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
******************************************************/
static uint8 GetUartCmn_u_XOR(void)
{
	uint8 LeUartCmn_u_i;
	uint8 LeUartCmn_u_Lng;
	uint8 LeUartCmn_u_ok = 0U;
	uint8 LeUartCmn_u_Xor = 0U;
	LeUartCmn_u_Lng = SaUartCmn_u_RxBuff[3] + 3;
	for(LeUartCmn_u_i = 0U; LeUartCmn_u_i < LeUartCmn_u_Lng;LeUartCmn_u_i++)
	{
		LeUartCmn_u_Xor ^= SaUartCmn_u_RxBuff[UARTCMN_RX_SEQNR_STRTBYTE + LeUartCmn_u_i];
	}
	LeUartCmn_u_Xor = (~LeUartCmn_u_Xor);
	if(LeUartCmn_u_Xor == SaUartCmn_u_RxBuff[60])/*У��ͨ��*/
	{
		LeUartCmn_u_ok = 1U;
	}
	return LeUartCmn_u_ok;
}

/******************************************************
*��������GetUartCmn_u_RxMsg

*��  �Σ�

*����ֵ��

*��  ������ȡ��������

*��  ע��
******************************************************/
void GetUartCmn_u_RxMsg(uint8* LeUartCmn_RxData)
{ 
	uint8 LeUartCmn_u_Index;
	for(LeUartCmn_u_Index = 0U;LeUartCmn_u_Index < UARTCMN_RX_SCLD_LNG; LeUartCmn_u_Index++)
	{
		LeUartCmn_RxData[LeUartCmn_u_Index] = SaUartCmn_u_RxBuff[UARTCMN_RX_PCKTHEAD_LNG+LeUartCmn_u_Index];
	}
}


/******************************************************
*��������GetUartCmn_u_TxIdle

*��  �Σ�

*����ֵ�� 1--���У�0--æ

*��  ������ȡ�����Ƿ����:1--���У�0--æ

*��  ע��
******************************************************/
uint8 GetUartCmn_u_TxIdle(void)
{
	if(0U == Uart_TxMsg.ucTxFlg)
	{
		return 1U;
	}
	else
	{
		return 0U;
	}
}

/******************************************************
*��������GetUartCmn_u_TxIdle

*��  �Σ�

*����ֵ�� 1--���У�0--æ

*��  ������ȡ���ݽ����Ƿ����:1--���У�0--æ

*��  ע��
******************************************************/
uint8 GetUartCmn_u_RxIdle(void)
{
	if(0U == Uart_RecMsg.RxBusyFlg)
	{
		return 1U;
	}
	else
	{
		return 0U;
	}
}

/******************************************************
*��������SetUartCmn_TxMsg

*��  �Σ�LeUartCmn_u_TxData[]   ��������
		LeUartCmn_u_Lng		   ��������֡����

*����ֵ��

*��  �������÷��ͱ��Ĳ���������

*��  ע��
******************************************************/
void SetUartCmn_TxMsg(uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng)
{
	uint8 LeUartCmn_u_Index;
	//Uart_TxMsg.ucTxData[0U] = UARTCMN_TX_HEADER;
	for(LeUartCmn_u_Index = 0U;LeUartCmn_u_Index < LeUartCmn_u_Lng;LeUartCmn_u_Index++)
	{
		Uart_TxMsg.ucTxData[LeUartCmn_u_Index] = LeUartCmn_u_TxData[LeUartCmn_u_Index];
	}
	//Uart_TxMsg.ucTxData[1+LeUartCmn_u_Lng] = UARTCMN_TX_ETC;
	Uart_TxMsg.ucLength = LeUartCmn_u_Lng;
	Uart_TxMsg.ucTxFlg = 1U;/*�÷��ͱ�־��Ч*/
	//DISABLE_RIEN;/* �رս����ж����� */
	//DISABLE_REN; /* ��ֹ���� */
	ENABLE_TEN; /* ������ */
	ENABLE_TIEN;/* ʹ�ܷ��ͼĴ������ж� */		
}


/******************************************************
*��������Uart_Send

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��
******************************************************/
void Uart_Send(uint8_t* Data,uint8_t Length)
{
	uint8_t LeUart_u_Len;
	DISABLE_RIEN;/* �رս����ж����� */
	DISABLE_REN; /* ��ֹ���� */
    ENABLE_TEN;
	Uart_TxMsg.ucTxFlg = 1U;/*�÷��ͱ�־��Ч*/
	while((UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE)==RESET));	
	UartCmn_SendData8(UARTCMN_TX_HEADER);
	while((UartCmn_GetFlagStatus(UARTCMN_IT_TC)==RESET));
	
	for(LeUart_u_Len = 0;LeUart_u_Len < Length;LeUart_u_Len++)
	{
		while((UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE)==RESET));	
		UartCmn_SendData8(Data[LeUart_u_Len]);
		while((UartCmn_GetFlagStatus(UARTCMN_IT_TC)==RESET));
	}
	while((UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE)==RESET));	
	UartCmn_SendData8(UARTCMN_TX_ETC);
	while((UartCmn_GetFlagStatus(UARTCMN_IT_TC)==RESET));	
	Uart_TxMsg.ucTxFlg = 0U;/*�÷��ͱ�־��Ч*/
	
    DISABLE_TEN;
	ENABLE_RIEN; /* �����ж����� */
	ENABLE_REN; /* ������� */
}

/******************************************************
*��������Uart_PrintStr

*��  �Σ�void

*����ֵ��void

*��  ������ӡ�ַ���

*��  ע��
******************************************************/
void Uart_PrintStr(uint8_t* Str)
{
    ENABLE_TEN;
	while(*Str)
	{
		while((UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE)==RESET));	
		UartCmn_SendData8(*(Str++));
		while((UartCmn_GetFlagStatus(UARTCMN_IT_TC)==RESET));
	}
    DISABLE_TEN;
}


/******************************************************
*��������SetUartCmn_BluetoothTxMsg

*��  �Σ�LeUartCmn_u_TxData[]   ��������
		LeUartCmn_u_Lng		   ��������֡����

*����ֵ��

*��  �������÷��ͱ��Ĳ���������(����ģ��)

*��  ע��
******************************************************/
void SetUartCmn_BluetoothTxMsg(char* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng)
{
	uint8 LeUartCmn_u_Index;
	for(LeUartCmn_u_Index = 0U;LeUartCmn_u_Index < LeUartCmn_u_Lng;LeUartCmn_u_Index++)
	{
		Uart_BluetoothTxMsg.ucTxData[LeUartCmn_u_Index] = LeUartCmn_u_TxData[LeUartCmn_u_Index];
	}
	Uart_BluetoothTxMsg.ucLength = LeUartCmn_u_Lng;
	Uart_BluetoothTxMsg.ucTxFlg = 1U;/*�÷��ͱ�־��Ч*/
	//USART_SendData(UART5,Uart_BluetoothTxMsg.ucTxData[Uart_BluetoothTxMsg.ucTxCnt]);/* �������ݳ� */
	//Uart_BluetoothTxMsg.ucTxCnt++;	
	//USART_ITConfig(UART5_USART, USART_IT_RXNE, DISABLE);/* �رս����ж����� */
	//(UART5->CR1 &= (uint8_t)(~USART_Mode_Rx));/* ��ֹ���� */
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);/* ʹ�ܷ��ͼĴ������ж� */
	//(UART5->CR1 |= (uint8_t)USART_Mode_Tx);/* ������ */
	//DISABLE_RIEN;/* �رս����ж����� */
	//DISABLE_REN; /* ��ֹ���� */
	//ENABLE_TEN; /* ������ */
	//ENABLE_TIEN;/* ʹ�ܷ��ͼĴ������ж� */		
}

/******************************************************
*��������UartCmn_Tx_BluetoothMsg

*��  �Σ�

*����ֵ��

*��  �������ڷ���(����ģ��)

*��  ע��
******************************************************/
void UartCmn_Tx_BluetoothMsg(void)
{
	//uint32 Le_dw_Timer = 0x00ffffff; 
	if(1U == Uart_BluetoothTxMsg.ucTxFlg)
	{
		//if(USART_GetFlagStatus(UART5,USART_FLAG_TXE))/*���ͼĴ�����*/
		{
			if(Uart_BluetoothTxMsg.ucTxCnt < Uart_BluetoothTxMsg.ucLength)
			{    
				USART_SendData(UART5,Uart_BluetoothTxMsg.ucTxData[Uart_BluetoothTxMsg.ucTxCnt]);/* �������ݳ� */
				Uart_BluetoothTxMsg.ucTxCnt++;		
			}
			else if(Uart_BluetoothTxMsg.ucTxCnt >= Uart_BluetoothTxMsg.ucLength)
			{
				//while(!(USART_GetFlagStatus(UART5,UARTCMN_IT_TC)) && (Le_dw_Timer--));
				Uart_BluetoothTxMsg.ucTxCnt = 0U;/*�巢�ͼ���ֵ*/
				Uart_BluetoothTxMsg.ucTxFlg = 0U;/*�巢�ͱ�־*/
				USART_ITConfig(UART5_USART, USART_IT_TXE, DISABLE);/* ��ֹ���ͼĴ������ж� */	
				//(UART5->CR1 &= (uint8_t)(~USART_Mode_Tx));/* ��ֹ���� */
				//USART_ITConfig(UART5_USART, USART_IT_RXNE, ENABLE);/* �����ж����� */
				//(UART5->CR1 |= (uint8_t)USART_Mode_Rx); /* ������� */	
				//DISABLE_TIEN;/* ��ֹ���ͼĴ������ж� */
				//DISABLE_TEN;  /* ��ֹ���� */
				//ENABLE_RIEN; /* �����ж����� */
				//ENABLE_REN; /* ������� */	
			}
			else
			{}
		}  		
	}
}



void UartCmn_Rx_BleMsg(void)
{
	uint8_t i;
	uint8 Buff;
	if(USART_GetFlagStatus(UART5,UARTCMN_FLAG_OE)  || USART_GetFlagStatus(UART5,UARTCMN_FLAG_FE))/* ֡�����־λ ���� ���������־λ */
	{
		Buff = USART_ReceiveData(UART5);/* ���Break�����ݼ�֡�������� */ 
		blenum = 0;
	}				
	else if (USART_GetFlagStatus(UART5,UARTCMN_FLAG_RXNE))/* �������ݼĴ�������־λ */
	{
		Buff = USART_ReceiveData(UART5); 
		switch(RcvFlag)
		{
			case 0U:
			{
				if(Buff == 'O')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 1U;
				}
			}
			break;
			case 1U:
			{
				if(Buff == 'K')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 2U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 2U:
			{
				if(Buff == '+')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 3U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 3U:
			{
				if(Buff == 'D')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 4U;
				}	
				else if(Buff == 'A')//׼������MAC��ַ(hm -11)
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 12U;
				}
				else if(Buff == 'G')//׼������MAC��ַ(hm -13)
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 12U;
				}
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 4U:
			{
				if(Buff == 'I')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 5U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 5U:
			{
				if(Buff == 'S')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 6U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 6U:
			{
				switch(Buff)
				{
					case 'A'://����ɨ�裺׼��������Ч����(hm-11)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 7U;
					}
					break;
					case 'C'://����ɨ�裺׼�����տ�ʼ���������֡(hm-11)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 11U;
					}
					break;
					case 'B'://����ɨ�裺׼�����տ�ʼ���������֡(hm-13)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 11U;
					}
					break;
					case '0'://����ɨ�裺׼��������Ч����(hm-13)
					case '1':
					case '2':
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 7U;
					}
					break;
					default:
					{
						blenum = 0U;
						RcvFlag = 0U;
					}
					break;
				}
			}
			break;
			case 7U:
			{
				if(Buff == ':')//��Ч����ͷ�������
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 8U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 8U://����P1~P4,9 Byte(hm-11);����P2~P4, 19 Byte(hm-13);
			{
				BleTemp[blenum] = Buff;
				blenum++;
#ifdef HM_11
				if(blenum == 17)
				{
					if((17+BleTemp[16]) < 120U)
					{
						RcvFlag = 9U;
					}
					else
					{//�����������
						blenum = 0;
						RcvFlag = 0U;
					}
				}
#else
				if(blenum == 27)
				{
					if(BleTemp[26] != 0)
					{
						if((27+BleTemp[26]) < 120U)
						{
							RcvFlag = 9U;
						}
						else
						{//�����������
							blenum = 0;
							RcvFlag = 0U;
						}
					}
					else
					{
						blenum = 0;
						RcvFlag = 0U;
					}
				}		
#endif
			}
			break;
			case 9U://����Reset DATA
			{
				BleTemp[blenum] = Buff;
				blenum++;
#ifdef HM_11
				if(blenum == (17+BleTemp[16]))//����ɨ�����ݽ������(hm-11)
				{
					RcvFlag = 10U;
				}
#else
				if(blenum == (27+BleTemp[26]))//����ɨ�����ݽ������(hm-13)
				{
					RcvFlag = 10U;
				}				
#endif
			}
			break;
			case 10U://������ɴ���һ֡������'\n'(0d 0a)���������յ�'\n'���һ֡���ݴ���
			{
#ifdef HM_11
				switch(BleTemp[16])
				{
					case 0x1A://��׿������ʱ����
					{
						if((BleTemp[19] == 0x57) && (BleTemp[20] == 0x5A))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[21+i];//�ſڻ�����
								BleTempPassword[i + 6] = BleTemp[37+i];//Χǽ������
							}
							ble_flag = 1;
						}
						else
						{}
					}
					break;
					case 0x0F://��׿�����ֻ���
					{
						if((BleTemp[19] == 0x77) && (BleTemp[20] == 0x7A))
						{
							for(i = 0;i < 11U;i++)
							{
								Se_u_PhoneNum[i] = BleTemp[21+i];
							}
							Se_u_PhoneNumFlg = 1;
						}
					}
					break;
					case 0x18://��׿Уʱ
					{
						if((BleTemp[19] == 0x49) && (BleTemp[20] == 0x54))
						{
							strncpy(timelist,BleTemp+21,14);
							set_time_flag = 1;
						}
					}						
					case 0x08://ƻ��IOS������ʱ����
					{
						if((BleTemp[17] == 0x07) && (BleTemp[18] == 0x09))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[19+i];//�ſڻ�����
								BleTempPassword[i + 6] = BleTemp[19+i];//Χǽ������
							}
							ble_flag = 1;
						}
					}
					break;
					case 0x12://ƻ��IOSУʱ
					{
						if((BleTemp[19] == 0x54) && (BleTemp[20] == 0x49))
						{
							strncpy(timelist,BleTemp+21,14);
							set_time_flag = 1;
						}
					}
					break;
					case 0x1E://ƻ��IOS�����ֻ���
					{
						if((BleTemp[26] == 0x00) && (BleTemp[27] == 0x0A))
						{
							if(BleTemp[30] == 0x52)
							{//��ʾ���յ��ֻ���������1,��Ч���ֻ���
								for(i = 0;i < 11U;i++)
								{
									Se_u_PhoneNum[i] = BleTemp[30+i];
								}
								Se_u_PhoneNumFlg = 1;
							}
						}
					}
					break;
					default:
					{
						
					}
					break;				 					
				}
#else
				switch(BleTemp[26])
				{
					case 0x1A://��׿������ʱ����
					{
						if((BleTemp[29] == 0x57) && (BleTemp[30] == 0x5A))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[31+i];//�ſڻ�����
								BleTempPassword[i + 6] = BleTemp[47+i];//Χǽ������
							}
							ble_flag = 1;
						}
						else
						{}
					}
					break;
					case 0x0F://��׿�����ֻ���
					{
						if((BleTemp[29] == 0x77) && (BleTemp[30] == 0x7A))
						{
							for(i = 0;i < 11U;i++)
							{
								Se_u_PhoneNum[i] = BleTemp[31+i];
							}
							Se_u_PhoneNumFlg = 1;
						}
					}
					break;
					case 0x18://��׿Уʱ
					{
						if((BleTemp[29] == 0x49) && (BleTemp[30] == 0x54))
						{
							strncpy(timelist,BleTemp+31,14);
							set_time_flag = 1;
						}
					}						
					case 0x08://ƻ��IOS������ʱ����
					{
						if((BleTemp[27] == 0x07) && (BleTemp[28] == 0x09))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[29+i];//�ſڻ�����
								BleTempPassword[i + 6] = BleTemp[29+i];//Χǽ������
							}
							ble_flag = 1;
						}
					}
					break;
					case 0x12://ƻ��IOSУʱ
					{
						if((BleTemp[29] == 0x54) && (BleTemp[30] == 0x49))
						{
							strncpy(timelist,BleTemp+31,14);
							set_time_flag = 1;
						}
					}
					break;
					case 0x1E://ƻ��IOS�����ֻ���
					{
						if((BleTemp[36] == 0x00) && (BleTemp[37] == 0x0A))
						{
							if(BleTemp[40] == 0x52)
							{//��ʾ���յ��ֻ���������1,��Ч���ֻ���
								for(i = 0;i < 11U;i++)
								{
									Se_u_PhoneNum[i] = BleTemp[40+i];
								}
								Se_u_PhoneNumFlg = 1;
							}
						}
					}
					break;
					default:
					{
						
					}
					break;				 					
				}		
#endif		
				blenum = 0;
				RcvFlag = 0U;
			}
			break;
			case 11U:
			{
				switch(Buff)
				{
					case 'S'://�յ���ʼ�ַ���
					{
						blenum = 0U;
						RcvFlag = 0U;
					}
					break;
					case 'E'://�յ������ַ���
					{
						blenum = 0U;
						RcvFlag = 0U;
						ClrZR60Ctrl_BleSt();
					}
					break;
					default:
					{
						blenum = 0U;
						RcvFlag = 0U;
					}
					break;
				}
			}
			break;
			case 12U://case 12~case 16����MAC��ַ
			{
				if(Buff == 'D')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 13U;
				}
				else if(Buff == 'e')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 13U;
				}
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 13U://case 12~case 16����MAC��ַ
			{
				if(Buff == 'D')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 14U;
				}
				else if(Buff == 't')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 15U;
				}
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 14U://case 12~case 16����MAC��ַ
			{
				if(Buff == 'R')
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 15U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 15U://case 12~case 16����MAC��ַ
			{
				if(Buff == ':')
				{//׼������MAC��ַ
					blenum = 0U;
					RcvFlag = 16U;
				}	
				else
				{
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			case 16U://����6Byte MAC��ַ
			{
				//BleTemp[blenum] = Buff;
				DeviceInit.Blemac[blenum] = Buff;
				if((Buff>= '0') &&(Buff <='9'))
				{
					Se_u_MacAddr[blenum++] = Buff-0x30;
				}
				else
				{
					Se_u_MacAddr[blenum++] = Buff-0x37;
				}
				if(blenum ==  12U)
				{
					DeviceInit.Blemac[blenum] = '\0';
					Se_u_BleMacFlg = 1U;
					blenum = 0U;
					RcvFlag = 0U;
				}
			}
			break;
			default:
			{
				blenum = 0;
				RcvFlag = 0U;
			}
			break;
		}
	}
}
