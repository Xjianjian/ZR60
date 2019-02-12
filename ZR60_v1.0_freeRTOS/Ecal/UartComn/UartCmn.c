/******************************************************
文件名：	UartCmn

描述：	半双工串口通信	TX522B	
Data			Vasion			author
2018/04/1		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "UartCmn.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static Uart_Rx_Msg   Uart_RecMsg;/*接收数据结构体成员*/
static Uart_TX_Msg   Uart_TxMsg;/*发送数据结构体成员*/

static Uart_TX_Msg   Uart_BluetoothTxMsg;/*发送数据结构体成员(蓝牙模块)*/
static uint8 		 SaUartCmn_u_RxBuff[UARTCMN_RX_PCKT_LNG];/*接收报文缓存区*/
/*******************************************************
description： global variable definitions
*******************************************************/
static char BleTemp[120];
char BleTempPassword[12];//蓝牙开门临时密码：0~5存门口机密码；6~11存围墙机密码
char  timelist[15] = {0};
uint8_t volatile blenum = 0;
uint8_t volatile ble_flag = 0U;
uint8_t volatile set_time_flag =0U;
uint8_t volatile RcvFlag = 0U;
uint8_t Se_u_MacAddr[12U] = {0};//mac地址
uint8_t volatile Se_u_BleMacFlg = 0U;//获取蓝牙mac标志
uint8_t volatile Se_u_PhoneNum[12U] = {0};//手机号
uint8_t volatile Se_u_PhoneNumFlg = 0U;//获取手机号标志
/*******************************************************
description： static variable definitions
*******************************************************/
static void UartCmn_u_RxMsgBuff(void);
static uint8 GetUartCmn_u_XOR(void);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：void InitUartCmn_parameter(void)

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitUartCmn_parameter(void)
{
	Uart_TxMsg.ucTxFlg = 0U;/*初始化置数据发送标志无效*/
	Uart_TxMsg.ucTxCnt = 0U;
	Uart_RecMsg.RxBusyFlg = 0U;/*初始化置数据接收忙标志无效*/
}

/******************************************************
*函数名：UartCmn_Rx_Msg

*形  参：

*返回值：

*描  述：串口接收

*备  注：串口接收中断调用
******************************************************/
void UartCmn_Rx_Msg(void)
{   
    unsigned char Buff;/* 接受数据缓存 */
         
    if (UartCmn_GetFlagStatus(UARTCMN_FLAG_OE) || UartCmn_GetFlagStatus(UARTCMN_FLAG_FE))/* 帧错误标志位 或者 数据溢出标志位 */
    {
        Buff = UartCmn_ReceiveData8();/* 清除Break段数据及帧错误数据 */         
    }
	else if (UartCmn_GetFlagStatus(UARTCMN_FLAG_RXNE))/* 接受数据寄存器满标志位 */
    {
		Uart_RecMsg.RxBusyFlg = 1U;/*置数据接收忙标志有效*/
        Buff = UartCmn_ReceiveData8();/*读取数据寄存器数据*/        
        switch(Uart_RecMsg.UartHeaderFlag) 
        {
			case 0U:/*接收帧头*/                           		
				Uart_RecMsg.UartRx.Pckt.UartHeader[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartHeader[0] == UARTCMN_RX_HEADER)   /*判断帧头是否正确*/
				{
					Uart_RecMsg.UartHeaderFlag=1U; /*启动数据与帧尾接收*/
				} 
				else 
				{
					/*帧头错误，重新接收帧头*/
				}
			break;		
			case 1U:/*接收包号*/
				Uart_RecMsg.UartRx.Pckt.UartSeqnr[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 2;
			break;
			case 2U:/*接收命令字节*/
				Uart_RecMsg.UartRx.Pckt.UartCmd[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 3;
			break;
			case 3U:/*接收有效数据长度表示字节*/
				Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] == 0U)
				{
					Uart_RecMsg.UartHeaderFlag = 5;
				}
				else if(Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0] <= UARTCMN_RX_PCKTDATA_LNG)
				{/*数据长度有效*/
					Uart_RecMsg.UartHeaderFlag = 4;
				}
				else/*数据长度错误*/
				{
					Uart_RecMsg.UartHeaderFlag = 0;
				}
			break;
			case 4U:/*接收有效数据*/ 
				Uart_RecMsg.UartRx.Pckt.UartRxData[Uart_RecMsg.UartRxNm++]=Buff;
				if(Uart_RecMsg.UartRxNm >= Uart_RecMsg.UartRx.Pckt.UartRxDataLng[0])/*有效数据接收完成*/
				{
					Uart_RecMsg.UartRxNm = 0U;/*接收有效数据计数器清零*/
					Uart_RecMsg.UartHeaderFlag = 5U;	
				}
				else
				{
					/*有效数据接收中*/
				}
			break;
			case 5U:/*接收校验和字节*/
				Uart_RecMsg.UartRx.Pckt.UartCheck[0] = Buff;
				Uart_RecMsg.UartHeaderFlag = 6;
			break;
			case 6U:/*接收结束字节*/
				Uart_RecMsg.UartRx.Pckt.UartETX[0] = Buff;
				if(Uart_RecMsg.UartRx.Pckt.UartETX[0] == UARTCMN_RX_ETC)/*整帧报文接收完成*/
				{
					Uart_RecMsg.UartRXFlag = 1U;/*接收成功标志*/
					Uart_RecMsg.RxBusyFlg = 0U;/*置数据接收忙效*/
					Uart_RecMsg.UartHeaderFlag = 0;
				}
				else/*报文终止符有误*/
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
		UartCmn_u_RxMsgBuff();/*接收数据缓存*/
	}
}


