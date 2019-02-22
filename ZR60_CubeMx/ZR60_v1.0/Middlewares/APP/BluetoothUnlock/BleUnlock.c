/******************************************************
�ļ�����	BleUnlock

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "./BluetoothUnlock/BleUnlock.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
/*
sm3��������ʹ�õ�������Ϣ
Χǽ����community_idΪС��idǰ4�ֽڣ�build_num��cell_numȫF
�ſڻ���community_idΪС��idǰ4�ֽڣ�build_numǰ4�ֽ�Ϊ¥����ţ�����ȫF;cell_numȫF
*/
struct sm3_info sm = {{'F','F','F','F','F','F','F','F'}, \
		{'F','F','F','F','F','F','F','F'},{'F','F','F','F','F','F','F','F'},'\0'};

static uint8_t 	sm3_flag = 0;
static uint8_t  state = 0U;
static uint8_t 	sec_count = 0;
static uint8_t  test_buf[12];//���津��������Ϣ
static uint8_t 	b_5s_flag = 0;//5s��ʱ��־
static uint16_t b_5s_count = 0;//5s��ʱ������
//static uint8_t 	b_err_5s_flag = 0;
//static uint8_t	b_err_5s_count = 0;
static uint8_t  SeZR60_u_DoorLockRelaySt = 0U;//�����̵���״̬״̬��0--�أ�1--��
static uint8_t  SeZR60_u_BleUnlockflag = 0U;//ָʾ��ǰ����������
static uint8_t  SeZR60_u_NetUnlockflag = 0U;//ָʾ��ǰ�����翪��
static uint8_t  SeZR60_u_BleLogFltrflag = 0U;//����������־��¼���˱�־
static uint8_t	Se_u_FireALARM = 0U;//����������־��Ĭ�Ϲر���������
static uint16 	Voltage_value[2U] = {0U};//��ѹֵ
static uint8_t 	Se_u_VoltageDifferencCnt = 0U;//ѹ������������ε�ѹ��С��ĳһ��ѹֵʱ����
static uint16  	Se_w_DoorLockdelayT = 0xffffU;//��ʱ�������õ�ʱ��(��λ��ms)
static uint8_t  Se_u_CloseDoorDelayTimeSetFlag = 0U;//��ʱ����ʱ�����ñ�־
static uint16_t	Se_w_LockDoorTimer = 0U;//��ʱ���ż�ʱ������ʱʱ�䵽�ر�����
//static uint32_t dw_5msflag_T1 = 0U;//5ms��ʱ��
//static uint32_t dw_5msflag_T2 = 0U;//5ms��ʱ��
static uint16_t w_SetAutoCard_DelayTime = 0U;//�����Զ�Ѱ����ʱʱ��
static uint16_t w_500msflag_DelayTime = 0U;//500ms��ʱ��
static uint16_t w_1sflag_DelayTime = 0U;//1s��ʱ��
static uint16_t w_BleNetExcUnlockDelayTimer = 0U;//���������翪�Ź��ܻ�����ʱ��ʱ��
static uint8_t  u_TouchKeyScanflag = 1;//�����ȡ��������ֵ��־
static uint8_t  key_num = 0;//��ȡ��������ֵ����
//static uint8_t  buf[2]={0x20,0x00};
static uint16_t w_iickeyVlaue = 0U;//iic��ȡ�ļ�ֵ
static uint8_t u_ActualKeyValue = 0U;//iic��ȡ�ļ�ֵת�����ʵ�ʴ�������ֵ
static uint8_t  key_flag = 0;//��ʱ���������ɱ�־
static uint8_t  SeCardSet_u_St = 0U;//ĸ������״̬
static uint32_t SeCardSet_dw_ALARMDlyTime = 0U;//������ʱʱ��

static uint8_t  SeCardSet_u_PasswordKeySt = 0U;//���밴��״̬
static uint8_t  SeCardSet_u_BtnFltrFlg = 0U;//��Ч�����˳���־

static uint8_t  set_time_success =0U;//����Уʱ�ɹ���־
static uint32_t SeCardSet_dw_setTimeCnt = 0U;//����Уʱ��ʱ��ʱ��
static uint8_t  SeCardSet_u_BleSt = 0U;//����״̬��0--���У�1--����ɨ��
static uint16_t SeCardSet_w_BleDisCnntT = 0U;//�����Ͽ���ʱ��ʱ��
static uint32_t SeCardSet_dw_nowtime = 0U;
static uint8_t  Se_u_PasswordCmpFlag = 0U;//����ȶԱ�־��0--ʧ�ܣ�1--�ɹ�
struct rtc_time Ve_h_tm;//����

