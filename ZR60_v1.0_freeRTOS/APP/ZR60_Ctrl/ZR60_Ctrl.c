/******************************************************
�ļ�����	ZR60_Ctrl

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "ZR60_Ctrl.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
const char CeZR60Ctrl_u_SoftVersion[10U] = {'Z','R','6','0','_','v','1','.','2','\0'};//����汾
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
static uint8_t  ZR60Ctrl_TonesPlay(uint16_t temp);
static void  CardSet_MainFunction(void);//ĸ������
static void  OpenDoor_IC_MainFunction(void);//����
static void  OpenDoor_Password_MainFunction(void);//������֤
static void  Lcd1062_MainFunction(void);//lcd1062
static uint16 ZR60Ctrl_w_CalPosDifference(uint16 Le_w_value1,uint16 Le_w_value2);
static uint8 GetZR60Ctrl_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
static void  OpenDoor_Switch_MainFunction(void);
static void  OpenDoor_Sound_MainFunction(void);
static void  OpenDoor_Net_MainFunction(void);
static void  OpenDoor_Fire_MainFunction(void);
static void  OpenDoor_Ble_MainFunction(void);
static uint8 ZR60Ctrl_u_IsBrdcstAddr(uint8* Le_u_Addr);
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);
static void ZR60Ctrl_u_LedDisplay(void);
static void  OpenDoor_WX_MainFunction(void);
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
void InitZR60Ctrl_parameter(void)
{
	uint8_t  Le_u_i;
	uint64_t Le_dw_WaitTimer = 0x1FFFFF;
	uint8_t LeZR60_u_Xor = 0;
	
	LeZR60_u_Xor = GetMemIf_u_CheckSum(EepromCfg_CardInfo);//��ȡ����0����У��ͣ����ĸ����Ϣ��
	if((LeZR60_u_Xor == MemIf_ReadEE(EepromCfg_CardInfo,sminfo.community_id,sizeof(sminfo))) && \
	  (STD_ACTIVE == GetMemIf_u_DtVild(EepromCfg_CardInfo)))
	{
		printf("\n��3��flash�洢��ĸ����Ϣ������Ч ��\r\n");
		SetMemIf_EEVild(EepromCfg_CardInfo);//������Ч
		for(Le_u_i = 0U;Le_u_i < 16U;Le_u_i++)
		{
			sminfo1.community_id[Le_u_i] = sminfo.community_id[Le_u_i];
			sminfo1.build_numOne[Le_u_i] = sminfo.build_numOne[Le_u_i];
			sminfo1.build_numTwo[Le_u_i] = sminfo.build_numTwo[Le_u_i];
			sminfo1.build_id[Le_u_i] = sminfo.build_id[Le_u_i];
			sminfo1.door_id[Le_u_i] = sminfo.door_id[Le_u_i];
		}
		sminfo1.suffix = sminfo.suffix;
		read_init_sminfo(&sminfo1,&sm);//��ȡĸ���ļ������ݣ�����ȡic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
	}
	else
	{
		printf("flash�洢��ĸ����Ϣ������Ч X\r\n");
	}
	
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
		SetUartCmn_BluetoothTxMsg("AT+ADDR?",strlen("AT+ADDR?"));//��ȡ����ģ��MAC��ַ
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
	//dw_5msflag_T1 = GetSysTick_Timer();
	//dw_5msflag_T2 = dw_5msflag_T1;
}

/******************************************************
*��������TskZR60Ctrl_MainFunction

*��  �Σ�

*����ֵ��

*��  �������ڽ���

*��  ע�����ڽ����жϵ���
******************************************************/
void TskZR60Ctrl_MainFunction(void)
{ 
	
	/*************************************************
	*		            ĸ������
	*************************************************/	
	CardSet_MainFunction();//ĸ������	
	
	/*************************************************
	*		            ic������
	*************************************************/
	OpenDoor_IC_MainFunction();//ic������	

	/*************************************************
	*		            ��ʱ���뿪��
	*************************************************/	
	OpenDoor_Password_MainFunction();//��ʱ���뿪��
	
	/*************************************************
	*		            ���ڿ��ز���
	*************************************************/
	OpenDoor_Switch_MainFunction();
		
	/*************************************************
	*		            ���翪��
	*************************************************/	
	OpenDoor_Net_MainFunction();
	
	/*************************************************
	*		            ��������
	*************************************************/	
	OpenDoor_Fire_MainFunction();
	
	/*************************************************
	*		            ��������
	*************************************************/	
	//OpenDoor_Sound_MainFunction();
	
	/*************************************************
	*		           ��������
	*************************************************/		
	OpenDoor_Ble_MainFunction();
	
	/*************************************************
	*		          ΢�ſ���
	*************************************************/		
	OpenDoor_WX_MainFunction();
	
	/*************************************************
	*		            ����״ָ̬ʾ�ƿ���
	*************************************************/	
	ZR60Ctrl_u_LedDisplay();
	
	/*************************************************
	*		            ��ʱ���ſ���
	*************************************************/	
	w_1sflag_DelayTime++;
	if(w_1sflag_DelayTime >= (1000/ZR60CTRL_SCHEDULING_CYCLE))
	{
		w_1sflag_DelayTime = 0U;
		if(0U == Se_u_CloseDoorDelayTimeSetFlag)
		{//������ʱʱ��δ���û���Ҫ��������,������ʱ����ʱ��
			/**��ȡ��λ���ĵ�ѹ,���ݵ�ѹ���ù�����ʱʱ��**/
			Voltage_value[0U] = (uint16)((uint32)GetADFliter_ADFliterResult(0)*3300/4096);//ÿ��1s��ȡ��λ���ĵ�ѹ(��ѹ�Ŵ�1000��)
			if(ZR60Ctrl_w_CalPosDifference(Voltage_value[0U],Voltage_value[1U]) <= 100U)
			{
				Se_u_VoltageDifferencCnt++;
				if(Se_u_VoltageDifferencCnt >= 3U)
				{
					Se_u_VoltageDifferencCnt = 0U;
					Voltage_value[1U] = Voltage_value[0U];
					Se_w_DoorLockdelayT = (uint16)((3100 - Voltage_value[0U]) / 150) + 1U;
					if(Se_w_DoorLockdelayT >= 15U)
					{
						Se_w_DoorLockdelayT = 15*1000;
					}
					else if(Se_w_DoorLockdelayT <= 1U)
					{
						Se_w_DoorLockdelayT = 1*1000;
					}
					else
					{
						Se_w_DoorLockdelayT = Se_w_DoorLockdelayT*1000;
					}
					Se_u_CloseDoorDelayTimeSetFlag = 1U;
					USART_PRINTF_D("��ʱ����ʱ���������,ʱ��Ϊ��%dms\n",Se_w_DoorLockdelayT);
				}
			}
			else
			{
				Voltage_value[1U] = Voltage_value[0U];
				Se_u_VoltageDifferencCnt = 0U;
			}
			USART_PRINTF_D("AD����ֵ��%d\n",GetADFliter_ADFliterResult(0));
			USART_PRINTF_D("��ѹ�Ŵ�1000��ֵ��%d\n",Voltage_value[0]);
		}
		else
		{//����ѹ��仯��������������ֵ���������ù�����ʱʱ��
			Voltage_value[0U] = (uint16)((uint32)GetADFliter_ADFliterResult(0)*3300/4096);//ÿ��1s��ȡ��λ���ĵ�ѹ(��ѹ�Ŵ�1000��)
			if(ZR60Ctrl_w_CalPosDifference(Voltage_value[0U],Voltage_value[1U]) >= 250U)//��ѹ�仯0.25v
			{
				Se_u_VoltageDifferencCnt++;
				if(Se_u_VoltageDifferencCnt >= 3U)
				{
					Se_u_VoltageDifferencCnt = 0U;
					Voltage_value[1U] = Voltage_value[0U];
					Se_u_CloseDoorDelayTimeSetFlag = 0U;
				}
			}
			else
			{
				Se_u_VoltageDifferencCnt = 0U;
			}
		}
	}
	
	if(0U == GetZR60Ctrl_u_Alarm())
	{//���������ر�״̬�£�ִ����������
		Se_w_LockDoorTimer++;
		if(Se_w_LockDoorTimer >= (Se_w_DoorLockdelayT/ZR60CTRL_SCHEDULING_CYCLE))
		{		
			LOCK_OFF;
			Se_w_LockDoorTimer = 0U;
			SeZR60_u_DoorLockRelaySt = 0U;
			USART_PRINTF_S("����״̬ -> �ر�");
		}
	}

#ifdef UART_DEBUG	
	/*************************************************
	*		          ��ӡʱ��  
	*************************************************/	
	struct rtc_time Le_h_tm;
	w_500msflag_DelayTime++;
	if(w_500msflag_DelayTime >= (15000/ZR60CTRL_SCHEDULING_CYCLE))
	{
		w_500msflag_DelayTime = 0U;
#ifdef  HYM8563
		(void)hym8563_read_datetime(&Le_h_tm);//��ȡhym8563����ʱ��
#else
		Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
		USART_PRINTF_DATE("��ǰʱ�䣺%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
					  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
	}
#endif
	
	
#if 0
	/*************************************************
	*		            lcd1062��ʾ����
	*************************************************/	
	w_500msflag_DelayTime++;
	if(w_500msflag_DelayTime >= 100)
	{
		w_500msflag_DelayTime = 0;
		update_sec ++;
		if(update_sec == 2)
		{
			update_sec = 0;
			if(sm3_flag == 0)
			{
				//display_time();
			}						
			sec_count ++;	

			if(sec_count >= 20)//lcd1062
			{
				if(sm3_flag == 1)
				{
					sm3_flag = 0;	
					key_num = 0;
				}		
				sec_count = 0;	
			}
		}
	}
#endif
}

/******************************************************
*��������CardSet_MainFunction

*��  �Σ�

*����ֵ��

*��  ����ĸ������

*��  ע��
******************************************************/
static void  CardSet_MainFunction(void)//ĸ������
{
	uint8_t Le_u_i;
	static uint32_t SeCardSet_dw_Timer = 0U;
	switch(SeCardSet_u_St)
	{
		case 0U://����״̬
		{
			if(GetBtnFltr_u_BtnShrtSt(Key_card_set) == STD_ACTIVE)//ĸ�����ð�����Ч
			{
				ClrBtnFltr_BtnShrtValid(Key_card_set);
				SetAudioIO_PlayFile(AudioIO_ConfDevice,2U);
				SeCardSet_dw_Timer = 0U;
				SeCardSet_u_St = 1U;
				USART_PRINTF_S("����ĸ������ģʽ");
			}
		}
		break;
		case 1U://�ȴ�ĸ������״̬
		{
			SeCardSet_dw_Timer++;
			if(SeCardSet_dw_Timer <= (9000/ZR60CTRL_SCHEDULING_CYCLE))
			{
				if(ZR60_CTRL_SETCARD == 1)
				{
					ZR60_CTRL_SETCARD = 0;	
					if(0U == ZR60Ctrl_u_ArrayCmp(sminfo.community_id,sminfo1.community_id,sizeof(sminfo1)))//���ĸ����Ϣ�и���
					{//ĸ����Ϣ�и�����дflash,�������³�ʼ���豸
						if(MemIf_WriteEE(EepromCfg_CardInfo,&sminfo,sizeof(sminfo)))
						{
							for(Le_u_i = 0U;Le_u_i < 16U;Le_u_i++)
							{
								sminfo1.community_id[Le_u_i] = sminfo.community_id[Le_u_i];
								sminfo1.build_numOne[Le_u_i] = sminfo.build_numOne[Le_u_i];
								sminfo1.build_numTwo[Le_u_i] = sminfo.build_numTwo[Le_u_i];
								sminfo1.build_id[Le_u_i] = sminfo.build_id[Le_u_i];
								sminfo1.door_id[Le_u_i] = sminfo.door_id[Le_u_i];
							}
							sminfo1.suffix = sminfo.suffix;
							read_init_sminfo(&sminfo1,&sm);//����ic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
							tcp_LngConnect_Parameter();
							//Settcp_client_DeviceInit();
						}
					}
					Settcp_client_DeviceInit();
					SetAudioIO_PlayFile(AudioIO_ConfSuccess,2U);
					memset(test_buf,0,sizeof(test_buf));//test_buf[]��0		
					SeCardSet_u_St = 0U;
					ZR60_CTRL_AUTOSEARCHCARD = 1;//�����Զ�Ѱ��
					USART_PRINTF_S("ĸ�����óɹ�  ��");
				}
			}
			else//�ȴ���ʱ
			{
				SeCardSet_u_St = 0U;
				USART_PRINTF_S("�ȴ�ĸ�����ó�ʱ���˳�ĸ������ģʽ");
				SetAudioIO_PlayFile(AudioIO_ConfCancel,2U);
			}
		}
		break;
		default:
		break;
	}
}


/******************************************************
*��������OpenDoor_IC_MainFunction

*��  �Σ�

*����ֵ��

*��  ��������

*��  ע��
******************************************************/
static void  OpenDoor_IC_MainFunction(void)//����
{
	//uint64_t Time;
	char Le_u_UnlockInfo[32U] = {0x00};
	
	/*���ö�ic������*/
	if((0U == ZR60_CTRL_SETKEY) && (1U == GetUartCmn_u_TxIdle()))
	{
		Setreadcard_Key();//���ö�������
	}
	
	/*������յ�ic������*/
	if(GetUartCmn_u_RxMsgRenew() == 1)//�յ�������������һ֡����
	{
		GetUartCmn_u_RxMsg(rebackinfo.Dt.Block);
		if(rebackinfo.Dt.DtSrt.Seqnr == 0xFF)
		{//�����������ӻ���Ӧ
			Setstm8_fm17550_SlaveReplySt(rebackinfo.Dt.DtSrt.St);
		}
		else
		{
			handleMsg(rebackinfo); //�����������	
		}
	}
	
	if(1U != SeCardSet_u_St)//�ǿ�Ƭ����״̬
	{
		if(open_door == 1)//����
		{	
			USART_PRINTF_S("ic�� -> ����");
			open_door = 0;
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			Se_w_LockDoorTimer = 0;	
			ZR60_CTRL_AUTOSEARCHCARD = 1;//�����Զ�Ѱ��
			w_SetAutoCard_DelayTime = 0U;
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);	
			
			/*��¼������Ϣ*/
			Json_HexToStr(Le_u_UnlockInfo,SeReadcard_h_CardID.CardID,SeReadcard_h_CardID.lng);
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_CARD);
		}
	}
	
	if(ZR60_CTRL_AUTOSEARCHCARD == 1)//�Զ�Ѱ������
	{
		if(w_SetAutoCard_DelayTime >= (2500/ZR60CTRL_SCHEDULING_CYCLE))//��ʱ2.5s�·��Զ�Ѱ������
		{
			if(1U == GetUartCmn_u_TxIdle())//����tx����
			{
				w_SetAutoCard_DelayTime = 0U;
				Setreadcard_AutoSearchCard();//�·��Զ�Ѱ��ָ��
				ZR60_CTRL_AUTOSEARCHCARD = 0;
			}
		}
		else
		{
			w_SetAutoCard_DelayTime++;
		}
	}
	else
	{
		w_SetAutoCard_DelayTime = 0U;
	}
}