/******************************************************
*函数名：UartCmn_Tx_Msg

*形  参：

*返回值：

*描  述：串口发送

*备  注：
******************************************************/
void UartCmn_Tx_Msg(void)
{
	//uint32 Le_dw_Timer = 0xffffffff; 
	if(1U == Uart_TxMsg.ucTxFlg)
	{
		//if(UartCmn_GetFlagStatus(UARTCMN_FLAG_TXE))/*发送寄存器空*/
		{
			if(Uart_TxMsg.ucTxCnt < Uart_TxMsg.ucLength)
			{    
				UartCmn_SendData8(Uart_TxMsg.ucTxData[Uart_TxMsg.ucTxCnt]) ;/* 发送数据场 */
				Uart_TxMsg.ucTxCnt++;		
			}
			else if(Uart_TxMsg.ucTxCnt >= Uart_TxMsg.ucLength)
			{
				//while(!(UartCmn_GetFlagStatus(UARTCMN_IT_TC)) && (Le_dw_Timer--));
				Uart_TxMsg.ucTxCnt = 0U;/*清发送计数值*/
				Uart_TxMsg.ucTxFlg = 0U;/*清发送标志*/
				DISABLE_TIEN;/* 禁止发送寄存器空中断 */
				//DISABLE_TEN;  /* 禁止发送 */
				//ENABLE_RIEN; /* 接收中断请求 */
				//ENABLE_REN; /* 允许接收 */	
			}
			else
			{}
		}  		
	}
}


/******************************************************
*函数名：UartCmn_u_RxMsgBuff

*形  参：

*返回值：

*描  述：接收数据缓存

*备  注：
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
*函数名：GetUartCmn_u_RxMsgRenew

*形  参：

*返回值：

*描  述：获取接收数据更新标志

*备  注：9600波特率，10ms调度周期；115200波特率，5ms调度周期
******************************************************/
uint8 GetUartCmn_u_RxMsgRenew(void)
{
	uint8 LeUartCmn_u_Flg = 0U;
	if(Uart_RecMsg.UartNewFlag == 1U)/*接收数据缓存区更新*/
	{
		Uart_RecMsg.UartNewFlag = 0U;
		if(1U == GetUartCmn_u_XOR())/*校验通过*/
		{
			LeUartCmn_u_Flg =1U;/*接收数据更新标志置有效*/
		}
	}
	return LeUartCmn_u_Flg;
}


