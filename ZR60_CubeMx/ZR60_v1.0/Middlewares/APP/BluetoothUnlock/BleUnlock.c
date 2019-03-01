/******************************************************
�ļ�����	BleUnlock

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "BluetoothUnlock/BleUnlock.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint8  	set_time_success =0U;//����Уʱ�ɹ���־
static uint32 	SeCardSet_dw_setTimeCnt = 0U;//����Уʱ��ʱ��ʱ��
static uint8  	set_time_flag = 0;//Уʱ�����־
static char   	Se_u_time[15] = {0};//����ʱ��
static uint8 	ble_Unlockflag = 0;//�������������־
static char   	Se_u_password[12] = {0};//������֤����
static uint8	Se_u_PhoneNumFlg = 0;//���յ��ֻ��ű�־
static uint8	Se_u_PhoneNum[12] = {0};//���յ��ֻ���
static uint8 	b_5s_flag = 0;//5s��ʱ��־�����ڹ���5s�ڶ���Ŀ�������ָ�
static uint16 	b_5s_count = 0;//5s��ʱ������
static uint8  	SeZR60_u_BleUnlockflag = 0U;//ָʾ��ǰ����������
static uint8  	SeZR60_u_BleLogFltrflag = 0U;//����������־��¼���˱�־
static uint16 	w_BleNetExcUnlockDelayTimer = 0U;//���������翪�Ź��ܻ�����ʱ��ʱ��
static uint8  	SeCardSet_u_BleSt = 0U;//����״̬��0--���У�1--����ɨ��
static uint16 	SeCardSet_w_BleDisCnntT = 0U;//�����Ͽ���ʱ��ʱ��
static volatile uint8  Se_u_BleMacFlg = 0U;//mac��ַ��Ч��־
static uint8	Se_u_MacAddr[12U] = {0};//mac��ַ

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitBleUnlock_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitBleUnlock_parameter(void)
{
	uint64_t Le_dw_WaitTimer = 0x6FFFFFF;
	uint8 LeBleUnlock_u_Xor = 0;
	BleUnlock_rtcTime	rtcTime;
	uint32 timestamp;
	uint8 LeBleUnlock_u_cnt = 30;
	while(--Le_dw_WaitTimer);//�ȴ�����ģ���ȶ���hm-13˫ģģ���ϵ���Ҫ1.5s�ȶ���
	LeBleUnlock_u_Xor = GetBleUnlockCfg_u_CheckSum(BLEUNLOCK_MAC_INFO);//��ȡmac��ַ����У���
	if((LeBleUnlock_u_Xor == BleUnlockCfg_RdEE(BLEUNLOCK_MAC_INFO,Se_u_MacAddr,sizeof(Se_u_MacAddr))) \
											&& (STD_ACTIVE == GetBleUnlockCfg_u_DtVild(BLEUNLOCK_MAC_INFO)))
	{
		printf("\nflash�洢��mac��ַ��Ч  ��\n");
		SetMemIf_EEVild(BLEUNLOCK_MAC_INFO);//������Ч
		Se_u_BleMacFlg = 1U;
	}
	else
	{//��ȡ����mac��ַ
		while(LeBleUnlock_u_cnt)
		{
			Le_dw_WaitTimer = 0x1FFFFF;
			BleUnlockCfg_BleTxMsg(BleUnlock_MAC_ADDR);//��ȡ����ģ��MAC��ַ
			while((Se_u_BleMacFlg != 1) && (--Le_dw_WaitTimer));//�ȴ���ȡ����mac��ַ
			LeBleUnlock_u_cnt--;
			if(Se_u_BleMacFlg == 1) break;
		}
		
		if(Se_u_BleMacFlg == 1)
		{
			printf("\n��ȡ����mac��ַ�ɹ�\n");
			(void)BleUnlockCfg_WrEE(BLEUNLOCK_MAC_INFO,Se_u_MacAddr,sizeof(Se_u_MacAddr));//mac��ַд��eeprom/flash
		}
		else
		{
			printf("\n��ȡ����mac��ַʧ��  X\n");
			BleUnlockCfg_datetime(&rtcTime,&timestamp);
			BLEUNLOCK_PRINTF_DATE("��ȡ�ı���ʱ�䣺%d.%d.%d %d:%d:%d\n",rtcTime.tm_year,rtcTime.tm_mon, \
							  rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
			/*����rtcʱ��ʱ��ת��MAC��ַ*/
			Se_u_MacAddr[0] = (rtcTime.tm_year%100)/10;
			Se_u_MacAddr[1] = (rtcTime.tm_year%100)%10;
			Se_u_MacAddr[2] = rtcTime.tm_mon/10;
			Se_u_MacAddr[3] = rtcTime.tm_mon%10;
			Se_u_MacAddr[4] = rtcTime.tm_mday/10;
			Se_u_MacAddr[5] = rtcTime.tm_mday%10;	
			Se_u_MacAddr[6] = rtcTime.tm_hour/10;
			Se_u_MacAddr[7] = rtcTime.tm_hour%10;
			Se_u_MacAddr[8] = rtcTime.tm_min/10;
			Se_u_MacAddr[9] = rtcTime.tm_min%10;
			Se_u_MacAddr[10] = rtcTime.tm_sec/10;
			Se_u_MacAddr[11] = rtcTime.tm_sec%10;
		}
	}
	SetEthernetif_macAddr(Se_u_MacAddr);
}

