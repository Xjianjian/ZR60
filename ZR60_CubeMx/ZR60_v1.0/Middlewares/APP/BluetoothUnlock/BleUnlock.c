/******************************************************
文件名：	BleUnlock

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "./BluetoothUnlock/BleUnlock.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
/*
sm3密码生成使用的数据信息
围墙机：community_id为小区id前4字节；build_num和cell_num全F
门口机：community_id为小区id前4字节；build_num前4字节为楼栋编号，其余全F;cell_num全F
*/
struct sm3_info sm = {{'F','F','F','F','F','F','F','F'}, \
		{'F','F','F','F','F','F','F','F'},{'F','F','F','F','F','F','F','F'},'\0'};

static uint8_t 	sm3_flag = 0;
static uint8_t  state = 0U;
static uint8_t 	sec_count = 0;
static uint8_t  test_buf[12];//缓存触摸按键信息
static uint8_t 	b_5s_flag = 0;//5s延时标志
static uint16_t b_5s_count = 0;//5s延时计数器
//static uint8_t 	b_err_5s_flag = 0;
//static uint8_t	b_err_5s_count = 0;
static uint8_t  SeZR60_u_DoorLockRelaySt = 0U;//门锁继电器状态状态，0--关；1--开
static uint8_t  SeZR60_u_BleUnlockflag = 0U;//指示当前是蓝牙开门
static uint8_t  SeZR60_u_NetUnlockflag = 0U;//指示当前是网络开门
static uint8_t  SeZR60_u_BleLogFltrflag = 0U;//蓝牙开门日志记录过滤标志
static uint8_t	Se_u_FireALARM = 0U;//消防警报标志：默认关闭消防报警
static uint16 	Voltage_value[2U] = {0U};//电压值
static uint8_t 	Se_u_VoltageDifferencCnt = 0U;//压差计数器，两次电压差小于某一电压值时计数
static uint16  	Se_w_DoorLockdelayT = 0xffffU;//延时关门设置的时间(单位：ms)
static uint8_t  Se_u_CloseDoorDelayTimeSetFlag = 0U;//延时关门时间设置标志
static uint16_t	Se_w_LockDoorTimer = 0U;//延时关门计时器，计时时间到关闭门锁
//static uint32_t dw_5msflag_T1 = 0U;//5ms计时器
//static uint32_t dw_5msflag_T2 = 0U;//5ms计时器
static uint16_t w_SetAutoCard_DelayTime = 0U;//设置自动寻卡延时时间
static uint16_t w_500msflag_DelayTime = 0U;//500ms计时器
static uint16_t w_1sflag_DelayTime = 0U;//1s计时器
static uint16_t w_BleNetExcUnlockDelayTimer = 0U;//蓝牙、网络开门功能互锁延时计时器
static uint8_t  u_TouchKeyScanflag = 1;//允许读取触摸按键值标志
static uint8_t  key_num = 0;//读取触摸按键值个数
//static uint8_t  buf[2]={0x20,0x00};
static uint16_t w_iickeyVlaue = 0U;//iic读取的键值
static uint8_t u_ActualKeyValue = 0U;//iic读取的键值转换后的实际触摸按键值
static uint8_t  key_flag = 0;//临时密码接收完成标志
static uint8_t  SeCardSet_u_St = 0U;//母卡设置状态
static uint32_t SeCardSet_dw_ALARMDlyTime = 0U;//警报延时时间

static uint8_t  SeCardSet_u_PasswordKeySt = 0U;//密码按键状态
static uint8_t  SeCardSet_u_BtnFltrFlg = 0U;//无效按键滤除标志

static uint8_t  set_time_success =0U;//蓝牙校时成功标志
static uint32_t SeCardSet_dw_setTimeCnt = 0U;//蓝牙校时延时计时器
static uint8_t  SeCardSet_u_BleSt = 0U;//蓝牙状态：0--空闲；1--正在扫描
static uint16_t SeCardSet_w_BleDisCnntT = 0U;//蓝牙断开超时计时器
static uint32_t SeCardSet_dw_nowtime = 0U;
static uint8_t  Se_u_PasswordCmpFlag = 0U;//密码比对标志：0--失败；1--成功
struct rtc_time Ve_h_tm;//日期

static uint8_t  SeCardSet_u_MasterCtrlIap = 0U;//主控在线升级标志
static uint8_t  SeCardSet_u_CardReaderIap = 0U;//读卡器模块在线升级标志

