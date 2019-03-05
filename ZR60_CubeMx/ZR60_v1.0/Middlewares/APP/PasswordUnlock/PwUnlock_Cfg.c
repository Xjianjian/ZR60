/******************************************************
文件名：	PwUnlock_cfg.c

描述：		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "PasswordUnlock/PwUnlock_Cfg.h"


/*******************************************************
description： global variable definitions
*******************************************************/
const PwUnlockCfg_Key PwUnlock_Key[PW_UNLOCK_KEYNUM] = 
{
	{0x0001,'6'},
	{0x0002,'9'},
	{0x0004,'A'},
	{0x0008,'0'},
	{0x0010,'3'},
	{0x0020,'2'},
	{0x0040,'1'},
	{0x0080,'4'},
	{0x1000,'7'},
	{0x2000,'C'},
	{0x4000,'8'},
	{0x8000,'5'}
};
/*******************************************************
description： static variable definitions
*******************************************************/


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*函数名：PwUnlockCfg_WrEE
 
*形  参：
 
*返回值：void
 
*描  述：写Flash
 
*备  注：
******************************************************/
void PwUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*函数名：PwUnlockCfg_ReadEE
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
******************************************************/
uint8 PwUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}


/******************************************************
*函数名：GetPwUnlockCfg_u_CheckSum
 
*形  参：
 
*返回值：void
 
*描  述：读取扇区0数据校验和（存放母卡信息）
 
*备  注：
******************************************************/
uint8 GetPwUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum(Le_u_Obj);
}

/******************************************************
*函数名：GetPwUnlockCfg_u_DtVild
 
*形  参：
 
*返回值：void
 
*描  述：读取数据有效性标志
 
*备  注：
******************************************************/
uint8 GetPwUnlockCfg_u_DtVild(uint8 Le_u_Obj)
{
	return GetMemIf_u_DtVild(Le_u_Obj);
}

/******************************************************
*函数名：SetPwUnlockCfg_EEVild
 
*形  参：
 
*返回值：void
 
*描  述：置位数据有效性标志
 
*备  注：
******************************************************/
void SetPwUnlockCfg_EEVild(uint8 Le_u_Obj)
{
	(void)SetMemIf_EEVild( Le_u_Obj);
}

/******************************************************
*函数名：PwUnlockCfg_u_machineType

*形  参：

*返回值：

*描  述：获取设备类型

*备  注：
******************************************************/
uint8 PwUnlockCfg_u_machineType(void)
{
	return GetIcUnlock_u_MachineType();
}

/******************************************************
*函数名：GetPwUnlockCfg_u_cardSetKeySt
 
*形  参：
 
*返回值：void
 
*描  述：获取母卡设置按键状态
 
*备  注：
******************************************************/
uint8 GetPwUnlockCfg_u_cardSetKeySt(void)
{
	uint8 ret;
	
	ret = GetBtnFltr_u_BtnShrtSt(Key_card_set);
	ClrBtnFltr_BtnShrtValid(Key_card_set);
	
	return ret;
}

/******************************************************
*函数名：PwUnlockCfg_u_RecordUnlockLog
 
*形  参：
 
*返回值：void
 
*描  述：记录开门log
 
*备  注：
******************************************************/
void PwUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
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
*函数名：PwUnlockCfg_ReadKeyValue

*形  参：

*返回值：

*描  述：获取按键值

*备  注：
******************************************************/
uint8 PwUnlockCfg_ReadKeyValue(uint8* Le_u_buffer,uint8 bufsize)
{
	uint8 ret = 0;
	//if(HAL_I2C_Mem_Read(&hi2c1,0x6E, 0xAA,I2C_MEMADD_SIZE_8BIT,Le_u_buffer,bufsize,10) == HAL_OK)
	{
		ret = 1;
	}
	return ret;
}


/******************************************************
*函数名：PwUnlockCfg_datetime

*形  参：

*返回值：

*描  述：读取时钟芯片时间

*备  注：
******************************************************/
void PwUnlockCfg_datetime(PwUnlockCfg_rtcTime* time,uint32* timestamp)
{
	GetLocalRTC_datetime((struct rtc_time*)time,timestamp);	
}

/******************************************************
*函数名：SetPwUnlockCfg_openDoor
*形  参：
		
*返回值：
*描  述：下发开门请求
*备  注：
******************************************************/
void SetPwUnlockCfg_openDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*函数名：PwUnlockCfg_PasswordAuth

*形  参：

*返回值：

*描  述：sm3密码验证

*备  注：
******************************************************/
uint8 PwUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password)
{
	return SM3Auth_PasswordAuth(timestamp,machine_type,Password,PASSWORD_TEMPORARY);;
}

/******************************************************
*函数名：PwUnlockCfg_Play

*形  参：

*返回值：

*描  述：设置音频播放

*备  注：
******************************************************/
void PwUnlockCfg_Play(uint8 playfile)
{
	//SetAudioIO_PlayFile(playfile);
}