/******************************************************
*��������OpenDoor_Sound_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע��
******************************************************/
static void  OpenDoor_Sound_MainFunction(void)
{
	struct rtc_time Le_h_tm;
	uint64_t Time = 0U;
	if(receive_flag == 1)//����������־��Ч
	{
		receive_flag = 0;
		if(0U == SeZR60_u_DoorLockRelaySt)
		{//�����ر�״̬
#ifdef  HYM8563
			(void)hym8563_read_datetime(&Le_h_tm);//��ȡhym8563����ʱ��
#else
			Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ����ʱ��
#endif
			SeCardSet_dw_nowtime = GetTick(&Le_h_tm);
			Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_SOUND,machine_type);//��������ȶ�
			if(Se_u_PasswordCmpFlag != 1)
			{
				SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
			}
			else
			{
				/*��¼������Ϣ*/
#ifdef  HYM8563
				hym8563_Currenttime(&Time);
#else
				RTC_Currenttime(&Time);
#endif
				WrUnlockLogCache_Queue(&Time,"unknown",UNLOCKLOGCACHE_SOUNDWAVE);//������־��¼д�뻺�����

				USART_PRINTF_S("���� -> ����");
				//LED_RED;
				LOCK_ON;
				SeZR60_u_DoorLockRelaySt = 1U;
				SeZR60_u_DoorOpenLedEvent = 1U;
				Se_w_LockDoorTimer = 0;
				SetAudioIO_PlayFile(AudioIO_WelBack,2U);		
			}
		}
		else
		{
			USART_PRINTF_S("�����ѿ�");
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
		}
	}	
}

