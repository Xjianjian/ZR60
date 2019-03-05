/******************************************************
文件名：	BleUnlock_Cfg.c

描述：		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "BluetoothUnlock/BleUnlock_Cfg.h"


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
*函数名：GetBleUnlockCfg_u_CheckSum
 
*形  参：
 
*返回值：void
 
*描  述：读取有效数据校验和
 
*备  注：
******************************************************/
uint8 GetBleUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum( Le_u_Obj);
}

/******************************************************
*函数名：BleUnlockCfg_WrEE
 
*形  参：
 
*返回值：void
 
*描  述：写Flash
 
*备  注：
******************************************************/
void BleUnlockCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*函数名：BleUnlockCfg_RdEE
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
******************************************************/
uint8 BleUnlockCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE(Le_u_Obj,Le_u_Data,Le_w_Lng);
}


/******************************************************
*函数名：GetBListMngCfg_EEIdle
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
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
*函数名：GetBleUnlockCfg_u_DtVild

*形  参：

*返回值：

*描  述：获取扇区数据有效性

*备  注：
******************************************************/
uint8 GetBleUnlockCfg_u_DtVild(uint8 Obj)
{

	return GetMemIf_u_DtVild(Obj);
}

/******************************************************
*函数名：GetMemIf_w_DtLng

*形  参：

*返回值：

*描  述：获取扇区有效数据长度

*备  注：
******************************************************/
uint16 GetBListMngCfg_w_DtLng(uint8 Obj)
{
	return GetMemIf_w_DtLng(Obj);
}

/******************************************************
*函数名：SetBleUnlockCfg_EEVild

*形  参：

*返回值：

*描  述：设置扇区数据有效

*备  注：
******************************************************/
void SetBleUnlockCfg_EEVild(uint8 Obj)
{
	//SetMemIf_EEVild(Obj);
}



/******************************************************
*函数名：BleUnlockCfg_BleTxMsg

*形  参：

*返回值：

*描  述：发送蓝牙模块请求

*备  注：
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
			//SetUartCmn_BluetoothTxMsg("AT+ADDE?",strlen("AT+ADDE?"));//获取蓝牙模块MAC地址(hm-13)	
		}
		break;
		case BleUnlock_DISA:
		{
			if(HAL_UART_Transmit_DMA(&huart5,"AT+DISA?",strlen("AT+DISA?")) != HAL_OK)
			{
				//Error_Handler();
			}
			//SetUartCmn_BluetoothTxMsg("AT+DISA?",strlen("AT+DISA?"));//给蓝牙模块发送扫描指令
		}
		break;
		default:
		break;
	}
}


/******************************************************
*函数名：BleUnlockCfg_UnlockRecord

*形  参：

*返回值：

*描  述：记录开锁信息

*备  注：
******************************************************/
void BleUnlockCfg_UnlockRecord(char* Le_u_UnlockInfo)
{
	uint64_t Time = 0U;
#ifdef  HYM8563
	//hym8563_Currenttime(&Time);
#else
	//RTC_Currenttime(&Time);
#endif
	//WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,UNLOCKLOGCACHE_BLE);//开锁日志记录写入缓存队列
}


/******************************************************
*函数名：BleUnlockCfg_timeCali

*形  参：

*返回值：

*描  述：校准时钟芯片时间

*备  注：
******************************************************/
void BleUnlockCfg_timeCali(char* time)
{
	//timestamp_timeCalibration(time,TIME_STAMP_BJT)//校准时钟芯片时间
}

/******************************************************
*函数名：BleUnlockCfg_u_NetUnlockSt

*形  参：

*返回值：

*描  述：获取网络开门状态

*备  注：
******************************************************/
uint8 BleUnlockCfg_u_NetUnlockSt(void)
{
	return 0;//
}

/******************************************************
*函数名：BleUnlockCfg_datetime

*形  参：

*返回值：

*描  述：读取时钟芯片时间

*备  注：
******************************************************/
void BleUnlockCfg_datetime(BleUnlock_rtcTime* time,uint32* timestamp)
{
	GetLocalRTC_datetime((struct rtc_time*)time,timestamp);	
}

/******************************************************
*函数名：BleUnlockCfg_u_machineType

*形  参：

*返回值：

*描  述：获取设备类型

*备  注：
******************************************************/
uint8 BleUnlockCfg_u_machineType(void)
{
	return GetIcUnlock_u_MachineType();
}


/******************************************************
*函数名：BleUnlockCfg_u_PasswordAuth

*形  参：

*返回值：

*描  述：sm3密码验证

*备  注：
******************************************************/
uint8 BleUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password)
{
	return SM3Auth_PasswordAuth(timestamp,machine_type,Password,PASSWORD_BLE);
}

/******************************************************
*函数名：BleUnlockCfg_OpenDoor

*形  参：

*返回值：

*描  述：下发看门请求

*备  注：
******************************************************/
void BleUnlockCfg_OpenDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*函数名：GetBleUnlockCfg_u_DoorSt

*形  参：

*返回值：

*描  述：获取门锁状态

*备  注：
******************************************************/
uint8 GetBleUnlockCfg_u_DoorSt(void)
{
	return 1;
}


/******************************************************
*函数名：BleUnlockCfg_Play

*形  参：

*返回值：

*描  述：设置音频播放

*备  注：
******************************************************/
void BleUnlockCfg_Play(uint8 playfile)
{
	//SetAudioIO_PlayFile(playfile);
}
