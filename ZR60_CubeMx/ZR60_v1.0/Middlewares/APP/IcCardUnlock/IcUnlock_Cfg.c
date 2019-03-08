/******************************************************
文件名：	IcUnlock_cfg.c

描述：		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "IcCardUnlock/IcUnlock_cfg.h"


/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*函数名：IcUnlockCfg_WrEE
 
*形  参：
 
*返回值：void
 
*描  述：写Flash
 
*备  注：
******************************************************/
void IcUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*函数名：IcUnlockCfg_ReadEE
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
******************************************************/
uint8 IcUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}


/******************************************************
*函数名：GetIcUnlockCfg_u_CheckSum
 
*形  参：
 
*返回值：void
 
*描  述：读取扇区0数据校验和（存放母卡信息）
 
*备  注：
******************************************************/
uint8 GetIcUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum(Le_u_Obj);
}

/******************************************************
*函数名：GetIcUnlockCfg_u_DtVild
 
*形  参：
 
*返回值：void
 
*描  述：读取数据有效性标志
 
*备  注：
******************************************************/
uint8 GetIcUnlockCfg_u_DtVild(uint8 Le_u_Obj)
{
	return GetMemIf_u_DtVild(Le_u_Obj);
}

/******************************************************
*函数名：SetIcUnlockCfg_EEVild
 
*形  参：
 
*返回值：void
 
*描  述：置位数据有效性标志
 
*备  注：
******************************************************/
void SetIcUnlockCfg_EEVild(uint8 Le_u_Obj)
{
	(void)SetMemIf_EEVild( Le_u_Obj);
}

/******************************************************
*函数名：SetIcUnlockCfg_PlayFile
 
*形  参：
 
*返回值：void
 
*描  述：设置音频播放文件
 
*备  注：
******************************************************/
void SetIcUnlockCfg_PlayFile(uint8 Le_u_PlayFile)
{
	//SetAudioIO_PlayFile(Le_u_PlayFile,2U)
}


/******************************************************
*函数名：GetIcUnlockCfg_u_cardSetKeySt
 
*形  参：
 
*返回值：void
 
*描  述：获取母卡设置按键状态
 
*备  注：
******************************************************/
uint8 GetIcUnlockCfg_u_cardSetKeySt(void)
{
	uint8 ret;
	
	ret = GetBtnFltr_u_BtnShrtSt(Key_card_set);
	ClrBtnFltr_BtnShrtValid(Key_card_set);
	
	return ret;
}

/******************************************************
*函数名：IcUnlockCfg_u_RecordUnlockLog
 
*形  参：
 
*返回值：void
 
*描  述：记录开门log
 
*备  注：
******************************************************/
void IcUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{
	//uint64_t Time = 0U;
#ifdef  HYM8563
	//hym8563_Currenttime(&Time);
#else
	//RTC_Currenttime(&Time);
#endif
	//WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,Le_u_UnlockEvent);//开锁日志记录写入缓存队列
}


/******************************************************
*函数名：GetIcUnlockCfg_u_RxMsgRenew

*形  参：

*返回值：

*描  述：获取串口接收数据更新标志

*备  注：
******************************************************/
uint8 GetIcUnlockCfg_u_RxMsgRenew(uint8* Le_u_RxMsg)
{
	uint8 ret = 0;
	uint8 Le_u_i;
	if(1 == uart3_dma_recv_end_flag)
	{
		uart3_dma_recv_end_flag = 0;
		if((uart3_dma_rx_buffer[0] == 0x20) && (uart3_dma_rx_buffer[uart3_dma_rx_len-1] == 0x03))
		{//数据帧首尾字节正确
			if(uart3_dma_rx_buffer[uart3_dma_rx_len -2] == \
					xor_checkSum(&uart3_dma_rx_buffer[1],(uart3_dma_rx_len - 3)))
			{//异或校验通过
				for(Le_u_i = 0;Le_u_i < (uart3_dma_rx_len - 3);Le_u_i++)
				{
					Le_u_RxMsg[Le_u_i] = uart3_dma_rx_buffer[Le_u_i + 1];
				}
				uart3_dma_rx_len = 0;
				ret = 1;
			}
		}
		IC_UNLOCK_PRINTF_S("接收到读卡器串口数据:");
#ifdef IC_UNLOCK_DEBUG
		for(Le_u_i = 0;Le_u_i < uart3_dma_rx_len;Le_u_i++)
		{
			IC_UNLOCK_PRINTF_D("%x\n",uart3_dma_rx_buffer[Le_u_i]);
		}
#endif	
		memset(uart3_dma_rx_buffer,0,UART3_DMA_RX_BUFFER_SIZE);//清0
		HAL_UART_Receive_DMA(&huart3,uart3_dma_rx_buffer,UART3_DMA_RX_BUFFER_SIZE);//重新打开DMA接收		
	}

	return ret;
}


/******************************************************
*函数名：SetIcUnlockCfg_TxMsg
*形  参：LeUartCmn_u_TxData[]   发送数据
		LeUartCmn_u_Lng		   发送数据帧长度
*返回值：
*描  述：设置发送报文并启动发送
*备  注：
******************************************************/
void SetIcUnlockCfg_TxMsg(uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng)
{
	//SetUartCmn_TxMsg(LeUartCmn_u_TxData,LeUartCmn_u_Lng);
	if(HAL_UART_Transmit_DMA(&huart3,LeUartCmn_u_TxData,LeUartCmn_u_Lng)!= HAL_OK)
	{
		//Error_Handler();
	}
}

/******************************************************
*函数名：SetIcUnlockCfg_openDoor
*形  参：
		
*返回值：
*描  述：下发开门请求
*备  注：
******************************************************/
void SetIcUnlockCfg_openDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*函数名：IcUnlockCfg_CardRenewCallback
*形  参：
		
*返回值：
*描  述：母卡信息更新回调函数
*备  注：
******************************************************/
void IcUnlockCfg_CardRenewCallback(IcUnlock_CardConf* CardConf,uint8 machineType)
{
	InitSM3Auth_Paramter(CardConf,machineType);//读取母卡文件的数据，并截取ic卡小区号，楼栋号，单元号的前8位用于sm3密码验证
	tcp_LngConnect_Parameter();
	//Settcp_client_DeviceInit();//设置设备重新初始化
}