static uint8_t  SeZR60_u_DoorOpenLedEvent = 0U;//指示灯事件：0--无；1--触发一次开锁指示灯事件

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void  OpenDoor_Ble_MainFunction(void);
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);
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
	uint8_t  Le_u_i;
	uint64_t Le_dw_WaitTimer = 0x6FFFFFF;
	uint8_t LeZR60_u_Xor = 0;
	
	LeZR60_u_Xor = GetMemIf_u_CheckSum(EepromCfg_MacInfo);//读取mac地址数据校验和
	if((LeZR60_u_Xor == MemIf_ReadEE(EepromCfg_MacInfo,Se_u_MacAddr,sizeof(Se_u_MacAddr))) && \
	  (STD_ACTIVE == GetMemIf_u_DtVild(EepromCfg_MacInfo)))
	{
		printf("\n【6】flash存储的mac地址有效  √\r\n");
		SetMemIf_EEVild(EepromCfg_MacInfo);//数据有效
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
	{//读取蓝牙mac地址
		while(--Le_dw_WaitTimer);//等待蓝牙模块稳定（hm-13双模模块上电需要1.5s稳定）
		Le_dw_WaitTimer = 0x1FFFFF;
#ifdef HM_11
		SetUartCmn_BluetoothTxMsg("AT+ADDR?",strlen("AT+ADDR?"));//获取蓝牙模块MAC地址(hm-11)
#else	
		SetUartCmn_BluetoothTxMsg("AT+ADDE?",strlen("AT+ADDE?"));//获取蓝牙模块MAC地址(hm-13)
#endif
		while((Se_u_BleMacFlg != 1) && (--Le_dw_WaitTimer));//等待获取蓝牙mac地址

		if(Se_u_BleMacFlg == 1)
		{
			printf("获取蓝牙mac地址成功\r\n");
			(void)MemIf_WriteEE(EepromCfg_MacInfo,Se_u_MacAddr,sizeof(Se_u_MacAddr));//mac地址写入eeprom/flash
		}
		else
		{
			printf("获取蓝牙mac地址失败  X\r\n");
#ifdef  HYM8563
			if(hym8563_read_datetime(&Ve_h_tm) == 0)
			{
				printf("读取当前时间成功\r\n");
			}
#else
			Read_RTC_TimeAndDate(&Ve_h_tm);//读取日期时间
#endif
		}
	}
}