static uint8_t  SeCardSet_u_MasterCtrlIap = 0U;//��������������־
static uint8_t  SeCardSet_u_CardReaderIap = 0U;//������ģ������������־

static uint8_t  SeZR60_u_DoorOpenLedEvent = 0U;//ָʾ���¼���0--�ޣ�1--����һ�ο���ָʾ���¼�

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void  OpenDoor_Ble_MainFunction(void);
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);
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
	uint8_t  Le_u_i;
	uint64_t Le_dw_WaitTimer = 0x6FFFFFF;
	uint8_t LeZR60_u_Xor = 0;
	
	LeZR60_u_Xor = GetMemIf_u_CheckSum(EepromCfg_MacInfo);//��ȡmac��ַ����У���
	if((LeZR60_u_Xor == MemIf_ReadEE(EepromCfg_MacInfo,Se_u_MacAddr,sizeof(Se_u_MacAddr))) && \
	  (STD_ACTIVE == GetMemIf_u_DtVild(EepromCfg_MacInfo)))
	{
		printf("\n��6��flash�洢��mac��ַ��Ч  ��\r\n");
		SetMemIf_EEVild(EepromCfg_MacInfo);//������Ч
		for(Le_u_i = 0U;Le_u_i < sizeof(Se_u_MacAddr);Le_u_i++)
		{
			if(Se_u_MacAddr[Le_u_i] <=9)
			{
				DeviceInit.Blemac[Le_u_i] = Se_u_MacAddr[Le_u_i] + 0x30;
			}
			else
			{
				DeviceInit.Blemac[Le_u_i] = Se_u_MacAddr[Le_u_i] + 0x37;
			}
		}
		Se_u_BleMacFlg = 1U;
	}
	else
	{//��ȡ����mac��ַ
		while(--Le_dw_WaitTimer);//�ȴ�����ģ���ȶ���hm-13˫ģģ���ϵ���Ҫ1.5s�ȶ���
		Le_dw_WaitTimer = 0x1FFFFF;
#ifdef HM_11
		SetUartCmn_BluetoothTxMsg("AT+ADDR?",strlen("AT+ADDR?"));//��ȡ����ģ��MAC��ַ(hm-11)
#else	
		SetUartCmn_BluetoothTxMsg("AT+ADDE?",strlen("AT+ADDE?"));//��ȡ����ģ��MAC��ַ(hm-13)
#endif
		while((Se_u_BleMacFlg != 1) && (--Le_dw_WaitTimer));//�ȴ���ȡ����mac��ַ

		if(Se_u_BleMacFlg == 1)
		{
			printf("��ȡ����mac��ַ�ɹ�\r\n");
			(void)MemIf_WriteEE(EepromCfg_MacInfo,Se_u_MacAddr,sizeof(Se_u_MacAddr));//mac��ַд��eeprom/flash
		}
		else
		{
			printf("��ȡ����mac��ַʧ��  X\r\n");
#ifdef  HYM8563
			if(hym8563_read_datetime(&Ve_h_tm) == 0)
			{
				printf("��ȡ��ǰʱ��ɹ�\r\n");
			}
#else
			Read_RTC_TimeAndDate(&Ve_h_tm);//��ȡ����ʱ��
#endif
		}
	}
}