/******************************************************
*函数名：GetUartCmn_u_XOR

*形  参：

*返回值：

*描  述：异或取反校验

*备  注：
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
	if(LeUartCmn_u_Xor == SaUartCmn_u_RxBuff[60])/*校验通过*/
	{
		LeUartCmn_u_ok = 1U;
	}
	return LeUartCmn_u_ok;
}

/******************************************************
*函数名：GetUartCmn_u_RxMsg

*形  参：

*返回值：

*描  述：获取接收数据

*备  注：
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
*函数名：GetUartCmn_u_TxIdle

*形  参：

*返回值： 1--空闲；0--忙

*描  述：获取发送是否空闲:1--空闲；0--忙

*备  注：
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
*函数名：GetUartCmn_u_TxIdle

*形  参：

*返回值： 1--空闲；0--忙

*描  述：获取数据接收是否空闲:1--空闲；0--忙

*备  注：
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
*函数名：SetUartCmn_TxMsg

*形  参：LeUartCmn_u_TxData[]   发送数据
		LeUartCmn_u_Lng		   发送数据帧长度

*返回值：

*描  述：设置发送报文并启动发送

*备  注：
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
	Uart_TxMsg.ucTxFlg = 1U;/*置发送标志有效*/
	//DISABLE_RIEN;/* 关闭接收中断请求 */
	//DISABLE_REN; /* 禁止接收 */
	ENABLE_TEN; /* 允许发送 */
	ENABLE_TIEN;/* 使能发送寄存器空中断 */		
}


/******************************************************
*函数名：Uart_Send

*形  参：void

*返回值：void

*描  述：

*备  注：
******************************************************/
void Uart_Send(uint8_t* Data,uint8_t Length)
{
	uint8_t LeUart_u_Len;
	DISABLE_RIEN;/* 关闭接收中断请求 */
	DISABLE_REN; /* 禁止接收 */
    ENABLE_TEN;
	Uart_TxMsg.ucTxFlg = 1U;/*置发送标志有效*/
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
	Uart_TxMsg.ucTxFlg = 0U;/*置发送标志有效*/
	
    DISABLE_TEN;
	ENABLE_RIEN; /* 接收中断请求 */
	ENABLE_REN; /* 允许接收 */
}

/******************************************************
*函数名：Uart_PrintStr

*形  参：void

*返回值：void

*描  述：打印字符串

*备  注：
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
*函数名：SetUartCmn_BluetoothTxMsg

*形  参：LeUartCmn_u_TxData[]   发送数据
		LeUartCmn_u_Lng		   发送数据帧长度

*返回值：

*描  述：设置发送报文并启动发送(蓝牙模块)

*备  注：
******************************************************/
void SetUartCmn_BluetoothTxMsg(char* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng)
{
	uint8 LeUartCmn_u_Index;
	for(LeUartCmn_u_Index = 0U;LeUartCmn_u_Index < LeUartCmn_u_Lng;LeUartCmn_u_Index++)
	{
		Uart_BluetoothTxMsg.ucTxData[LeUartCmn_u_Index] = LeUartCmn_u_TxData[LeUartCmn_u_Index];
	}
	Uart_BluetoothTxMsg.ucLength = LeUartCmn_u_Lng;
	Uart_BluetoothTxMsg.ucTxFlg = 1U;/*置发送标志有效*/
	//USART_SendData(UART5,Uart_BluetoothTxMsg.ucTxData[Uart_BluetoothTxMsg.ucTxCnt]);/* 发送数据场 */
	//Uart_BluetoothTxMsg.ucTxCnt++;	
	//USART_ITConfig(UART5_USART, USART_IT_RXNE, DISABLE);/* 关闭接收中断请求 */
	//(UART5->CR1 &= (uint8_t)(~USART_Mode_Rx));/* 禁止接收 */
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);/* 使能发送寄存器空中断 */
	//(UART5->CR1 |= (uint8_t)USART_Mode_Tx);/* 允许发送 */
	//DISABLE_RIEN;/* 关闭接收中断请求 */
	//DISABLE_REN; /* 禁止接收 */
	//ENABLE_TEN; /* 允许发送 */
	//ENABLE_TIEN;/* 使能发送寄存器空中断 */		
}

