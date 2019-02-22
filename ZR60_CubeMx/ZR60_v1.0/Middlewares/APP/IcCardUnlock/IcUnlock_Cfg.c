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
	//(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
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
	return 1;//MemIf_ReadEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
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
	return 1;//GetMemIf_u_CheckSum(Le_u_Obj);
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
	return 1;//GetMemIf_u_DtVild(Le_u_Obj);
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
	//(void)SetMemIf_EEVild( Le_u_Obj);
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
	return 0;//GetBtnFltr_u_BtnShrtSt(Key_card_set);
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
	return 0;
}


/******************************************************
*��������SetIcUnlockCfg_TxMsg
*��  �Σ�LeUartCmn_u_TxData[]   ��������
		LeUartCmn_u_Lng		   ��������֡����
*����ֵ��
*��  �������÷��ͱ��Ĳ���������
*��  ע��
******************************************************/
void SetIcUnlockCfg_TxMsg(const uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng)
{
	//SetUartCmn_TxMsg(LeUartCmn_u_TxData,LeUartCmn_u_Lng);
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

}

/******************************************************
*��������IcUnlockCfg_CardRenewCallback
*��  �Σ�
		
*����ֵ��
*��  ����ĸ����Ϣ���»ص�����
*��  ע��
******************************************************/
void IcUnlockCfg_CardRenewCallback(IcUnlock_CardConf* CardConf)
{
	//read_init_sminfo(CardConf)//��ȡĸ���ļ������ݣ�����ȡic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
	tcp_LngConnect_Parameter();
	//Settcp_client_DeviceInit();
}

