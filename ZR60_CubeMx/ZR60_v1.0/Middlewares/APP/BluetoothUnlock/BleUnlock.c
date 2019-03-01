/******************************************************
文件名：	BleUnlock

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "BluetoothUnlock/BleUnlock.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static uint8  	set_time_success =0U;//蓝牙校时成功标志
static uint32 	SeCardSet_dw_setTimeCnt = 0U;//蓝牙校时延时计时器
static uint8  	set_time_flag = 0;//校时请求标志
static char   	Se_u_time[15] = {0};//蓝牙时间
static uint8 	ble_Unlockflag = 0;//蓝牙开门请求标志
static char   	Se_u_password[12] = {0};//蓝牙验证密码
static uint8	Se_u_PhoneNumFlg = 0;//接收到手机号标志
static uint8	Se_u_PhoneNum[12] = {0};//接收到手机号
static uint8 	b_5s_flag = 0;//5s延时标志（用于过滤5s内多余的开门请求指令）
static uint16 	b_5s_count = 0;//5s延时计数器
static uint8  	SeZR60_u_BleUnlockflag = 0U;//指示当前是蓝牙开门
static uint8  	SeZR60_u_BleLogFltrflag = 0U;//蓝牙开门日志记录过滤标志
static uint16 	w_BleNetExcUnlockDelayTimer = 0U;//蓝牙、网络开门功能互锁延时计时器
static uint8  	SeCardSet_u_BleSt = 0U;//蓝牙状态：0--空闲；1--正在扫描
static uint16 	SeCardSet_w_BleDisCnntT = 0U;//蓝牙断开超时计时器
static volatile uint8  Se_u_BleMacFlg = 0U;//mac地址有效标志
static uint8	Se_u_MacAddr[12U] = {0};//mac地址

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitBleUnlock_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitBleUnlock_parameter(void)
{
	uint64_t Le_dw_WaitTimer = 0x6FFFFFF;
	uint8 LeBleUnlock_u_Xor = 0;
	BleUnlock_rtcTime	rtcTime;
	uint32 timestamp;
	uint8 LeBleUnlock_u_cnt = 30;
	while(--Le_dw_WaitTimer);//等待蓝牙模块稳定（hm-13双模模块上电需要1.5s稳定）
	LeBleUnlock_u_Xor = GetBleUnlockCfg_u_CheckSum(BLEUNLOCK_MAC_INFO);//读取mac地址数据校验和
	if((LeBleUnlock_u_Xor == BleUnlockCfg_RdEE(BLEUNLOCK_MAC_INFO,Se_u_MacAddr,sizeof(Se_u_MacAddr))) \
											&& (STD_ACTIVE == GetBleUnlockCfg_u_DtVild(BLEUNLOCK_MAC_INFO)))
	{
		printf("\nflash存储的mac地址有效  √\n");
		SetMemIf_EEVild(BLEUNLOCK_MAC_INFO);//数据有效
		Se_u_BleMacFlg = 1U;
	}
	else
	{//读取蓝牙mac地址
		while(LeBleUnlock_u_cnt)
		{
			Le_dw_WaitTimer = 0x1FFFFF;
			BleUnlockCfg_BleTxMsg(BleUnlock_MAC_ADDR);//获取蓝牙模块MAC地址
			while((Se_u_BleMacFlg != 1) && (--Le_dw_WaitTimer));//等待获取蓝牙mac地址
			LeBleUnlock_u_cnt--;
			if(Se_u_BleMacFlg == 1) break;
		}
		
		if(Se_u_BleMacFlg == 1)
		{
			printf("\n获取蓝牙mac地址成功\n");
			(void)BleUnlockCfg_WrEE(BLEUNLOCK_MAC_INFO,Se_u_MacAddr,sizeof(Se_u_MacAddr));//mac地址写入eeprom/flash
		}
		else
		{
			printf("\n获取蓝牙mac地址失败  X\n");
			BleUnlockCfg_datetime(&rtcTime,&timestamp);
			BLEUNLOCK_PRINTF_DATE("读取的本地时间：%d.%d.%d %d:%d:%d\n",rtcTime.tm_year,rtcTime.tm_mon, \
							  rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
			/*根据rtc时钟时间转换MAC地址*/
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
*函数名：TskBleUnlock_MainFunction

*形  参：

*返回值：

*描  述：