/******************************************************
*函数名：UartCmn_Tx_BluetoothMsg

*形  参：

*返回值：

*描  述：串口发送(蓝牙模块)

*备  注：
******************************************************/
void UartCmn_Tx_BluetoothMsg(void)
{
	//uint32 Le_dw_Timer = 0x00ffffff; 
	if(1U == Uart_BluetoothTxMsg.ucTxFlg)
	{
		//if(USART_GetFlagStatus(UART5,USART_FLAG_TXE))/*发送寄存器空*/
		{
			if(Uart_BluetoothTxMsg.ucTxCnt < Uart_BluetoothTxMsg.ucLength)
			{    
				USART_SendData(UART5,Uart_BluetoothTxMsg.ucTxData[Uart_BluetoothTxMsg.ucTxCnt]);/* 发送数据场 */
				Uart_BluetoothTxMsg.ucTxCnt++;		
			}
			else if(Uart_BluetoothTxMsg.ucTxCnt >= Uart_BluetoothTxMsg.ucLength)
			{
				//while(!(USART_GetFlagStatus(UART5,UARTCMN_IT_TC)) && (Le_dw_Timer--));
				Uart_BluetoothTxMsg.ucTxCnt = 0U;/*清发送计数值*/
				Uart_BluetoothTxMsg.ucTxFlg = 0U;/*清发送标志*/
				USART_ITConfig(UART5_USART, USART_IT_TXE, DISABLE);/* 禁止发送寄存器空中断 */	
				//(UART5->CR1 &= (uint8_t)(~USART_Mode_Tx));/* 禁止发送 */
				//USART_ITConfig(UART5_USART, USART_IT_RXNE, ENABLE);/* 接收中断请求 */
				//(UART5->CR1 |= (uint8_t)USART_Mode_Rx); /* 允许接收 */	
				//DISABLE_TIEN;/* 禁止发送寄存器空中断 */
				//DISABLE_TEN;  /* 禁止发送 */
				//ENABLE_RIEN; /* 接收中断请求 */
				//ENABLE_REN; /* 允许接收 */	
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
	if(USART_GetFlagStatus(UART5,UARTCMN_FLAG_OE)  || USART_GetFlagStatus(UART5,UARTCMN_FLAG_FE))/* 帧错误标志位 或者 数据溢出标志位 */
	{
		Buff = USART_ReceiveData(UART5);/* 清除Break段数据及帧错误数据 */ 
		blenum = 0;
	}				
	else if (USART_GetFlagStatus(UART5,UARTCMN_FLAG_RXNE))/* 接受数据寄存器满标志位 */
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
				else if(Buff == 'A')//准备接收MAC地址(hm -11)
				{
					BleTemp[blenum] = Buff;
					blenum++;
					RcvFlag = 12U;
				}
				else if(Buff == 'G')//准备接收MAC地址(hm -13)
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
					case 'A'://蓝牙扫描：准备接收有效数据(hm-11)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 7U;
					}
					break;
					case 'C'://蓝牙扫描：准备接收开始或结束数据帧(hm-11)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 11U;
					}
					break;
					case 'B'://蓝牙扫描：准备接收开始或结束数据帧(hm-13)
					{
						BleTemp[blenum] = Buff;
						blenum++;
						RcvFlag = 11U;
					}
					break;
					case '0'://蓝牙扫描：准备接收有效数据(hm-13)
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
				if(Buff == ':')//有效数据头接收完成
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
			case 8U://接收P1~P4,9 Byte(hm-11);接收P2~P4, 19 Byte(hm-13);
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
					{//接收溢出错误
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
						{//接收溢出错误
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
			case 9U://接收Reset DATA
			{
				BleTemp[blenum] = Buff;
				blenum++;
#ifdef HM_11
				if(blenum == (17+BleTemp[16]))//蓝牙扫描数据接收完成(hm-11)
				{
					RcvFlag = 10U;
				}
#else
				if(blenum == (27+BleTemp[26]))//蓝牙扫描数据接收完成(hm-13)
				{
					RcvFlag = 10U;
				}				
#endif
			}
			break;
			case 10U://接收完成处理（一帧数据以'\n'(0d 0a)结束，接收到'\n'后对一帧数据处理）
			{
#ifdef HM_11
				switch(BleTemp[16])
				{
					case 0x1A://安卓接收临时密码
					{
						if((BleTemp[19] == 0x57) && (BleTemp[20] == 0x5A))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[21+i];//门口机密码
								BleTempPassword[i + 6] = BleTemp[37+i];//围墙机密码
							}
							ble_flag = 1;
						}
						else
						{}
					}
					break;
					case 0x0F://安卓接收手机号
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
					case 0x18://安卓校时
					{
						if((BleTemp[19] == 0x49) && (BleTemp[20] == 0x54))
						{
							strncpy(timelist,BleTemp+21,14);
							set_time_flag = 1;
						}
					}						
					case 0x08://苹果IOS接收临时密码
					{
						if((BleTemp[17] == 0x07) && (BleTemp[18] == 0x09))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[19+i];//门口机密码
								BleTempPassword[i + 6] = BleTemp[19+i];//围墙机密码
							}
							ble_flag = 1;
						}
					}
					break;
					case 0x12://苹果IOS校时
					{
						if((BleTemp[19] == 0x54) && (BleTemp[20] == 0x49))
						{
							strncpy(timelist,BleTemp+21,14);
							set_time_flag = 1;
						}
					}
					break;
					case 0x1E://苹果IOS接收手机号
					{
						if((BleTemp[26] == 0x00) && (BleTemp[27] == 0x0A))
						{
							if(BleTemp[30] == 0x52)
							{//表示接收到手机号码数字1,有效的手机号
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
					case 0x1A://安卓接收临时密码
					{
						if((BleTemp[29] == 0x57) && (BleTemp[30] == 0x5A))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[31+i];//门口机密码
								BleTempPassword[i + 6] = BleTemp[47+i];//围墙机密码
							}
							ble_flag = 1;
						}
						else
						{}
					}
					break;
					case 0x0F://安卓接收手机号
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
					case 0x18://安卓校时
					{
						if((BleTemp[29] == 0x49) && (BleTemp[30] == 0x54))
						{
							strncpy(timelist,BleTemp+31,14);
							set_time_flag = 1;
						}
					}						
					case 0x08://苹果IOS接收临时密码
					{
						if((BleTemp[27] == 0x07) && (BleTemp[28] == 0x09))
						{
							for(i = 0;i < 6;i++)
							{
								BleTempPassword[i] = BleTemp[29+i];//门口机密码
								BleTempPassword[i + 6] = BleTemp[29+i];//围墙机密码
							}
							ble_flag = 1;
						}
					}
					break;
					case 0x12://苹果IOS校时
					{
						if((BleTemp[29] == 0x54) && (BleTemp[30] == 0x49))
						{
							strncpy(timelist,BleTemp+31,14);
							set_time_flag = 1;
						}
					}
					break;
					case 0x1E://苹果IOS接收手机号
					{
						if((BleTemp[36] == 0x00) && (BleTemp[37] == 0x0A))
						{
							if(BleTemp[40] == 0x52)
							{//表示接收到手机号码数字1,有效的手机号
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
					case 'S'://收到开始字符串
					{
						blenum = 0U;
						RcvFlag = 0U;
					}
					break;
					case 'E'://收到结束字符串
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
			case 12U://case 12~case 16接收MAC地址
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
			case 13U://case 12~case 16接收MAC地址
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
			case 14U://case 12~case 16接收MAC地址
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
			case 15U://case 12~case 16接收MAC地址
			{
				if(Buff == ':')
				{//准备接收MAC地址
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
			case 16U://接收6Byte MAC地址
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