/******************************************************
*��������OpenDoor_Ble_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע��
******************************************************/
static void  OpenDoor_Ble_MainFunction(void)
{
	uint16_t Le_w_i;
	struct rtc_time Le_h_tm;
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
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
		if(set_time_flag == 1)
		{
			USART_PRINTF_D("����Уʱ������ʱ��:%s\n",timelist);
			timestamp_timeCalibration(timelist,TIME_STAMP_BJT);//У׼ʱ��оƬʱ��
			set_time_flag = 0;
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
				USART_PRINTF_S("���� -> ����");
				//LED_RED;
				LOCK_ON;
				SeZR60_u_DoorLockRelaySt = 1U;
				SeZR60_u_DoorOpenLedEvent = 1U;
				SeZR60_u_BleUnlockflag = 1U;
				SeZR60_u_BleLogFltrflag = 1U;
				Se_w_LockDoorTimer = 0;
				//SetAudioIO_PlayFile(AudioIO_WelBack,2U);
				SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
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
					SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
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
	
	if(1U == SeCardSet_u_BleSt)//����ģ��ǿ���ʱ����ʱ4s(������������ͨ�ų�ʱ����)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (4000/ZR60CTRL_SCHEDULING_CYCLE))//��������ͨ�ų�ʱ
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
*��������OpenDoor_Password_MainFunction

*��  �Σ�

*����ֵ��

*��  ����������֤

*��  ע��
******************************************************/
static void  OpenDoor_Password_MainFunction(void)//������֤
{
	struct rtc_time tm;
	//uint64_t Time;
	//char Le_u_UnlockInfo[32U] = {0x00};
	uint8_t  buf[2]={0x20,0x00};
	static uint16_t SeCardSet_w_ClrTimer = 0U;//�尴��ֵ����buff��ʱ��
	//static uint8_t SeCardSet_u_BtnFltrFlg = 0U;//��Ч�����˳���־
	
	if(u_TouchKeyScanflag == 1)
	{
		if((SeCardSet_u_PasswordKeySt == STD_ACTIVE) &&(0U == SeCardSet_u_BtnFltrFlg))//����
		{
			USART_PRINTF_S("\r\n��������\r\n");
			SeCardSet_u_PasswordKeySt = 0U;
			SeCardSet_w_ClrTimer = 0U;
			//��ȡ��������ֵ��iic�ӿ���Ҫ����
			if(i2c_master_reg8_recv(0x6E, 0xAA, buf, 2) == 0)
			{
				sec_count = 0;
				w_iickeyVlaue = buf[1];
				w_iickeyVlaue = (w_iickeyVlaue << 8) | buf[0];
				u_ActualKeyValue = ZR60Ctrl_TonesPlay(w_iickeyVlaue);
				if(u_ActualKeyValue != 0xff)
				{
					SeCardSet_u_BtnFltrFlg = 1U;
					if(u_ActualKeyValue == 'C')//���� -> ȡ��
					{
						key_num = 0;
						memset(test_buf,0,sizeof(test_buf));
						//sm3_flag = 1;		
					}
					else if(u_ActualKeyValue == 'A')//���� -> ȷ��
					{ 												
						if(key_num == 6)
						{//��ǰΪ��ʱ���뿪��
							strncpy(key_buf,test_buf,6);
							key_flag = 1;
							//state = 0;	
						}
						else if(key_num == 9)
						{//��ǰΪ�̼�ˢ��
							if(strcmp(test_buf,"110911060")==0)
							{
								USART_PRINTF_S("��ǰ������������");
								SeCardSet_u_MasterCtrlIap = 1U;//��������������־
								
							}
							else if(strcmp(test_buf,"110911061")==0)
							{
								USART_PRINTF_S("����������");
								SeCardSet_u_CardReaderIap = 1U;//������ģ������������־
								
							}
							else
							{
								SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
							}
						}
						else
						{
							SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
						}
						key_num = 0;
						memset(test_buf,0,sizeof(test_buf));
						//sm3_flag = 0;												 
					}
					else
					{	
						if(key_num < sizeof(test_buf))
						{
							test_buf[key_num] = u_ActualKeyValue;
							key_num++;
						}
						else
						{
							key_num = 0;
							memset(test_buf,0,sizeof(test_buf));
							SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
						}
						//sm3_flag = 1;
					}											
					//state = 1;
				}
			}
			else
			{
				USART_PRINTF_S("\r\nErrorLogging����ȡ����ֵʧ��\r\n");
			}
		}
		else
		{
			if(SeCardSet_u_PasswordKeySt == STD_ACTIVE)
			{
				//USART_PRINTF_S("\r\n��Ч�����ж��˳�\r\n");
				SeCardSet_u_BtnFltrFlg = 0U;
				SeCardSet_u_PasswordKeySt = 0U;
			}
		}
	}
	else
	{
		SeCardSet_u_PasswordKeySt = 0U;
	}
	
	if (key_flag == 1)//����6λ�����־
	{
		key_flag = 0;
#ifdef  HYM8563
		(void)hym8563_read_datetime(&tm);//��ȡhym8563����ʱ��
#else
		Read_RTC_TimeAndDate(&tm);//��ȡ����ʱ��
#endif
		SeCardSet_dw_nowtime = GetTick(&tm);
		Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_PASSWORD,machine_type);//��������ȶ�
		if(Se_u_PasswordCmpFlag != 1)
		{
			SetAudioIO_PlayFile(AudioIO_ErrKey,1U);		
		}
		else
		{
			USART_PRINTF_S("��ʱ���� -> ����");
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			Se_w_LockDoorTimer = 0;
			memset(key_buf,0,6);
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
			
			/*��¼������Ϣ*/
			ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_TEMPPSWORD);
		}
	}
	
	if(SeCardSet_w_ClrTimer >= (12000/ZR60CTRL_SCHEDULING_CYCLE))//10ms�������ڣ����12s
	{
		key_num = 0;
		key_flag = 0;
		memset(key_buf,0,6);
		memset(test_buf,0,sizeof(test_buf));
		//USART_PRINTF_S("�������볬ʱ\r\n");
	}
	else
	{
		SeCardSet_w_ClrTimer++;
	}
}