/******************************************************
*��������TskBleUnlock_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע��
******************************************************/
void TskBleUnlock_MainFunction(void)
{ 
	uint16_t Le_w_i;
	struct rtc_time Le_h_tm;
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
	static uint16_t Ble_openDoor_cnt = 0;
	if(SeCardSet_u_BleSt == 0U)//����ģ�����
	{
		SeCardSet_u_BleSt = 1U;
		SetUartCmn_BluetoothTxMsg("AT+DISA?",strlen("AT+DISA?"));//������ģ�鷢��ɨ��ָ��
		SeCardSet_w_BleDisCnntT = 0U;
		//USART_PRINTF_S("\n\r��������ɨ��ָ�� AT+DISA?");
	}
	
	/*��������������ʱ��У׼��־*/
	if(0U == Getntpclient_u_NtpCalTimeSt())
	{//��ntpУʱʱ������Ƿ�������Уʱ����
		if(0 == set_time_success)
		{
			if(set_time_flag == 1)
			{
				set_time_flag = 0;
				USART_PRINTF_D("����Уʱ������ʱ��:%s\n",timelist);
				timestamp_timeCalibration(timelist,TIME_STAMP_BJT);//У׼ʱ��оƬʱ��
				set_time_success = 1;
				SeCardSet_dw_setTimeCnt = 0;
			}
		}
		else
		{//����Уʱ��ɺ󣬾����´�Уʱ�ӳ�5s
			SeCardSet_dw_setTimeCnt++;
			if(SeCardSet_dw_setTimeCnt >= (5000/ZR60CTRL_SCHEDULING_CYCLE))
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
	
	/**��������**/
	if((ble_flag == 1) && (0U == b_5s_flag))
	{
		b_5s_flag = 1U;//5s��ʱ��־�����ڼ���Խ��յ��Ŀ���ָ��
		b_5s_count = 0U;//5s��ʱ��ʱ��
		ble_flag = 0;
		if(SeZR60_u_NetUnlockflag != 1U)//��ʾһ��ʱ���������翪��
		{
#ifdef  HYM8563
			(void)hym8563_read_datetime(&Le_h_tm);//��ȡhym8563����ʱ��
#else
			Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
			USART_PRINTF_DATE("��ȡ��ʱ�䣺%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
							  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
			SeCardSet_dw_nowtime = GetTick(&Le_h_tm);//����ʱ���
			Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_BLE,machine_type);
			if(Se_u_PasswordCmpFlag!= 1)//��ʾ�յ�����ȶ�û�гɹ�
			{
				USART_PRINTF_S("����������֤ʧ�� X");
				USART_PRINTF_D("set_time_flag:%d\n",set_time_flag);
				USART_PRINTF_D("����ʱ��:%s\n",timelist);
				SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
			}
			else
			{
				Ble_openDoor_cnt++;
				USART_PRINTF_D("���� -> ���� %d ��\r\n",Ble_openDoor_cnt);
				LOCK_ON;
				SeZR60_u_DoorLockRelaySt = 1U;
				SeZR60_u_DoorOpenLedEvent = 1U;
				SeZR60_u_BleUnlockflag = 1U;
				SeZR60_u_BleLogFltrflag = 1U;
				Se_w_LockDoorTimer = 0;
				SetBleUnlock_PlayFile();
			}
		}
		else
		{//��⵽���һ��ʱ���ڷ���������Զ�̿��ţ�����������رգ�ִ�п���
			if(0U == SeZR60_u_DoorLockRelaySt)
			{//�����ر�״̬
#ifdef  HYM8563
				(void)hym8563_read_datetime(&Le_h_tm);//��ȡhym8563����ʱ��
#else
				Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
				USART_PRINTF_DATE("��ȡ��ʱ�䣺%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
								  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
				SeCardSet_dw_nowtime = GetTick(&Le_h_tm);//����ʱ���
				Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_BLE,machine_type);
				if(Se_u_PasswordCmpFlag!= 1)//��ʾ�յ�����ȶ�û�гɹ�
				{
					USART_PRINTF_S("����������֤ʧ�� X");
					USART_PRINTF_D("set_time_flag:%d\n",set_time_flag);
					//USART_PRINTF_D("����ʱ��:%s\n",timelist);
					SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
				}
				else
				{
					USART_PRINTF_S("���� -> ����");
					//LED_RED;
					LOCK_ON;
					SeZR60_u_DoorLockRelaySt = 1U;
					SeZR60_u_DoorOpenLedEvent = 1U;
					SeZR60_u_BleUnlockflag = 1U;
					SeZR60_u_BleLogFltrflag = 1U;
					Se_w_LockDoorTimer = 0;
					SetBleUnlock_PlayFile();
				}
			}	
		}
	}
	
	if(1U == SeZR60_u_BleLogFltrflag)//�ϴ�����������־
	{
		if(1U == Se_u_PhoneNumFlg)//���յ������ֻ���
		{
			Se_u_PhoneNumFlg = 0U;
			SeZR60_u_BleLogFltrflag = 0U;
			USART_PRINTF_D("���յ��ֻ��ţ�%s\n",Se_u_PhoneNum);
			char temp = 33;
			for(Le_w_i=0U;Le_w_i < 11U;Le_w_i++)
			{//�ֻ��������
				Le_u_UnlockInfo[Le_w_i] = Se_u_PhoneNum[Le_w_i] - temp;
				temp = temp - 2;
			}
			USART_PRINTF_D("���յ��ֻ��Ž����%s\n",Le_u_UnlockInfo);
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_BLE);
		}
	}
	else
	{
		Se_u_PhoneNumFlg = 0U;
	}
	
	
	if(1U == b_5s_flag)//��������ʱ����ʱ�����ڴ��������5s�ڽ��յ�����������ָ�
	{
		b_5s_count++;
		if(b_5s_count >= (5000/ZR60CTRL_SCHEDULING_CYCLE))//5s��ʱ
		{//5s��ʱʱ�䵽����b_5s_flag��־��������������
			b_5s_count = 0U;
			ble_flag = 0U;//�忪��ָ��
			b_5s_flag = 0U;
			Se_u_PhoneNumFlg = 0U;
		}
	}
	else
	{
		b_5s_count = 0U;
	}
	
	if(1U == SeCardSet_u_BleSt)//����ģ��ǿ���ʱ����ʱ8s(������������ͨ�ų�ʱ����)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (8000/ZR60CTRL_SCHEDULING_CYCLE))//��������ͨ�ų�ʱ
		{
			SeCardSet_w_BleDisCnntT = 0U;
			SeCardSet_u_BleSt = 0U;
			RcvFlag = 0U;
			blenum = 0U;
		}
	}
	else
	{
		SeCardSet_w_BleDisCnntT = 0U;
	}
	
	if((SeZR60_u_BleUnlockflag == 1U) || (SeZR60_u_NetUnlockflag == 1U))
	{//���������翪��
		if(w_BleNetExcUnlockDelayTimer >= (3000/ZR60CTRL_SCHEDULING_CYCLE))//3s���ڼ���Խ��յ������������翪������
		{
			SeZR60_u_BleUnlockflag = 0U;
			SeZR60_u_NetUnlockflag = 0U;
			w_BleNetExcUnlockDelayTimer = 0U;
		}
		else
		{
			w_BleNetExcUnlockDelayTimer++;
		}
	}
}



