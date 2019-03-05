/******************************************************
�ļ�����	BleUnlock_Cfg.c

������		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "BluetoothUnlock/BleUnlock_Cfg.h"


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
*��������GetBleUnlockCfg_u_CheckSum
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡ��Ч����У���
 
*��  ע��
******************************************************/
uint8 GetBleUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum( Le_u_Obj);
}

/******************************************************
*��������BleUnlockCfg_WrEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����дFlash
 
*��  ע��
******************************************************/
void BleUnlockCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*��������BleUnlockCfg_RdEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
******************************************************/
uint8 BleUnlockCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE(Le_u_Obj,Le_u_Data,Le_w_Lng);
}


/******************************************************
*��������GetBListMngCfg_EEIdle
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
******************************************************/
uint8 GetBListMngCfg_EEIdle(void)
{
	uint8 ret = 0U;
	if((1U == GetMemIf_u_Idle()) && (1U == MemIfCfg_FLASH_IDLE()))
	{
		ret = 1U;
	}
	return ret;
}

/******************************************************
*��������GetBleUnlockCfg_u_DtVild

*��  �Σ�

*����ֵ��

*��  ������ȡ����������Ч��

*��  ע��
******************************************************/
uint8 GetBleUnlockCfg_u_DtVild(uint8 Obj)
{

	return GetMemIf_u_DtVild(Obj);
}

/******************************************************
*��������GetMemIf_w_DtLng

*��  �Σ�

*����ֵ��

*��  ������ȡ������Ч���ݳ���

*��  ע��
******************************************************/
uint16 GetBListMngCfg_w_DtLng(uint8 Obj)
{
	return GetMemIf_w_DtLng(Obj);
}

/******************************************************
*��������SetBleUnlockCfg_EEVild

*��  �Σ�

*����ֵ��

*��  ������������������Ч

*��  ע��
******************************************************/
void SetBleUnlockCfg_EEVild(uint8 Obj)
{
	//SetMemIf_EEVild(Obj);
}



/******************************************************
*��������BleUnlockCfg_BleTxMsg

*��  �Σ�

*����ֵ��

*��  ������������ģ������

*��  ע��
******************************************************/
void BleUnlockCfg_BleTxMsg(uint8 ReqType)
{
	switch(ReqType)
	{
		case BleUnlock_MAC_ADDR:
		{
			if(HAL_UART_Transmit_DMA(&huart5,"AT+ADDB?",strlen("AT+ADDB?")) != HAL_OK)
			{
				//Error_Handler();
			}
			//SetUartCmn_BluetoothTxMsg("AT+ADDE?",strlen("AT+ADDE?"));//��ȡ����ģ��MAC��ַ(hm-13)	
		}
		break;
		case BleUnlock_DISA:
		{
			if(HAL_UART_Transmit_DMA(&huart5,"AT+DISA?",strlen("AT+DISA?")) != HAL_OK)
			{
				//Error_Handler();
			}
			//SetUartCmn_BluetoothTxMsg("AT+DISA?",strlen("AT+DISA?"));//������ģ�鷢��ɨ��ָ��
		}
		break;
		default:
		break;
	}
}


/******************************************************
*��������BleUnlockCfg_UnlockRecord

*��  �Σ�

*����ֵ��

*��  ������¼������Ϣ

*��  ע��
******************************************************/
void BleUnlockCfg_UnlockRecord(char* Le_u_UnlockInfo)
{
	uint64_t Time = 0U;
#ifdef  HYM8563
	//hym8563_Currenttime(&Time);
#else
	//RTC_Currenttime(&Time);
#endif
	//WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,UNLOCKLOGCACHE_BLE);//������־��¼д�뻺�����
}


/******************************************************
*��������BleUnlockCfg_timeCali

*��  �Σ�

*����ֵ��

*��  ����У׼ʱ��оƬʱ��

*��  ע��
******************************************************/
void BleUnlockCfg_timeCali(char* time)
{
	//timestamp_timeCalibration(time,TIME_STAMP_BJT)//У׼ʱ��оƬʱ��
}

/******************************************************
*��������BleUnlockCfg_u_NetUnlockSt

*��  �Σ�

*����ֵ��

*��  ������ȡ���翪��״̬

*��  ע��
******************************************************/
uint8 BleUnlockCfg_u_NetUnlockSt(void)
{
	return 0;//
}

/******************************************************
*��������BleUnlockCfg_datetime

*��  �Σ�

*����ֵ��

*��  ������ȡʱ��оƬʱ��

*��  ע��
******************************************************/
void BleUnlockCfg_datetime(BleUnlock_rtcTime* time,uint32* timestamp)
{
	GetLocalRTC_datetime((struct rtc_time*)time,timestamp);	
}

/******************************************************
*��������BleUnlockCfg_u_machineType

*��  �Σ�

*����ֵ��

*��  ������ȡ�豸����

*��  ע��
******************************************************/
uint8 BleUnlockCfg_u_machineType(void)
{
	return GetIcUnlock_u_MachineType();
}


/******************************************************
*��������BleUnlockCfg_u_PasswordAuth

*��  �Σ�

*����ֵ��

*��  ����sm3������֤

*��  ע��
******************************************************/
uint8 BleUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password)
{
	return SM3Auth_PasswordAuth(timestamp,machine_type,Password,PASSWORD_BLE);
}

/******************************************************
*��������BleUnlockCfg_OpenDoor

*��  �Σ�

*����ֵ��

*��  �����·���������

*��  ע��
******************************************************/
void BleUnlockCfg_OpenDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*��������GetBleUnlockCfg_u_DoorSt

*��  �Σ�

*����ֵ��

*��  ������ȡ����״̬

*��  ע��
******************************************************/
uint8 GetBleUnlockCfg_u_DoorSt(void)
{
	return 1;
}


/******************************************************
*��������BleUnlockCfg_Play

*��  �Σ�

*����ֵ��

*��  ����������Ƶ����

*��  ע��
******************************************************/
void BleUnlockCfg_Play(uint8 playfile)
{
	//SetAudioIO_PlayFile(playfile);
}