/******************************************************
*��������OpenDoor_Switch_MainFunction

*��  �Σ�

*����ֵ��

*��  �������ڿ��ؿ���

*��  ע��
******************************************************/
static void  OpenDoor_Switch_MainFunction(void)
{
	//uint64_t Time = 0U;
	if( GetBtnFltr_u_BtnShrtSt(Key_Unlock) == STD_ACTIVE)//���ڿ��ز���
	{
		USART_PRINTF_S("���ڿ��� -> ����");
		ClrBtnFltr_BtnShrtValid(Key_Unlock);	
		LOCK_ON;
		Se_w_LockDoorTimer = 0;
		SeZR60_u_DoorLockRelaySt = 1U;
		SeZR60_u_DoorOpenLedEvent = 1U;
		SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);	
		
		/*��¼������Ϣ*/
		//ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_DOORSWITCH);
	}
}


/******************************************************
*��������OpenDoor_Fire_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע��
******************************************************/
static void  OpenDoor_Fire_MainFunction(void)
{
	//uint64_t Time = 0U;
	switch(Se_u_FireALARM)
	{
		case 0U:
		{//��������δ����״̬
			if(GetPSFltr_u_PeriSt(Key_Alarm) == STD_ON)//��������
			{
				//ClrBtnFltr_BtnShrtValid(Key_Alarm);	
				USART_PRINTF_S("�������� -> ����");
				LOCK_ON;//����
				SeZR60_u_DoorLockRelaySt = 1U;
				USART_Cmd(UART5,  DISABLE);//�رմ���5�ж�
				USART_Cmd(USART3, DISABLE);//�رմ���3�ж�
				USART_Cmd(UART4,  DISABLE);//�رմ���4�ж�
				u_TouchKeyScanflag = 0;//�رհ������ȡ
				Se_u_FireALARM = 1U;
				SeCardSet_dw_ALARMDlyTime = (2500/ZR60CTRL_SCHEDULING_CYCLE);
				
				//��¼������Ϣ
				//ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_FIREALARM);
			}		
		}
		break;
		case 1U:
		{//������������״̬,���縴λ/������Żָ���������
			if(0U == GetAudioIO_u_PlaySt())
			{
				SeCardSet_dw_ALARMDlyTime++;
				if(SeCardSet_dw_ALARMDlyTime >= (2500/ZR60CTRL_SCHEDULING_CYCLE))//��������Ƶ�ʿ��ƣ�2.5s/��
				{
					SeCardSet_dw_ALARMDlyTime = 0U;
					SetAudioIO_PlayFile(AudioIO_FireAlarm,2U);
				}
			}
		}
		break;
		default:
		{
			Se_u_FireALARM = 0U;
		}
		break;
	}
}