/******************************************************
*��������ClrZR60Ctrl_BleSt

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void ClrZR60Ctrl_BleSt(void)
{
	SeCardSet_u_BleSt = 0U;
}

/******************************************************
*��������ClrZR60Ctrl_BleCnntTimeout

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void ClrZR60Ctrl_BleCnntTimeout(void)
{
	SeCardSet_w_BleDisCnntT = 0U;
}




/******************************************************
*��������ZR60Ctrl_u_ArrayCmp

*��  �Σ�

*����ֵ��

*��  ��������Ƚ�

*��  ע��
******************************************************/
char ZR60Ctrl_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len)
{
	unsigned int Le_w_i;
	for(Le_w_i = 0; Le_w_i < len; Le_w_i++)
		if(a[Le_w_i]!=b[Le_w_i]) return 0;//���ֲ����Ԫ�أ�����0.
	return 1;//��ȫ��ȣ�����1��
}


/******************************************************
*��������GetZR60Ctrl_u_XOR

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
******************************************************/
static uint8 GetZR60Ctrl_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng)
{
	uint16 Le_w_i;
	uint8 Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return Le_u_Xor;
}

static uint8 ZR60Ctrl_u_IsBrdcstAddr(uint8* Le_u_Addr)
{
	uint8 Le_u_ret=1U;
	uint8 Le_u_i;
	USART_PRINTF_S("mac��ַ��");
	for(Le_u_i = 0U;Le_u_i < 12U;Le_u_i++)
	{
		USART_PRINTF_D("%x ",Le_u_Addr[Le_u_i]);
		if(Le_u_Addr[Le_u_i] != 0xff)
		{
			Le_u_ret=0U;
		}
	}
	return Le_u_ret;
}

/*
	��¼������Ϣ
*/
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{
	uint64_t Time = 0U;
#ifdef  HYM8563
	hym8563_Currenttime(&Time);
#else
	RTC_Currenttime(&Time);
#endif
	WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,Le_u_UnlockEvent);//������־��¼д�뻺�����
}