/******************************************************
*��������TskBleUnlock_MainFunction

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void TskBleUnlock_MainFunction(void)
{ 
	uint16 Le_w_i;
	BleUnlock_rtcTime rtcTime;
	char Le_u_UnlockInfo[32U] = {0};
	uint32 timestamp;
	uint8  machine_type;
	uint32 password;
	static uint32 Ble_openDoor_cnt = 0;

	if(SeCardSet_u_BleSt == 0U)//����ģ�����
	{
		SeCardSet_u_BleSt = 1U;
		BleUnlockCfg_BleTxMsg(BleUnlock_DISA);//������ģ�鷢��ɨ��ָ��
		SeCardSet_w_BleDisCnntT = 0U;
	}
	
	if(1U == SeCardSet_u_BleSt)//����ģ��ǿ���ʱ����ʱ8s(������������ͨ�ų�ʱ����)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (8000/BLEUNLOCK_SCHEDULING_CYCLE))
		{//ɨ��ָ�����8s��ʱʱ���ڶ�δ�յ�ɨ�����ָ���������ͨ�ų�ʱ
			SeCardSet_w_BleDisCnntT = 0U;
			SeCardSet_u_BleSt = 0U;
		}
	}
	else
	{
		SeCardSet_w_BleDisCnntT = 0U;
	}
	
	/**��������**/
	if((ble_Unlockflag == 1) && (0U == b_5s_flag))
	{
		b_5s_flag = 1U;//5s��ʱ��־�����ڼ���Խ��յ��Ŀ���ָ��
		b_5s_count = 0U;//5s��ʱ��ʱ��
		ble_Unlockflag = 0;
		if(BleUnlockCfg_u_NetUnlockSt() != 1U)//��ʾһ��ʱ���������翪��
		{
			BleUnlockCfg_datetime(&rtcTime,&timestamp);
			BLEUNLOCK_PRINTF_DATE("��ȡ��ʱ�䣺%d.%d.%d %d:%d:%d\n",rtcTime.tm_year,rtcTime.tm_mon, \
							  rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
			machine_type = BleUnlockCfg_u_machineType();
			if(BleUnlockCfg_PasswordAuth(timestamp,machine_type,password) == 1)
			{//��ʾ�յ�����ȶԳɹ�
				Ble_openDoor_cnt++;
				BLEUNLOCK_PRINTF_D("���� -> ���� %d ��\r\n",Ble_openDoor_cnt);
				BleUnlockCfg_OpenDoor();

				SeZR60_u_BleUnlockflag = 1U;
				SeZR60_u_BleLogFltrflag = 1U;
				BleUnlockCfg_Play(BLEUNLOCK_DOOROPEN);
			}
			else
			{
				BLEUNLOCK_PRINTF_D("����������֤ʧ�ܣ�����ʱ��:%s\n",Se_u_time);
				BleUnlockCfg_Play(BLEUNLOCK_ERRKEY);				
			}
		}
		else
		{//��⵽���һ��ʱ���ڷ���������Զ�̿��ţ�����������رգ�ִ�п���
			if(0U == GetBleUnlockCfg_u_DoorSt())
			{//�����ر�״̬
				BleUnlockCfg_datetime(&rtcTime,&timestamp);
				BLEUNLOCK_PRINTF_DATE("��ȡ��ʱ�䣺%d.%d.%d %d:%d:%d\n",rtcTime.tm_year, \
					rtcTime.tm_mon, rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
				machine_type = BleUnlockCfg_u_machineType();
				if(BleUnlockCfg_PasswordAuth(timestamp,machine_type,password) == 1)
				{//��ʾ�յ�����ȶԳɹ�		
					Ble_openDoor_cnt++;
					BLEUNLOCK_PRINTF_D("���� -> ���� %d ��\r\n",Ble_openDoor_cnt);
					BleUnlockCfg_OpenDoor();
					SeZR60_u_BleUnlockflag = 1U;
					SeZR60_u_BleLogFltrflag = 1U;
					BleUnlockCfg_Play(BLEUNLOCK_DOOROPEN);				
				}
				else
				{
					BLEUNLOCK_PRINTF_D("����������֤ʧ�ܣ�����ʱ��:%s\n",Se_u_time);
					BleUnlockCfg_Play(BLEUNLOCK_ERRKEY);
				}
			}	
		}
	}
	
	if(1U == b_5s_flag)//��������ʱ����ʱ�����ڴ��������5s�ڽ��յ�����������ָ�
	{
		b_5s_count++;
		if(b_5s_count >= (5000/BLEUNLOCK_SCHEDULING_CYCLE))//5s��ʱ
		{//5s��ʱʱ�䵽����b_5s_flag��־��������������
			b_5s_count = 0U;
			ble_Unlockflag = 0U;//�忪��ָ��
			b_5s_flag = 0U;
		}
	}
	else
	{
		b_5s_count = 0U;
	}

	if(SeZR60_u_BleUnlockflag == 1U)//3s���ڼ���Խ��յ���������������
	{
		if(w_BleNetExcUnlockDelayTimer >= (3000/BLEUNLOCK_SCHEDULING_CYCLE))
		{
			SeZR60_u_BleUnlockflag = 0U;
			w_BleNetExcUnlockDelayTimer = 0U;
		}
		else
		{
			w_BleNetExcUnlockDelayTimer++;
		}
	}
	
	/*�ϴ�����������־*/
	if(1U == SeZR60_u_BleLogFltrflag)
	{
		if(1U == Se_u_PhoneNumFlg)//���յ������ֻ���
		{
			Se_u_PhoneNumFlg = 0U;
			SeZR60_u_BleLogFltrflag = 0U;
			BLEUNLOCK_PRINTF_D("���յ��ֻ��ţ�%s\n",Se_u_PhoneNum);
			char temp = 33;
			for(Le_w_i=0U;Le_w_i < 11U;Le_w_i++)
			{//�ֻ��������
				Le_u_UnlockInfo[Le_w_i] = Se_u_PhoneNum[Le_w_i] - temp;
				temp = temp - 2;
			}
			BLEUNLOCK_PRINTF_D("���յ��ֻ��Ž����%s\n",Le_u_UnlockInfo);
			BleUnlockCfg_UnlockRecord(Le_u_UnlockInfo);
		}
	}
	else
	{
		Se_u_PhoneNumFlg = 0U;
	}
	
	/*��������������ʱ��У׼��־*/
	if(0U == Getntpclient_u_NtpCalTimeSt())
	{//��ntpУʱʱ������Ƿ�������Уʱ����
		if(0 == set_time_success)
		{
			if(set_time_flag == 1)
			{
				set_time_flag = 0;
				BLEUNLOCK_PRINTF_D("����Уʱ������ʱ��:%s\n",Se_u_time);
				BleUnlockCfg_timeCali(Se_u_time);
				set_time_success = 1;
				SeCardSet_dw_setTimeCnt = 0;
			}
		}
		else
		{//����Уʱ��ɺ󣬾����´�Уʱ�ӳ�5s
			SeCardSet_dw_setTimeCnt++;
			if(SeCardSet_dw_setTimeCnt >= (5000/BLEUNLOCK_SCHEDULING_CYCLE))
			{
				set_time_flag = 0;
				set_time_success = 0;
			}
		}
	}
	else
	{
		set_time_flag = 0;
	}
}


/******************************************************
*��������BleUnlock_RxMsgCallback

*��  �Σ�

*����ֵ��

*��  �������ڽ�������֡����ص�����

*��  ע��
******************************************************/
void BleUnlock_RxMsgCallback(char* Le_u_rxMsg,uint8 Le_u_lng)
{
	uint8 Le_u_i;
	//printf(Le_u_rxMsg);
	if(Le_u_rxMsg[0] != 0x4F) return;//���֡ͷ
	if((Le_u_lng != 6U) && (Le_u_lng != 19U) && (Le_u_lng != 8U) && ((Le_u_rxMsg[Le_u_lng -2] != 0x0D) || \
				(Le_u_rxMsg[Le_u_lng -1] != 0x0A)))//���֡β,����֡����Ϊ8�ֽ�,���ܽ��յ�ɨ�迪ʼ����ɨ�����֡
	{
		return;
	}
	
	switch(Le_u_lng)
	{
		case 59://ƻ��IOS�����ֻ���
		case 37://ƻ��IOS������ʱ����
		case 47://ƻ��IOSУʱ
		case 44://��׿�����ֻ�������֡����
		case 53://��׿Уʱ����֡����
		case 55://��׿������ʱ��������֡����
		{
			switch(Le_u_rxMsg[26])
			{
				case 0x1A://��׿������ʱ����
				{
					if((Le_u_rxMsg[29] == 0x57) && (Le_u_rxMsg[30] == 0x5A))
					{
						for(Le_u_i = 0;Le_u_i < 6;Le_u_i++)
						{
							Se_u_password[Le_u_i] = Le_u_rxMsg[31+Le_u_i];//�ſڻ�����
							Se_u_password[Le_u_i + 6] = Le_u_rxMsg[47+Le_u_i];//Χǽ������
						}
						ble_Unlockflag = 1;
					}
					else
					{}
				}
				break;
				case 0x0F://��׿�����ֻ���
				{
					if((Le_u_rxMsg[29] == 0x77) && (Le_u_rxMsg[30] == 0x7A))
					{
						for(Le_u_i = 0;Le_u_i < 11U;Le_u_i++)
						{
							Se_u_PhoneNum[Le_u_i] = Le_u_rxMsg[31+Le_u_i];
						}
						Se_u_PhoneNumFlg = 1;
					}
				}
				break;
				case 0x18://��׿Уʱ
				{
					if((Le_u_rxMsg[29] == 0x49) && (Le_u_rxMsg[30] == 0x54))
					{
						strncpy(Se_u_time,Le_u_rxMsg+31,14);
						set_time_flag = 1;
					}
				}						
				case 0x08://ƻ��IOS������ʱ����
				{
					if((Le_u_rxMsg[27] == 0x07) && (Le_u_rxMsg[28] == 0x09))
					{
						for(Le_u_i = 0;Le_u_i < 6;Le_u_i++)
						{
							Se_u_password[Le_u_i] = Le_u_rxMsg[29+Le_u_i];//�ſڻ�����
							Se_u_password[Le_u_i + 6] = Le_u_rxMsg[29+Le_u_i];//Χǽ������
						}
						ble_Unlockflag = 1;
					}
				}
				break;
				case 0x12://ƻ��IOSУʱ
				{
					if((Le_u_rxMsg[29] == 0x54) && (Le_u_rxMsg[30] == 0x49))
					{
						strncpy(Se_u_time,Le_u_rxMsg+31,14);
						set_time_flag = 1;
					}
				}
				break;
				case 0x1E://ƻ��IOS�����ֻ���
				{
					if((Le_u_rxMsg[36] == 0x00) && (Le_u_rxMsg[37] == 0x0A))
					{
						if(Le_u_rxMsg[40] == 0x52)
						{//��ʾ���յ��ֻ���������1,��Ч���ֻ���
							for(Le_u_i = 0;Le_u_i < 11U;Le_u_i++)
							{
								Se_u_PhoneNum[Le_u_i] = Le_u_rxMsg[40+Le_u_i];
							}
							Se_u_PhoneNumFlg = 1;
						}
					}
				}
				break;
				default:
				break;				 					
			}
		}
		break;
		case 19://MAC��ַ����֡����
		{
			if((Le_u_rxMsg[3] == 'G') && (Le_u_rxMsg[4] == 'e') && \
				(Le_u_rxMsg[5] == 't') && (Le_u_rxMsg[6] == ':'))
			{//���յ�MAC��ַ
				for(Le_u_i = 0;Le_u_i < 12U;Le_u_i++)
				{
					Se_u_MacAddr[Le_u_i] = Le_u_rxMsg[7+Le_u_i];
				}
				BLEUNLOCK_PRINTF_D("\n����MAC��ַ����֡��%s",Se_u_MacAddr);
				Se_u_BleMacFlg = 1;
			}
		}
		break;
		case 6://���յ�ɨ�����֡
		case 8://���յ�ɨ�迪ʼ������ɨ�����֡
		{
			if((Le_u_rxMsg[3] == 'D') && (Le_u_rxMsg[4] == 'I') && \
					(Le_u_rxMsg[5] == 'S'))
			{
				if((Le_u_lng == 6U) || ((Le_u_rxMsg[6] == 'B') && \
											(Le_u_rxMsg[7] == 'E')))//�յ�ɨ�����֡
				{
					//BLEUNLOCK_PRINTF_S("\n���յ�ɨ�����֡");
					SeCardSet_u_BleSt = 0U;//��������ģ�����
				}
				else
				{}
			}
		}
		break;
		default:
		break;
	}
	memset(Le_u_rxMsg,0,UART5_DMA_RX_BUFFER_SIZE);//��0
}