/******************************************************
*��������OpenDoor_Net_MainFunction

*��  �Σ�

*����ֵ��

*��  �������翪��

*��  ע��
******************************************************/
static void  OpenDoor_Net_MainFunction(void)
{
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
	if( 1U == ZR60_CTRL_REMOTE_UNLOCK)
	{
		USART_PRINTF_S("Զ�� -> ����");
		ZR60_CTRL_REMOTE_UNLOCK = 0U;
		if(SeZR60_u_BleUnlockflag != 1U)//��ʾһ��ʱ��������������
		{
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			SeZR60_u_NetUnlockflag = 1U;
			Se_w_LockDoorTimer = 0;
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);

			Gettcp_LngConnect_PhoneNo(Le_u_UnlockInfo);
			/*��¼������Ϣ*/
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_REMOTE);
		}
		else
		{//��⵽���һ��ʱ���ڷ������������ţ�����������رգ�ִ�п���
			if(0U == SeZR60_u_DoorLockRelaySt)
			{//�����ر�״̬
				LOCK_ON;
				SeZR60_u_DoorLockRelaySt = 1U;
				SeZR60_u_DoorOpenLedEvent = 1U;
				SeZR60_u_NetUnlockflag = 1U;
				Se_w_LockDoorTimer = 0;
				SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
				Gettcp_LngConnect_PhoneNo(Le_u_UnlockInfo);		
				/*��¼������Ϣ*/
				ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_REMOTE);
			}
		}
	}	
}

