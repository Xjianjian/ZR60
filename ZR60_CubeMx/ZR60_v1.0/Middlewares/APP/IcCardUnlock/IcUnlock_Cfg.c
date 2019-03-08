/******************************************************
�ļ�����	IcUnlock_cfg.c

������		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "IcCardUnlock/IcUnlock_cfg.h"


/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*��������IcUnlockCfg_WrEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����дFlash
 
*��  ע��
******************************************************/
void IcUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*��������IcUnlockCfg_ReadEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
******************************************************/
uint8 IcUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}


/******************************************************
*��������GetIcUnlockCfg_u_CheckSum
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡ����0����У��ͣ����ĸ����Ϣ��
 
*��  ע��
******************************************************/
uint8 GetIcUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum(Le_u_Obj);
}

/******************************************************
*��������GetIcUnlockCfg_u_DtVild
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡ������Ч�Ա�־
 
*��  ע��
******************************************************/
uint8 GetIcUnlockCfg_u_DtVild(uint8 Le_u_Obj)
{
	return GetMemIf_u_DtVild(Le_u_Obj);
}

/******************************************************
*��������SetIcUnlockCfg_EEVild
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������λ������Ч�Ա�־
 
*��  ע��
******************************************************/
void SetIcUnlockCfg_EEVild(uint8 Le_u_Obj)
{
	(void)SetMemIf_EEVild( Le_u_Obj);
}

/******************************************************
*��������SetIcUnlockCfg_PlayFile
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����������Ƶ�����ļ�
 
*��  ע��
******************************************************/
void SetIcUnlockCfg_PlayFile(uint8 Le_u_PlayFile)
{
	//SetAudioIO_PlayFile(Le_u_PlayFile,2U)
}


/******************************************************
*��������GetIcUnlockCfg_u_cardSetKeySt
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡĸ�����ð���״̬
 
*��  ע��
******************************************************/
uint8 GetIcUnlockCfg_u_cardSetKeySt(void)
{
	uint8 ret;
	
	ret = GetBtnFltr_u_BtnShrtSt(Key_card_set);
	ClrBtnFltr_BtnShrtValid(Key_card_set);
	
	return ret;
}

/******************************************************
*��������IcUnlockCfg_u_RecordUnlockLog
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������¼����log
 
*��  ע��
******************************************************/
void IcUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{
	//uint64_t Time = 0U;
#ifdef  HYM8563
	//hym8563_Currenttime(&Time);
#else
	//RTC_Currenttime(&Time);
#endif
	//WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,Le_u_UnlockEvent);//������־��¼д�뻺�����
}


/******************************************************
*��������GetIcUnlockCfg_u_RxMsgRenew

*��  �Σ�

*����ֵ��

*��  ������ȡ���ڽ������ݸ��±�־

*��  ע��
******************************************************/
uint8 GetIcUnlockCfg_u_RxMsgRenew(uint8* Le_u_RxMsg)
{
	uint8 ret = 0;
	uint8 Le_u_i;
	if(1 == uart3_dma_recv_end_flag)
	{
		uart3_dma_recv_end_flag = 0;
		if((uart3_dma_rx_buffer[0] == 0x20) && (uart3_dma_rx_buffer[uart3_dma_rx_len-1] == 0x03))
		{//����֡��β�ֽ���ȷ
			if(uart3_dma_rx_buffer[uart3_dma_rx_len -2] == \
					xor_checkSum(&uart3_dma_rx_buffer[1],(uart3_dma_rx_len - 3)))
			{//���У��ͨ��
				for(Le_u_i = 0;Le_u_i < (uart3_dma_rx_len - 3);Le_u_i++)
				{
					Le_u_RxMsg[Le_u_i] = uart3_dma_rx_buffer[Le_u_i + 1];
				}
				uart3_dma_rx_len = 0;
				ret = 1;
			}
		}
		IC_UNLOCK_PRINTF_S("���յ���������������:");
#ifdef IC_UNLOCK_DEBUG
		for(Le_u_i = 0;Le_u_i < uart3_dma_rx_len;Le_u_i++)
		{
			IC_UNLOCK_PRINTF_D("%x\n",uart3_dma_rx_buffer[Le_u_i]);
		}
#endif	
		memset(uart3_dma_rx_buffer,0,UART3_DMA_RX_BUFFER_SIZE);//��0
		HAL_UART_Receive_DMA(&huart3,uart3_dma_rx_buffer,UART3_DMA_RX_BUFFER_SIZE);//���´�DMA����		
	}

	return ret;
}


/******************************************************
*��������SetIcUnlockCfg_TxMsg
*��  �Σ�LeUartCmn_u_TxData[]   ��������
		LeUartCmn_u_Lng		   ��������֡����
*����ֵ��
*��  �������÷��ͱ��Ĳ���������
*��  ע��
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
*��������SetIcUnlockCfg_openDoor
*��  �Σ�
		
*����ֵ��
*��  �����·���������
*��  ע��
******************************************************/
void SetIcUnlockCfg_openDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*��������IcUnlockCfg_CardRenewCallback
*��  �Σ�
		
*����ֵ��
*��  ����ĸ����Ϣ���»ص�����
*��  ע��
******************************************************/
void IcUnlockCfg_CardRenewCallback(IcUnlock_CardConf* CardConf,uint8 machineType)
{
	InitSM3Auth_Paramter(CardConf,machineType);//��ȡĸ���ļ������ݣ�����ȡic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
	tcp_LngConnect_Parameter();
	//Settcp_client_DeviceInit();//�����豸���³�ʼ��
}

