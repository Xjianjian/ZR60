/******************************************************
�ļ�����	PwUnlock_cfg.c

������		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "PasswordUnlock/PwUnlock_Cfg.h"


/*******************************************************
description�� global variable definitions
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
description�� static variable definitions
*******************************************************/


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*��������PwUnlockCfg_WrEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����дFlash
 
*��  ע��
******************************************************/
void PwUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*��������PwUnlockCfg_ReadEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
******************************************************/
uint8 PwUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	return MemIf_ReadEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}


/******************************************************
*��������GetPwUnlockCfg_u_CheckSum
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡ����0����У��ͣ����ĸ����Ϣ��
 
*��  ע��
******************************************************/
uint8 GetPwUnlockCfg_u_CheckSum(uint8 Le_u_Obj)
{
	return GetMemIf_u_CheckSum(Le_u_Obj);
}

/******************************************************
*��������GetPwUnlockCfg_u_DtVild
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡ������Ч�Ա�־
 
*��  ע��
******************************************************/
uint8 GetPwUnlockCfg_u_DtVild(uint8 Le_u_Obj)
{
	return GetMemIf_u_DtVild(Le_u_Obj);
}

/******************************************************
*��������SetPwUnlockCfg_EEVild
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������λ������Ч�Ա�־
 
*��  ע��
******************************************************/
void SetPwUnlockCfg_EEVild(uint8 Le_u_Obj)
{
	(void)SetMemIf_EEVild( Le_u_Obj);
}

/******************************************************
*��������PwUnlockCfg_u_machineType

*��  �Σ�

*����ֵ��

*��  ������ȡ�豸����

*��  ע��
******************************************************/
uint8 PwUnlockCfg_u_machineType(void)
{
	return GetIcUnlock_u_MachineType();
}

/******************************************************
*��������GetPwUnlockCfg_u_cardSetKeySt
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������ȡĸ�����ð���״̬
 
*��  ע��
******************************************************/
uint8 GetPwUnlockCfg_u_cardSetKeySt(void)
{
	uint8 ret;
	
	ret = GetBtnFltr_u_BtnShrtSt(Key_card_set);
	ClrBtnFltr_BtnShrtValid(Key_card_set);
	
	return ret;
}

/******************************************************
*��������PwUnlockCfg_u_RecordUnlockLog
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������¼����log
 
*��  ע��
******************************************************/
void PwUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
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
*��������PwUnlockCfg_ReadKeyValue

*��  �Σ�

*����ֵ��

*��  ������ȡ����ֵ

*��  ע��
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
*��������PwUnlockCfg_datetime

*��  �Σ�

*����ֵ��

*��  ������ȡʱ��оƬʱ��

*��  ע��
******************************************************/
void PwUnlockCfg_datetime(PwUnlockCfg_rtcTime* time,uint32* timestamp)
{
	GetLocalRTC_datetime((struct rtc_time*)time,timestamp);	
}

/******************************************************
*��������SetPwUnlockCfg_openDoor
*��  �Σ�
		
*����ֵ��
*��  �����·���������
*��  ע��
******************************************************/
void SetPwUnlockCfg_openDoor(void)
{
	DoorLockCtrl_OpenDoor();
}

/******************************************************
*��������PwUnlockCfg_PasswordAuth

*��  �Σ�

*����ֵ��

*��  ����sm3������֤

*��  ע��
******************************************************/
uint8 PwUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password)
{
	return SM3Auth_PasswordAuth(timestamp,machine_type,Password,PASSWORD_TEMPORARY);;
}

/******************************************************
*��������PwUnlockCfg_Play

*��  �Σ�

*����ֵ��

*��  ����������Ƶ����

*��  ע��
******************************************************/
void PwUnlockCfg_Play(uint8 playfile)
{
	//SetAudioIO_PlayFile(playfile);
}