/******************************************************
*��������OpenDoor_WX_MainFunction

*��  �Σ�

*����ֵ��

*��  ����΢�Ź��ںſ���

*��  ע��
******************************************************/
static void  OpenDoor_WX_MainFunction(void)
{
	char Le_u_UnlockInfo[32U] = {0};
	
	if( 1U == ZR60_CTRL_WX_UNLOCK)
	{
		USART_PRINTF_S("΢�Ź��ں� -> ����");
		ZR60_CTRL_WX_UNLOCK = 0U;

		LOCK_ON;
		Se_w_LockDoorTimer = 0;
		SeZR60_u_DoorLockRelaySt = 1U;
		SeZR60_u_DoorOpenLedEvent = 1U;
		SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);

		if(1 == Gettcp_LngConnect_WXOpenInfo(Le_u_UnlockInfo))
		{
			/*��¼������Ϣ*/
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_WECHAT);
		}
	}	
}

/******************************************************
*��������Lcd1062_MainFunction

*��  �Σ�

*����ֵ��

*��  ����lcd1062��ʾ

*��  ע��
******************************************************/
static void  Lcd1062_MainFunction(void)//lcd1062
{
	if(state == 1 && sm3_flag == 1)
	{
		//u_TouchKeyScanflag = 0;
		display(test_buf,key_num);
		if(key_num >  6)
		{				
			key_num =0;
			sm3_flag = 0;
		}
		u_TouchKeyScanflag = 1;
		state = 0;
	}
}


