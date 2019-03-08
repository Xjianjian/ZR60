/******************************************************
�ļ�����	PwUnlock

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "PasswordUnlock/PwUnlock.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint8  keyValue_buffer[12];//���津��������Ϣ
static uint8  u_TouchKeyScanflag = 1;//�����ȡ��������ֵ��־
static uint8  key_num = 0;//��ȡ��������ֵ����

static uint8  SeCardSet_u_PasswordKeySt = 0U;//���밴��״̬
static uint8  SeCardSet_u_BtnFltrFlg = 0U;//��Ч�����˳���־
static uint16 SeCardSet_w_ClrTimer = 0U;//�尴��ֵ����buff��ʱ��


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static char  ZR60Ctrl_TonesPlay(uint16 temp);

static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);

/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitZR60Ctrl_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitPwUnlock_parameter(void)
{
	
}

/******************************************************
*��������TskZR60Ctrl_MainFunction

*��  �Σ�

*����ֵ��

*��  �������ڽ���

*��  ע�����ڽ����жϵ���
******************************************************/
void TskPwUnlock_MainFunction(void)
{
	uint8  rcvbuf[2]={0x00};
	uint8  machine_type;
	uint32 timestamp;
	char  u_ActualKeyValue = 0U;//iic��ȡ�ļ�ֵת�����ʵ�ʴ�������ֵ
	uint16 w_iickeyVlaue = 0;
	uint32 Le_dw_password = 0;//��ʱ����
	PwUnlockCfg_rtcTime rtcTime;
	uint8 ret;
	if(u_TouchKeyScanflag == 1)
	{
		if((SeCardSet_u_PasswordKeySt == STD_ACTIVE) &&(0U == SeCardSet_u_BtnFltrFlg))//����
		{
			SeCardSet_u_PasswordKeySt = 0U;
			SeCardSet_w_ClrTimer = 0U;
			ret = PwUnlockCfg_ReadKeyValue(rcvbuf,2);
			if(ret == 0U)
			{
				w_iickeyVlaue = rcvbuf[1];
				w_iickeyVlaue = (w_iickeyVlaue << 8) | rcvbuf[0];
				u_ActualKeyValue = ZR60Ctrl_TonesPlay(w_iickeyVlaue);
				if(u_ActualKeyValue != 0xff)
				{
					SeCardSet_u_BtnFltrFlg = 1U;
					switch(u_ActualKeyValue)
					{
						case 'C'://���� -> ȡ��
						{
							PWUNLOCK_PRINTF_S("\n��ֵ=>ȡ��");
							key_num = 0;
							memset(keyValue_buffer,0,sizeof(keyValue_buffer));	
						}
						break;
						case 'A'://���� -> ȷ��
						{
							PWUNLOCK_PRINTF_S("\n��ֵ=>ȷ��");
							if(key_num == 6U)
							{//��ǰΪ��ʱ���뿪��
								PwUnlockCfg_datetime(&rtcTime,&timestamp);
								PWUNLOCK_PRINTF_DATE("\n��ȡ��ʱ�䣺%d.%d.%d %d:%d:%d\n",rtcTime.tm_year, \
								rtcTime.tm_mon, rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
								machine_type = PwUnlockCfg_u_machineType();
								Le_dw_password = (keyValue_buffer[0] - 0x30)*100000 + (keyValue_buffer[1] - 0x30)*10000 + \
												 (keyValue_buffer[2] - 0x30)*1000 + (keyValue_buffer[3] - 0x30)*100 + \
												 (keyValue_buffer[4] - 0x30)*10 + (keyValue_buffer[5] - 0x30);
								if(PwUnlockCfg_PasswordAuth(timestamp,machine_type,Le_dw_password) == 1)
								{									
									PWUNLOCK_PRINTF_S("\n��ʱ���� -> ����");
									SetPwUnlockCfg_openDoor();
									PwUnlockCfg_Play(PW_UNLOCK_DOOROPEN);
									
									/*��¼������Ϣ*/
									ZR60Ctrl_u_RecordUnlockInfo("unknown",PW_UNLOCK_LOGCACHE_CARD);	
								}
								else
								{
									PwUnlockCfg_Play(PW_UNLOCK_ERRKEY);
								}
								key_num = 0;
								memset(keyValue_buffer,0,sizeof(keyValue_buffer));
							}
							else if(key_num == 9U)
							{//��ǰΪ�̼�ˢ��
								if(strcmp(keyValue_buffer,"110911060")==0)
								{
									PWUNLOCK_PRINTF_S("\n��ǰ������������");
									//SeCardSet_u_MasterCtrlIap = 1U;//��������������־
									
								}
								else if(strcmp(keyValue_buffer,"110911061")==0)
								{
									PWUNLOCK_PRINTF_S("\n����������");
									//SeCardSet_u_CardReaderIap = 1U;//������ģ������������־
									
								}
								else
								{
									PwUnlockCfg_Play(PW_UNLOCK_ERRKEY);
								}
							}
							else
							{
								PwUnlockCfg_Play(PW_UNLOCK_ERRKEY);
							}
							key_num = 0;
							memset(keyValue_buffer,0,sizeof(keyValue_buffer));											 
						}
						break;
						default:
						{
							PWUNLOCK_PRINTF_D("\n��ֵ %d",(u_ActualKeyValue -0x30));	
							if(key_num < sizeof(keyValue_buffer))
							{
								keyValue_buffer[key_num] = u_ActualKeyValue;
								key_num++;
							}
							else
							{
								key_num = 0;
								memset(keyValue_buffer,0,sizeof(keyValue_buffer));
								PwUnlockCfg_Play(PW_UNLOCK_ERRKEY);
							}
						}
						break;
						
					}
				}
			}
			else
			{
				PWUNLOCK_PRINTF_D("\nErrorLogging����ȡ����ֵʧ��,����ֵ��%d",ret);
			}
		}
		else
		{
			if(SeCardSet_u_PasswordKeySt == STD_ACTIVE)
			{
				//PWUNLOCK_PRINTF_S("\n��Ч�����ж��˳�");
				SeCardSet_u_BtnFltrFlg = 0U;
				SeCardSet_u_PasswordKeySt = 0U;
			}
		}
	}
	else
	{
		SeCardSet_u_PasswordKeySt = 0U;
	}

	
	if(SeCardSet_w_ClrTimer >= (12000/PW_UNLOCK_SCHEDULING_CYCLE))//10ms�������ڣ����12s
	{
		key_num = 0;
		memset(keyValue_buffer,0,sizeof(keyValue_buffer));
	}
	else
	{
		SeCardSet_w_ClrTimer++;
	}	
	
}

/******************************************************
*��������SetZR60Ctrl_PasswordKey

*��  �Σ�

*����ֵ��

*��  ��������password ����״̬

*��  ע��
******************************************************/
void SetZR60Ctrl_PasswordKey(void)
{
	SeCardSet_u_PasswordKeySt = STD_ACTIVE;
}

/******************************************************
*��������return_data

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
static char  ZR60Ctrl_TonesPlay(uint16 temp)
{
	uint8 Le_u_i;
	for(Le_u_i = 0;Le_u_i < PW_UNLOCK_KEYNUM;Le_u_i++)
	{
		if(temp == PwUnlock_Key[Le_u_i].RegValue)
		{
			PwUnlockCfg_Play(PW_UNLOCK_KEYVOICE);
			return PwUnlock_Key[Le_u_i].keyValue;
		}
	}

	return 0xff;
}




/*
	��¼������Ϣ
*/
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{

}