/******************************************************
*函数名：TskBleUnlock_MainFunction

*形  参：

*返回值：

*描  述：蓝牙开锁

*备  注：
******************************************************/
void TskBleUnlock_MainFunction(void)
{ 
	uint16_t Le_w_i;
	struct rtc_time Le_h_tm;
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
	static uint16_t Ble_openDoor_cnt = 0;
	if(SeCardSet_u_BleSt == 0U)//蓝牙模块空闲
	{
		SeCardSet_u_BleSt = 1U;
		SetUartCmn_BluetoothTxMsg("AT+DISA?",strlen("AT+DISA?"));//给蓝牙模块发送扫描指令
		SeCardSet_w_BleDisCnntT = 0U;
		//USART_PRINTF_S("\n\r发送蓝牙扫描指令 AT+DISA?");
	}
	
	/*处理蓝牙发来的时间校准标志*/
	if(0U == Getntpclient_u_NtpCalTimeSt())
	{//非ntp校时时，检查是否有蓝牙校时请求
		if(0 == set_time_success)
		{
			if(set_time_flag == 1)
			{
				set_time_flag = 0;
				USART_PRINTF_D("蓝牙校时，蓝牙时间:%s\n",timelist);
				timestamp_timeCalibration(timelist,TIME_STAMP_BJT);//校准时钟芯片时间
				set_time_success = 1;
				SeCardSet_dw_setTimeCnt = 0;
			}
		}
		else
		{//蓝牙校时完成后，距离下次校时延迟5s
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
	
	/**蓝牙开锁**/
	if((ble_flag == 1) && (0U == b_5s_flag))
	{
		b_5s_flag = 1U;//5s延时标志，此期间忽略接收到的开门指令
		b_5s_count = 0U;//5s延时计时器
		ble_flag = 0;
		if(SeZR60_u_NetUnlockflag != 1U)//表示一段时间内无网络开门
		{
#ifdef  HYM8563
			(void)hym8563_read_datetime(&Le_h_tm);//读取hym8563日期时间
#else
			Read_RTC_TimeAndDate(&Le_h_tm);//读取日期时间
#endif
			USART_PRINTF_DATE("读取的时间：%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
							  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
			SeCardSet_dw_nowtime = GetTick(&Le_h_tm);//生成时间戳
			Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_BLE,machine_type);
			if(Se_u_PasswordCmpFlag!= 1)//表示收到密码比对没有成功
			{
				USART_PRINTF_S("蓝牙密码验证失败 X");
				USART_PRINTF_D("set_time_flag:%d\n",set_time_flag);
				USART_PRINTF_D("蓝牙时间:%s\n",timelist);
				SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
			}
			else
			{
				Ble_openDoor_cnt++;
				USART_PRINTF_D("蓝牙 -> 开门 %d 次\r\n",Ble_openDoor_cnt);
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
		{//检测到最近一段时间内发生过网络远程开门，若检测门锁关闭，执行开门
			if(0U == SeZR60_u_DoorLockRelaySt)
			{//门锁关闭状态
#ifdef  HYM8563
				(void)hym8563_read_datetime(&Le_h_tm);//读取hym8563日期时间
#else
				Read_RTC_TimeAndDate(&Le_h_tm);//读取日期时间
#endif
				USART_PRINTF_DATE("读取的时间：%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
								  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
				SeCardSet_dw_nowtime = GetTick(&Le_h_tm);//生成时间戳
				Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_BLE,machine_type);
				if(Se_u_PasswordCmpFlag!= 1)//表示收到密码比对没有成功
				{
					USART_PRINTF_S("蓝牙密码验证失败 X");
					USART_PRINTF_D("set_time_flag:%d\n",set_time_flag);
					//USART_PRINTF_D("蓝牙时间:%s\n",timelist);
					SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
				}
				else
				{
					USART_PRINTF_S("蓝牙 -> 开门");
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
	
	if(1U == SeZR60_u_BleLogFltrflag)//上传蓝牙开门日志
	{
		if(1U == Se_u_PhoneNumFlg)//接收到蓝牙手机号
		{
			Se_u_PhoneNumFlg = 0U;
			SeZR60_u_BleLogFltrflag = 0U;
			USART_PRINTF_D("接收的手机号：%s\n",Se_u_PhoneNum);
			char temp = 33;
			for(Le_w_i=0U;Le_w_i < 11U;Le_w_i++)
			{//手机号码解码
				Le_u_UnlockInfo[Le_w_i] = Se_u_PhoneNum[Le_w_i] - temp;
				temp = temp - 2;
			}
			USART_PRINTF_D("接收的手机号解码后：%s\n",Le_u_UnlockInfo);
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_BLE);
		}
	}
	else
	{
		Se_u_PhoneNumFlg = 0U;
	}
	
	
	if(1U == b_5s_flag)//蓝牙开门时，计时（用于处理（清除）5s内接收到的蓝牙开门指令）
	{
		b_5s_count++;
		if(b_5s_count >= (5000/ZR60CTRL_SCHEDULING_CYCLE))//5s计时
		{//5s计时时间到，清b_5s_flag标志以允许蓝牙开门
			b_5s_count = 0U;
			ble_flag = 0U;//清开门指令
			b_5s_flag = 0U;
			Se_u_PhoneNumFlg = 0U;
		}
	}
	else
	{
		b_5s_count = 0U;
	}
	
	if(1U == SeCardSet_u_BleSt)//蓝牙模块非空闲时，计时8s(用于蓝牙串口通信超时处理)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (8000/ZR60CTRL_SCHEDULING_CYCLE))//蓝牙串口通信超时
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
	{//蓝牙或网络开门
		if(w_BleNetExcUnlockDelayTimer >= (3000/ZR60CTRL_SCHEDULING_CYCLE))//3s，期间忽略接收到的蓝牙或网络开门请求
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
*函数名：ClrZR60Ctrl_BleSt

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void ClrZR60Ctrl_BleSt(void)
{
	SeCardSet_u_BleSt = 0U;
}

/******************************************************
*函数名：ClrZR60Ctrl_BleCnntTimeout

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void ClrZR60Ctrl_BleCnntTimeout(void)
{
	SeCardSet_w_BleDisCnntT = 0U;
}




/******************************************************
*函数名：ZR60Ctrl_u_ArrayCmp

*形  参：

*返回值：

*描  述：数组比较

*备  注：
******************************************************/
char ZR60Ctrl_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len)
{
	unsigned int Le_w_i;
	for(Le_w_i = 0; Le_w_i < len; Le_w_i++)
		if(a[Le_w_i]!=b[Le_w_i]) return 0;//出现不相等元素，返回0.
	return 1;//完全相等，返回1。
}


/******************************************************
*函数名：GetZR60Ctrl_u_XOR

*形  参：

*返回值：

*描  述：异或取反校验

*备  注：
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
	USART_PRINTF_S("mac地址：");
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
	记录开锁信息
*/
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{
	uint64_t Time = 0U;
#ifdef  HYM8563
	hym8563_Currenttime(&Time);
#else
	RTC_Currenttime(&Time);
#endif
	WrUnlockLogCache_Queue(&Time,Le_u_UnlockInfo,Le_u_UnlockEvent);//开锁日志记录写入缓存队列
}