/******************************************************
*��������return_data

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
static uint8_t  ZR60Ctrl_TonesPlay(uint16_t temp)
{
	uint8_t ret;
    switch(temp)
    {
        case 0x0001:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '6';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 6\r\n");	
		}
		break;
        case 0x0002:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '9' ;
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 9\r\n");
		}
        break;
		case 0x0004:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  'A';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ ȷ��\r\n");
		}
		break;
        case 0x0008:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '0';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 0\r\n");
		}
		break;
        case 0x0010:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '3';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 3\r\n");
		}
		break;
        case 0x0020:
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '2';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 2\r\n");
		break;
        case 0x0040:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			 ret =  '1';
			 //SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 1\r\n");
		}
		break;
        case 0x0080:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '4';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 4\r\n");
		}
		break;
        case 0x1000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '7';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 7\r\n");
		}
		break;
        case 0x2000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = 'C';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ ȡ��\r\n");
		}
		break;
        case 0x4000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '8';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 8\r\n");
		}
		break;
        case 0x8000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '5';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n��ֵ 5\r\n");
		}
		break;
		default:
		{
			ret = 0xff;
			USART_PRINTF_S("��Ч��ֵ");
		}
		break;
    }
		return ret;
}


/******************************************************
*��������GetZR60Ctrl_u_Alarm

*��  �Σ�

*����ֵ��

*��  ������ȡ��������״̬

*��  ע��
******************************************************/
uint8_t	GetZR60Ctrl_u_Alarm(void)
{
	return Se_u_FireALARM;
}