*备  注：
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

	if(SeCardSet_u_BleSt == 0U)//蓝牙模块空闲
	{
		SeCardSet_u_BleSt = 1U;
		BleUnlockCfg_BleTxMsg(BleUnlock_DISA);//给蓝牙模块发送扫描指令
		SeCardSet_w_BleDisCnntT = 0U;
	}
	
	if(1U == SeCardSet_u_BleSt)//蓝牙模块非空闲时，计时8s(用于蓝牙串口通信超时处理)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (8000/BLEUNLOCK_SCHEDULING_CYCLE))
		{//扫描指令发出后，8s超时时间内都未收到扫描结束指令，蓝牙串口通信超时
			SeCardSet_w_BleDisCnntT = 0U;
			SeCardSet_u_BleSt = 0U;
		}
	}
	else
	{
		SeCardSet_w_BleDisCnntT = 0U;
	}
	
	/**蓝牙开锁**/
	if((ble_Unlockflag == 1) && (0U == b_5s_flag))
	{
		b_5s_flag = 1U;//5s延时标志，此期间忽略接收到的开门指令
		b_5s_count = 0U;//5s延时计时器
		ble_Unlockflag = 0;
		if(BleUnlockCfg_u_NetUnlockSt() != 1U)//表示一段时间内无网络开门
		{
			BleUnlockCfg_datetime(&rtcTime,&timestamp);
			BLEUNLOCK_PRINTF_DATE("读取的时间：%d.%d.%d %d:%d:%d\n",rtcTime.tm_year,rtcTime.tm_mon, \
							  rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
			machine_type = BleUnlockCfg_u_machineType();
			if(BleUnlockCfg_PasswordAuth(timestamp,machine_type,password) == 1)
			{//表示收到密码比对成功
				Ble_openDoor_cnt++;
				BLEUNLOCK_PRINTF_D("蓝牙 -> 开门 %d 次\r\n",Ble_openDoor_cnt);
				BleUnlockCfg_OpenDoor();

				SeZR60_u_BleUnlockflag = 1U;
				SeZR60_u_BleLogFltrflag = 1U;
				BleUnlockCfg_Play(BLEUNLOCK_DOOROPEN);
			}
			else
			{
				BLEUNLOCK_PRINTF_D("蓝牙密码验证失败，蓝牙时间:%s\n",Se_u_time);
				BleUnlockCfg_Play(BLEUNLOCK_ERRKEY);				
			}
		}
		else
		{//检测到最近一段时间内发生过网络远程开门，若检测门锁关闭，执行开门
			if(0U == GetBleUnlockCfg_u_DoorSt())
			{//门锁关闭状态
				BleUnlockCfg_datetime(&rtcTime,&timestamp);
				BLEUNLOCK_PRINTF_DATE("读取的时间：%d.%d.%d %d:%d:%d\n",rtcTime.tm_year, \
					rtcTime.tm_mon, rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
				machine_type = BleUnlockCfg_u_machineType();
				if(BleUnlockCfg_PasswordAuth(timestamp,machine_type,password) == 1)
				{//表示收到密码比对成功		
					Ble_openDoor_cnt++;
					BLEUNLOCK_PRINTF_D("蓝牙 -> 开门 %d 次\r\n",Ble_openDoor_cnt);
					BleUnlockCfg_OpenDoor();
					SeZR60_u_BleUnlockflag = 1U;
					SeZR60_u_BleLogFltrflag = 1U;
					BleUnlockCfg_Play(BLEUNLOCK_DOOROPEN);				
				}
				else
				{
					BLEUNLOCK_PRINTF_D("蓝牙密码验证失败，蓝牙时间:%s\n",Se_u_time);
					BleUnlockCfg_Play(BLEUNLOCK_ERRKEY);
				}
			}	
		}
	}
	
	if(1U == b_5s_flag)//蓝牙开门时，计时（用于处理（清除）5s内接收到的蓝牙开门指令）
	{
		b_5s_count++;
		if(b_5s_count >= (5000/BLEUNLOCK_SCHEDULING_CYCLE))//5s计时
		{//5s计时时间到，清b_5s_flag标志以允许蓝牙开门
			b_5s_count = 0U;
			ble_Unlockflag = 0U;//清开门指令
			b_5s_flag = 0U;
		}
	}
	else
	{
		b_5s_count = 0U;
	}

	if(SeZR60_u_BleUnlockflag == 1U)//3s，期间忽略接收到的蓝牙开门请求
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
	
	/*上传蓝牙开门日志*/
	if(1U == SeZR60_u_BleLogFltrflag)
	{
		if(1U == Se_u_PhoneNumFlg)//接收到蓝牙手机号
		{
			Se_u_PhoneNumFlg = 0U;
			SeZR60_u_BleLogFltrflag = 0U;
			BLEUNLOCK_PRINTF_D("接收的手机号：%s\n",Se_u_PhoneNum);
			char temp = 33;
			for(Le_w_i=0U;Le_w_i < 11U;Le_w_i++)
			{//手机号码解码
				Le_u_UnlockInfo[Le_w_i] = Se_u_PhoneNum[Le_w_i] - temp;
				temp = temp - 2;
			}
			BLEUNLOCK_PRINTF_D("接收的手机号解码后：%s\n",Le_u_UnlockInfo);
			BleUnlockCfg_UnlockRecord(Le_u_UnlockInfo);
		}
	}
	else
	{
		Se_u_PhoneNumFlg = 0U;
	}
	
	/*处理蓝牙发来的时间校准标志*/
	if(0U == Getntpclient_u_NtpCalTimeSt())
	{//非ntp校时时，检查是否有蓝牙校时请求
		if(0 == set_time_success)
		{
			if(set_time_flag == 1)
			{
				set_time_flag = 0;
				BLEUNLOCK_PRINTF_D("蓝牙校时，蓝牙时间:%s\n",Se_u_time);
				BleUnlockCfg_timeCali(Se_u_time);
				set_time_success = 1;
				SeCardSet_dw_setTimeCnt = 0;
			}
		}
		else
		{//蓝牙校时完成后，距离下次校时延迟5s
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
*函数名：BleUnlock_RxMsgCallback

*形  参：

*返回值：

*描  述：串口接收数据帧处理回调函数

*备  注：
******************************************************/
void BleUnlock_RxMsgCallback(char* Le_u_rxMsg,uint8 Le_u_lng)
{
	uint8 Le_u_i;
	//printf(Le_u_rxMsg);
	if(Le_u_rxMsg[0] != 0x4F) return;//检查帧头
	if((Le_u_lng != 6U) && (Le_u_lng != 19U) && (Le_u_lng != 8U) && ((Le_u_rxMsg[Le_u_lng -2] != 0x0D) || \
				(Le_u_rxMsg[Le_u_lng -1] != 0x0A)))//检查帧尾,数据帧长度为8字节,可能接收到扫描开始或者扫描结束帧
	{
		return;
	}
	
	switch(Le_u_lng)
	{
		case 59://苹果IOS接收手机号
		case 37://苹果IOS接收临时密码
		case 47://苹果IOS校时
		case 44://安卓接收手机号数据帧长度
		case 53://安卓校时数据帧长度
		case 55://安卓接收临时密码数据帧长度
		{
			switch(Le_u_rxMsg[26])
			{
				case 0x1A://安卓接收临时密码
				{
					if((Le_u_rxMsg[29] == 0x57) && (Le_u_rxMsg[30] == 0x5A))
					{
						for(Le_u_i = 0;Le_u_i < 6;Le_u_i++)
						{
							Se_u_password[Le_u_i] = Le_u_rxMsg[31+Le_u_i];//门口机密码
							Se_u_password[Le_u_i + 6] = Le_u_rxMsg[47+Le_u_i];//围墙机密码
						}
						ble_Unlockflag = 1;
					}
					else
					{}
				}
				break;
				case 0x0F://安卓接收手机号
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
				case 0x18://安卓校时
				{
					if((Le_u_rxMsg[29] == 0x49) && (Le_u_rxMsg[30] == 0x54))
					{
						strncpy(Se_u_time,Le_u_rxMsg+31,14);
						set_time_flag = 1;
					}
				}						
				case 0x08://苹果IOS接收临时密码
				{
					if((Le_u_rxMsg[27] == 0x07) && (Le_u_rxMsg[28] == 0x09))
					{
						for(Le_u_i = 0;Le_u_i < 6;Le_u_i++)
						{
							Se_u_password[Le_u_i] = Le_u_rxMsg[29+Le_u_i];//门口机密码
							Se_u_password[Le_u_i + 6] = Le_u_rxMsg[29+Le_u_i];//围墙机密码
						}
						ble_Unlockflag = 1;
					}
				}
				break;
				case 0x12://苹果IOS校时
				{
					if((Le_u_rxMsg[29] == 0x54) && (Le_u_rxMsg[30] == 0x49))
					{
						strncpy(Se_u_time,Le_u_rxMsg+31,14);
						set_time_flag = 1;
					}
				}
				break;
				case 0x1E://苹果IOS接收手机号
				{
					if((Le_u_rxMsg[36] == 0x00) && (Le_u_rxMsg[37] == 0x0A))
					{
						if(Le_u_rxMsg[40] == 0x52)
						{//表示接收到手机号码数字1,有效的手机号
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
		case 19://MAC地址数据帧长度
		{
			if((Le_u_rxMsg[3] == 'G') && (Le_u_rxMsg[4] == 'e') && \
				(Le_u_rxMsg[5] == 't') && (Le_u_rxMsg[6] == ':'))
			{//接收到MAC地址
				for(Le_u_i = 0;Le_u_i < 12U;Le_u_i++)
				{
					Se_u_MacAddr[Le_u_i] = Le_u_rxMsg[7+Le_u_i];
				}
				BLEUNLOCK_PRINTF_D("\n接收MAC地址数据帧：%s",Se_u_MacAddr);
				Se_u_BleMacFlg = 1;
			}
		}
		break;
		case 6://接收到扫描结束帧
		case 8://接收到扫描开始或者是扫描结束帧
		{
			if((Le_u_rxMsg[3] == 'D') && (Le_u_rxMsg[4] == 'I') && \
					(Le_u_rxMsg[5] == 'S'))
			{
				if((Le_u_lng == 6U) || ((Le_u_rxMsg[6] == 'B') && \
											(Le_u_rxMsg[7] == 'E')))//收到扫描结束帧
				{
					//BLEUNLOCK_PRINTF_S("\n接收到扫描结束帧");
					SeCardSet_u_BleSt = 0U;//设置蓝牙模块空闲
				}
				else
				{}
			}
		}
		break;
		default:
		break;
	}
	memset(Le_u_rxMsg,0,UART5_DMA_RX_BUFFER_SIZE);//清0
}