/******************************************************
*��������GetZR60Ctrl_u_CardSetSt

*��  �Σ�

*����ֵ��

*��  ������ȡ��Ƭ����״̬

*��  ע��
******************************************************/
uint8_t	GetZR60Ctrl_u_CardSetSt(void)
{
	return SeCardSet_u_St;
}

/******************************************************
*��������SetZR60Ctrl_u_PasswordKey

*��  �Σ�

*����ֵ��

*��  ��������password ����״̬

*��  ע��
******************************************************/
void SetZR60Ctrl_u_PasswordKey(void)
{
	SeCardSet_u_PasswordKeySt = 1U;
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
*��������

*��  �Σ�

*����ֵ��

*��  ���������������Ĳ�ֵ

*��  ע��
******************************************************/
static uint16 ZR60Ctrl_w_CalPosDifference(uint16 Le_w_value1,uint16 Le_w_value2)
{
	uint16 Le_w_retValue;
	if(Le_w_value1 >= Le_w_value2)
	{
		Le_w_retValue = Le_w_value1 - Le_w_value2;
	}
	else
	{
		Le_w_retValue = Le_w_value2 - Le_w_value1;
	}
	
	return Le_w_retValue;
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


/*
	ָʾ��״̬��ʾ
*/
static void ZR60Ctrl_u_LedDisplay(void)
{
	static uint8_t Le_u_cnt = 0U;
	static uint8_t Le_u_LedSt = 0U;//0--Ϩ��1--����
	static uint16_t Le_w_timer = 0U;
	if((0 != GET_PHY_LINK_STATUS()) && (Se_u_FireALARM == 0U))//��������״̬
	{
		switch(SeZR60_u_DoorOpenLedEvent)
		{
			case 0:
			{
				LED_GREEN;
				Le_u_LedSt = 1U;
			}
			break;
			case 1:
			{
				LED_RGBOFF;
				Le_u_LedSt = 0U;
				Le_u_cnt = 0;
				Le_w_timer = 0U;
				SeZR60_u_DoorOpenLedEvent = 2;
			}
			break;
			case 2:
			{
				if(Le_w_timer >= (100/ZR60CTRL_SCHEDULING_CYCLE))//���100ms
				{
					Le_w_timer = 0U;
					if(1 == Le_u_LedSt)
					{
						LED_RGBOFF;
						Le_u_LedSt = 0U;
					}
					else
					{
						LED_GREEN;
						Le_u_LedSt = 1U;
						Le_u_cnt++;
					}
					
					if(Le_u_cnt >= 3)
					{
						Le_u_cnt = 0;
						Le_w_timer = 0U;
						SeZR60_u_DoorOpenLedEvent = 0;
					}
				}
				else
				{
					Le_w_timer++;
				}
			}
			break;
			default:
			{
				SeZR60_u_DoorOpenLedEvent = 0;
			}
			break;
		}
	}
	else//����״̬
	{//�̵Ƴ�����˸�����200ms��
		if(Le_w_timer >= (200/ZR60CTRL_SCHEDULING_CYCLE))//���200ms
		{
			Le_w_timer = 0U;
			if(1 == Le_u_LedSt)
			{
				LED_RGBOFF;
				Le_u_LedSt = 0U;
			}
			else
			{
				LED_GREEN;
				Le_u_LedSt = 1U;
			}
		}
		else
		{
			Le_w_timer++;
		}
	}
}


/******************************************************
*��������GetZR60Ctrl_u_MastCtrlIapSt

*��  �Σ�

*����ֵ��

*��  ������ȡ��������������־

*��  ע��
******************************************************/
uint8_t	GetZR60Ctrl_u_MastCtrlIapSt(void)
{
	return SeCardSet_u_MasterCtrlIap;
}


/******************************************************
*��������ClrZR60Ctrl_MastCtrlIapSt

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void ClrZR60Ctrl_MastCtrlIapSt(void)
{
	SeCardSet_u_MasterCtrlIap = 0U;
}

/******************************************************
*��������GetZR60Ctrl_u_CardReaderIapSt

*��  �Σ�

*����ֵ��

*��  ������ȡ����������������־

*��  ע��
******************************************************/
uint8_t	GetZR60Ctrl_u_CardReaderIapSt(void)
{
	return SeCardSet_u_CardReaderIap;
}

/******************************************************
*��������ClrZR60Ctrl_CardReaderIapSt

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void ClrZR60Ctrl_CardReaderIapSt(void)
{
	SeCardSet_u_CardReaderIap = 0U;
}

