/******************************************************
文件名：	ZR60_Ctrl

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "ZR60_Ctrl.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
const char CeZR60Ctrl_u_SoftVersion[10U] = {'Z','R','6','0','_','v','1','.','2','\0'};//软件版本
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
static uint8_t  ZR60Ctrl_TonesPlay(uint16_t temp);
static void  CardSet_MainFunction(void);//母卡设置
static void  OpenDoor_IC_MainFunction(void);//开锁
static void  OpenDoor_Password_MainFunction(void);//密码验证
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
description： function code
******************************************************/
/******************************************************
*函数名：InitZR60Ctrl_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitZR60Ctrl_parameter(void)
{
	uint8_t  Le_u_i;
	uint64_t Le_dw_WaitTimer = 0x1FFFFF;
	uint8_t LeZR60_u_Xor = 0;
	
	LeZR60_u_Xor = GetMemIf_u_CheckSum(EepromCfg_CardInfo);//读取扇区0数据校验和（存放母卡信息）
	if((LeZR60_u_Xor == MemIf_ReadEE(EepromCfg_CardInfo,sminfo.community_id,sizeof(sminfo))) && \
	  (STD_ACTIVE == GetMemIf_u_DtVild(EepromCfg_CardInfo)))
	{
		printf("\n【3】flash存储的母卡信息数据有效 √\r\n");
		SetMemIf_EEVild(EepromCfg_CardInfo);//数据有效
		for(Le_u_i = 0U;Le_u_i < 16U;Le_u_i++)
		{
			sminfo1.community_id[Le_u_i] = sminfo.community_id[Le_u_i];
			sminfo1.build_numOne[Le_u_i] = sminfo.build_numOne[Le_u_i];
			sminfo1.build_numTwo[Le_u_i] = sminfo.build_numTwo[Le_u_i];
			sminfo1.build_id[Le_u_i] = sminfo.build_id[Le_u_i];
			sminfo1.door_id[Le_u_i] = sminfo.door_id[Le_u_i];
		}
		sminfo1.suffix = sminfo.suffix;
		read_init_sminfo(&sminfo1,&sm);//读取母卡文件的数据，并截取ic卡小区号，楼栋号，单元号的前8位用于sm3密码验证
	}
	else
	{
		printf("flash存储的母卡信息数据无效 X\r\n");
	}
	
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
		SetUartCmn_BluetoothTxMsg("AT+ADDR?",strlen("AT+ADDR?"));//获取蓝牙模块MAC地址
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
	//dw_5msflag_T1 = GetSysTick_Timer();
	//dw_5msflag_T2 = dw_5msflag_T1;
}

/******************************************************
*函数名：TskZR60Ctrl_MainFunction

*形  参：

*返回值：

*描  述：串口接收

*备  注：串口接收中断调用
******************************************************/
void TskZR60Ctrl_MainFunction(void)
{ 
	
	/*************************************************
	*		            母卡设置
	*************************************************/	
	CardSet_MainFunction();//母卡设置	
	
	/*************************************************
	*		            ic卡开锁
	*************************************************/
	OpenDoor_IC_MainFunction();//ic卡开锁	

	/*************************************************
	*		            临时密码开锁
	*************************************************/	
	OpenDoor_Password_MainFunction();//临时密码开锁
	
	/*************************************************
	*		            门内开关操作
	*************************************************/
	OpenDoor_Switch_MainFunction();
		
	/*************************************************
	*		            网络开门
	*************************************************/	
	OpenDoor_Net_MainFunction();
	
	/*************************************************
	*		            消防报警
	*************************************************/	
	OpenDoor_Fire_MainFunction();
	
	/*************************************************
	*		            声波开锁
	*************************************************/	
	//OpenDoor_Sound_MainFunction();
	
	/*************************************************
	*		           蓝牙开锁
	*************************************************/		
	OpenDoor_Ble_MainFunction();
	
	/*************************************************
	*		          微信开锁
	*************************************************/		
	OpenDoor_WX_MainFunction();
	
	/*************************************************
	*		            门锁状态指示灯控制
	*************************************************/	
	ZR60Ctrl_u_LedDisplay();
	
	/*************************************************
	*		            延时关门控制
	*************************************************/	
	w_1sflag_DelayTime++;
	if(w_1sflag_DelayTime >= (1000/ZR60CTRL_SCHEDULING_CYCLE))
	{
		w_1sflag_DelayTime = 0U;
		if(0U == Se_u_CloseDoorDelayTimeSetFlag)
		{//关门延时时间未设置或需要重新设置,设置延时关门时间
			/**读取电位器的电压,根据电压设置关门延时时间**/
			Voltage_value[0U] = (uint16)((uint32)GetADFliter_ADFliterResult(0)*3300/4096);//每隔1s读取电位器的电压(电压放大1000倍)
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
					USART_PRINTF_D("延时关门时间设置完成,时间为：%dms\n",Se_w_DoorLockdelayT);
				}
			}
			else
			{
				Voltage_value[1U] = Voltage_value[0U];
				Se_u_VoltageDifferencCnt = 0U;
			}
			USART_PRINTF_D("AD采样值：%d\n",GetADFliter_ADFliterResult(0));
			USART_PRINTF_D("电压放大1000倍值：%d\n",Voltage_value[0]);
		}
		else
		{//检查电压差变化，超过设置门限值，重新设置关门延时时间
			Voltage_value[0U] = (uint16)((uint32)GetADFliter_ADFliterResult(0)*3300/4096);//每隔1s读取电位器的电压(电压放大1000倍)
			if(ZR60Ctrl_w_CalPosDifference(Voltage_value[0U],Voltage_value[1U]) >= 250U)//电压变化0.25v
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
	{//消防报警关闭状态下，执行正常关门
		Se_w_LockDoorTimer++;
		if(Se_w_LockDoorTimer >= (Se_w_DoorLockdelayT/ZR60CTRL_SCHEDULING_CYCLE))
		{		
			LOCK_OFF;
			Se_w_LockDoorTimer = 0U;
			SeZR60_u_DoorLockRelaySt = 0U;
			USART_PRINTF_S("门锁状态 -> 关闭");
		}
	}

#ifdef UART_DEBUG	
	/*************************************************
	*		          打印时间  
	*************************************************/	
	struct rtc_time Le_h_tm;
	w_500msflag_DelayTime++;
	if(w_500msflag_DelayTime >= (15000/ZR60CTRL_SCHEDULING_CYCLE))
	{
		w_500msflag_DelayTime = 0U;
#ifdef  HYM8563
		(void)hym8563_read_datetime(&Le_h_tm);//读取hym8563日期时间
#else
		Read_RTC_TimeAndDate(&Le_h_tm);//读取日期时间
#endif
		USART_PRINTF_DATE("当前时间：%d.%d.%d %d:%d:%d\n",Le_h_tm.tm_year,Le_h_tm.tm_mon, \
					  Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec);
	}
#endif
	
	
#if 0
	/*************************************************
	*		            lcd1062显示任务
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
*函数名：CardSet_MainFunction

*形  参：

*返回值：

*描  述：母卡设置

*备  注：
******************************************************/
static void  CardSet_MainFunction(void)//母卡设置
{
	uint8_t Le_u_i;
	static uint32_t SeCardSet_dw_Timer = 0U;
	switch(SeCardSet_u_St)
	{
		case 0U://空闲状态
		{
			if(GetBtnFltr_u_BtnShrtSt(Key_card_set) == STD_ACTIVE)//母卡设置按键有效
			{
				ClrBtnFltr_BtnShrtValid(Key_card_set);
				SetAudioIO_PlayFile(AudioIO_ConfDevice,2U);
				SeCardSet_dw_Timer = 0U;
				SeCardSet_u_St = 1U;
				USART_PRINTF_S("进入母卡设置模式");
			}
		}
		break;
		case 1U://等待母卡设置状态
		{
			SeCardSet_dw_Timer++;
			if(SeCardSet_dw_Timer <= (9000/ZR60CTRL_SCHEDULING_CYCLE))
			{
				if(ZR60_CTRL_SETCARD == 1)
				{
					ZR60_CTRL_SETCARD = 0;	
					if(0U == ZR60Ctrl_u_ArrayCmp(sminfo.community_id,sminfo1.community_id,sizeof(sminfo1)))//检查母卡信息有更新
					{//母卡信息有更新则写flash,设置重新初始化设备
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
							read_init_sminfo(&sminfo1,&sm);//根据ic卡小区号，楼栋号，单元号的前8位用于sm3密码验证
							tcp_LngConnect_Parameter();
							//Settcp_client_DeviceInit();
						}
					}
					Settcp_client_DeviceInit();
					SetAudioIO_PlayFile(AudioIO_ConfSuccess,2U);
					memset(test_buf,0,sizeof(test_buf));//test_buf[]清0		
					SeCardSet_u_St = 0U;
					ZR60_CTRL_AUTOSEARCHCARD = 1;//开启自动寻卡
					USART_PRINTF_S("母卡设置成功  √");
				}
			}
			else//等待超时
			{
				SeCardSet_u_St = 0U;
				USART_PRINTF_S("等待母卡设置超时，退出母卡设置模式");
				SetAudioIO_PlayFile(AudioIO_ConfCancel,2U);
			}
		}
		break;
		default:
		break;
	}
}


/******************************************************
*函数名：OpenDoor_IC_MainFunction

*形  参：

*返回值：

*描  述：开锁

*备  注：
******************************************************/
static void  OpenDoor_IC_MainFunction(void)//开锁
{
	//uint64_t Time;
	char Le_u_UnlockInfo[32U] = {0x00};
	
	/*设置读ic卡密码*/
	if((0U == ZR60_CTRL_SETKEY) && (1U == GetUartCmn_u_TxIdle()))
	{
		Setreadcard_Key();//设置读卡密码
	}
	
	/*处理接收的ic卡数据*/
	if(GetUartCmn_u_RxMsgRenew() == 1)//收到读卡器发来的一帧数据
	{
		GetUartCmn_u_RxMsg(rebackinfo.Dt.Block);
		if(rebackinfo.Dt.DtSrt.Seqnr == 0xFF)
		{//在线升级，从机响应
			Setstm8_fm17550_SlaveReplySt(rebackinfo.Dt.DtSrt.St);
		}
		else
		{
			handleMsg(rebackinfo); //处理读卡数据	
		}
	}
	
	if(1U != SeCardSet_u_St)//非卡片设置状态
	{
		if(open_door == 1)//开锁
		{	
			USART_PRINTF_S("ic卡 -> 开门");
			open_door = 0;
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			Se_w_LockDoorTimer = 0;	
			ZR60_CTRL_AUTOSEARCHCARD = 1;//开启自动寻卡
			w_SetAutoCard_DelayTime = 0U;
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);	
			
			/*记录开锁信息*/
			Json_HexToStr(Le_u_UnlockInfo,SeReadcard_h_CardID.CardID,SeReadcard_h_CardID.lng);
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_CARD);
		}
	}
	
	if(ZR60_CTRL_AUTOSEARCHCARD == 1)//自动寻卡设置
	{
		if(w_SetAutoCard_DelayTime >= (2500/ZR60CTRL_SCHEDULING_CYCLE))//延时2.5s下发自动寻卡设置
		{
			if(1U == GetUartCmn_u_TxIdle())//串口tx空闲
			{
				w_SetAutoCard_DelayTime = 0U;
				Setreadcard_AutoSearchCard();//下发自动寻卡指令
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
*函数名：OpenDoor_Sound_MainFunction

*形  参：

*返回值：

*描  述：声波开锁

*备  注：
******************************************************/
static void  OpenDoor_Sound_MainFunction(void)
{
	struct rtc_time Le_h_tm;
	uint64_t Time = 0U;
	if(receive_flag == 1)//声波开锁标志有效
	{
		receive_flag = 0;
		if(0U == SeZR60_u_DoorLockRelaySt)
		{//门锁关闭状态
#ifdef  HYM8563
			(void)hym8563_read_datetime(&Le_h_tm);//读取hym8563日期时间
#else
			Read_RTC_TimeAndDate(&Le_h_tm);//读取日期时间
#endif
			SeCardSet_dw_nowtime = GetTick(&Le_h_tm);
			Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_SOUND,machine_type);//生成密码比对
			if(Se_u_PasswordCmpFlag != 1)
			{
				SetAudioIO_PlayFile(AudioIO_ErrKey,1U);
			}
			else
			{
				/*记录开锁信息*/
#ifdef  HYM8563
				hym8563_Currenttime(&Time);
#else
				RTC_Currenttime(&Time);
#endif
				WrUnlockLogCache_Queue(&Time,"unknown",UNLOCKLOGCACHE_SOUNDWAVE);//开锁日志记录写入缓存队列

				USART_PRINTF_S("声波 -> 开门");
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
			USART_PRINTF_S("门锁已开");
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
		}
	}	
}

/******************************************************
*函数名：OpenDoor_Ble_MainFunction

*形  参：

*返回值：

*描  述：蓝牙开锁

*备  注：
******************************************************/
static void  OpenDoor_Ble_MainFunction(void)
{
	uint16_t Le_w_i;
	struct rtc_time Le_h_tm;
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
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
		if(set_time_flag == 1)
		{
			USART_PRINTF_D("蓝牙校时，蓝牙时间:%s\n",timelist);
			timestamp_timeCalibration(timelist,TIME_STAMP_BJT);//校准时钟芯片时间
			set_time_flag = 0;
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
				USART_PRINTF_S("蓝牙 -> 开门");
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
					SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
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
	
	if(1U == SeCardSet_u_BleSt)//蓝牙模块非空闲时，计时4s(用于蓝牙串口通信超时处理)
	{
		SeCardSet_w_BleDisCnntT++;
		if(SeCardSet_w_BleDisCnntT >= (4000/ZR60CTRL_SCHEDULING_CYCLE))//蓝牙串口通信超时
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
*函数名：OpenDoor_Password_MainFunction

*形  参：

*返回值：

*描  述：密码验证

*备  注：
******************************************************/
static void  OpenDoor_Password_MainFunction(void)//密码验证
{
	struct rtc_time tm;
	//uint64_t Time;
	//char Le_u_UnlockInfo[32U] = {0x00};
	uint8_t  buf[2]={0x20,0x00};
	static uint16_t SeCardSet_w_ClrTimer = 0U;//清按键值接收buff计时器
	//static uint8_t SeCardSet_u_BtnFltrFlg = 0U;//无效按键滤除标志
	
	if(u_TouchKeyScanflag == 1)
	{
		if((SeCardSet_u_PasswordKeySt == STD_ACTIVE) &&(0U == SeCardSet_u_BtnFltrFlg))//按键
		{
			USART_PRINTF_S("\r\n按键触发\r\n");
			SeCardSet_u_PasswordKeySt = 0U;
			SeCardSet_w_ClrTimer = 0U;
			//读取触摸按键值的iic接口需要配置
			if(i2c_master_reg8_recv(0x6E, 0xAA, buf, 2) == 0)
			{
				sec_count = 0;
				w_iickeyVlaue = buf[1];
				w_iickeyVlaue = (w_iickeyVlaue << 8) | buf[0];
				u_ActualKeyValue = ZR60Ctrl_TonesPlay(w_iickeyVlaue);
				if(u_ActualKeyValue != 0xff)
				{
					SeCardSet_u_BtnFltrFlg = 1U;
					if(u_ActualKeyValue == 'C')//按键 -> 取消
					{
						key_num = 0;
						memset(test_buf,0,sizeof(test_buf));
						//sm3_flag = 1;		
					}
					else if(u_ActualKeyValue == 'A')//按键 -> 确认
					{ 												
						if(key_num == 6)
						{//当前为临时密码开门
							strncpy(key_buf,test_buf,6);
							key_flag = 1;
							//state = 0;	
						}
						else if(key_num == 9)
						{//当前为固件刷新
							if(strcmp(test_buf,"110911060")==0)
							{
								USART_PRINTF_S("当前进入主控升级");
								SeCardSet_u_MasterCtrlIap = 1U;//主控在线升级标志
								
							}
							else if(strcmp(test_buf,"110911061")==0)
							{
								USART_PRINTF_S("读卡器升级");
								SeCardSet_u_CardReaderIap = 1U;//读卡器模块在线升级标志
								
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
				USART_PRINTF_S("\r\nErrorLogging：读取按键值失败\r\n");
			}
		}
		else
		{
			if(SeCardSet_u_PasswordKeySt == STD_ACTIVE)
			{
				//USART_PRINTF_S("\r\n无效按键中断滤除\r\n");
				SeCardSet_u_BtnFltrFlg = 0U;
				SeCardSet_u_PasswordKeySt = 0U;
			}
		}
	}
	else
	{
		SeCardSet_u_PasswordKeySt = 0U;
	}
	
	if (key_flag == 1)//按键6位密码标志
	{
		key_flag = 0;
#ifdef  HYM8563
		(void)hym8563_read_datetime(&tm);//读取hym8563日期时间
#else
		Read_RTC_TimeAndDate(&tm);//读取日期时间
#endif
		SeCardSet_dw_nowtime = GetTick(&tm);
		Se_u_PasswordCmpFlag = sm3_time_PasswordAuth(SeCardSet_dw_nowtime,&sm,TIME_STAMP_PASSWORD,machine_type);//生成密码比对
		if(Se_u_PasswordCmpFlag != 1)
		{
			SetAudioIO_PlayFile(AudioIO_ErrKey,1U);		
		}
		else
		{
			USART_PRINTF_S("临时密码 -> 开门");
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			Se_w_LockDoorTimer = 0;
			memset(key_buf,0,6);
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
			
			/*记录开锁信息*/
			ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_TEMPPSWORD);
		}
	}
	
	if(SeCardSet_w_ClrTimer >= (12000/ZR60CTRL_SCHEDULING_CYCLE))//10ms调度周期，大概12s
	{
		key_num = 0;
		key_flag = 0;
		memset(key_buf,0,6);
		memset(test_buf,0,sizeof(test_buf));
		//USART_PRINTF_S("按键输入超时\r\n");
	}
	else
	{
		SeCardSet_w_ClrTimer++;
	}
}

/******************************************************
*函数名：OpenDoor_Switch_MainFunction

*形  参：

*返回值：

*描  述：门内开关开门

*备  注：
******************************************************/
static void  OpenDoor_Switch_MainFunction(void)
{
	//uint64_t Time = 0U;
	if( GetBtnFltr_u_BtnShrtSt(Key_Unlock) == STD_ACTIVE)//门内开关操作
	{
		USART_PRINTF_S("门内开关 -> 开门");
		ClrBtnFltr_BtnShrtValid(Key_Unlock);	
		LOCK_ON;
		Se_w_LockDoorTimer = 0;
		SeZR60_u_DoorLockRelaySt = 1U;
		SeZR60_u_DoorOpenLedEvent = 1U;
		SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);	
		
		/*记录开锁信息*/
		//ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_DOORSWITCH);
	}
}


/******************************************************
*函数名：OpenDoor_Fire_MainFunction

*形  参：

*返回值：

*描  述：消防开门

*备  注：
******************************************************/
static void  OpenDoor_Fire_MainFunction(void)
{
	//uint64_t Time = 0U;
	switch(Se_u_FireALARM)
	{
		case 0U:
		{//消防报警未触发状态
			if(GetPSFltr_u_PeriSt(Key_Alarm) == STD_ON)//消防报警
			{
				//ClrBtnFltr_BtnShrtValid(Key_Alarm);	
				USART_PRINTF_S("消防警报 -> 开门");
				LOCK_ON;//开门
				SeZR60_u_DoorLockRelaySt = 1U;
				USART_Cmd(UART5,  DISABLE);//关闭串口5中断
				USART_Cmd(USART3, DISABLE);//关闭串口3中断
				USART_Cmd(UART4,  DISABLE);//关闭串口4中断
				u_TouchKeyScanflag = 0;//关闭按键板读取
				Se_u_FireALARM = 1U;
				SeCardSet_dw_ALARMDlyTime = (2500/ZR60CTRL_SCHEDULING_CYCLE);
				
				//记录开锁信息
				//ZR60Ctrl_u_RecordUnlockInfo("unknown",UNLOCKLOGCACHE_FIREALARM);
			}		
		}
		break;
		case 1U:
		{//消防报警触发状态,掉电复位/重启后才恢复正常工作
			if(0U == GetAudioIO_u_PlaySt())
			{
				SeCardSet_dw_ALARMDlyTime++;
				if(SeCardSet_dw_ALARMDlyTime >= (2500/ZR60CTRL_SCHEDULING_CYCLE))//消防报警频率控制：2.5s/次
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
*函数名：OpenDoor_Net_MainFunction

*形  参：

*返回值：

*描  述：网络开门

*备  注：
******************************************************/
static void  OpenDoor_Net_MainFunction(void)
{
	char Le_u_UnlockInfo[32U] = {0};
	//uint64_t Time = 0U;
	if( 1U == ZR60_CTRL_REMOTE_UNLOCK)
	{
		USART_PRINTF_S("远程 -> 开门");
		ZR60_CTRL_REMOTE_UNLOCK = 0U;
		if(SeZR60_u_BleUnlockflag != 1U)//表示一段时间内无蓝牙开门
		{
			LOCK_ON;
			SeZR60_u_DoorLockRelaySt = 1U;
			SeZR60_u_DoorOpenLedEvent = 1U;
			SeZR60_u_NetUnlockflag = 1U;
			Se_w_LockDoorTimer = 0;
			SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);

			Gettcp_LngConnect_PhoneNo(Le_u_UnlockInfo);
			/*记录开锁信息*/
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_REMOTE);
		}
		else
		{//检测到最近一段时间内发生过蓝牙开门，若检测门锁关闭，执行开门
			if(0U == SeZR60_u_DoorLockRelaySt)
			{//门锁关闭状态
				LOCK_ON;
				SeZR60_u_DoorLockRelaySt = 1U;
				SeZR60_u_DoorOpenLedEvent = 1U;
				SeZR60_u_NetUnlockflag = 1U;
				Se_w_LockDoorTimer = 0;
				SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);
				Gettcp_LngConnect_PhoneNo(Le_u_UnlockInfo);		
				/*记录开锁信息*/
				ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_REMOTE);
			}
		}
	}	
}

/******************************************************
*函数名：OpenDoor_WX_MainFunction

*形  参：

*返回值：

*描  述：微信公众号开门

*备  注：
******************************************************/
static void  OpenDoor_WX_MainFunction(void)
{
	char Le_u_UnlockInfo[32U] = {0};
	
	if( 1U == ZR60_CTRL_WX_UNLOCK)
	{
		USART_PRINTF_S("微信公众号 -> 开门");
		ZR60_CTRL_WX_UNLOCK = 0U;

		LOCK_ON;
		Se_w_LockDoorTimer = 0;
		SeZR60_u_DoorLockRelaySt = 1U;
		SeZR60_u_DoorOpenLedEvent = 1U;
		SetAudioIO_PlayFile(AudioIO_DoorOpen,2U);

		if(1 == Gettcp_LngConnect_WXOpenInfo(Le_u_UnlockInfo))
		{
			/*记录开锁信息*/
			ZR60Ctrl_u_RecordUnlockInfo(Le_u_UnlockInfo,UNLOCKLOGCACHE_WECHAT);
		}
	}	
}

/******************************************************
*函数名：Lcd1062_MainFunction

*形  参：

*返回值：

*描  述：lcd1062显示

*备  注：
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
*函数名：return_data

*形  参：

*返回值：

*描  述：

*备  注：
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
			USART_PRINTF_S("\r\n键值 6\r\n");	
		}
		break;
        case 0x0002:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '9' ;
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 9\r\n");
		}
        break;
		case 0x0004:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  'A';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 确认\r\n");
		}
		break;
        case 0x0008:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '0';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 0\r\n");
		}
		break;
        case 0x0010:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '3';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 3\r\n");
		}
		break;
        case 0x0020:
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '2';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 2\r\n");
		break;
        case 0x0040:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			 ret =  '1';
			 //SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 1\r\n");
		}
		break;
        case 0x0080:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret =  '4';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 4\r\n");
		}
		break;
        case 0x1000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '7';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 7\r\n");
		}
		break;
        case 0x2000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = 'C';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 取消\r\n");
		}
		break;
        case 0x4000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '8';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 8\r\n");
		}
		break;
        case 0x8000:
		{
			SetAudioIO_PlayFile(AudioIO_keyVoice,2U);
			ret = '5';
			//SeCardSet_u_BtnFltrFlg = 1U;
			USART_PRINTF_S("\r\n键值 5\r\n");
		}
		break;
		default:
		{
			ret = 0xff;
			USART_PRINTF_S("无效键值");
		}
		break;
    }
		return ret;
}


/******************************************************
*函数名：GetZR60Ctrl_u_Alarm

*形  参：

*返回值：

*描  述：获取消防报警状态

*备  注：
******************************************************/
uint8_t	GetZR60Ctrl_u_Alarm(void)
{
	return Se_u_FireALARM;
}


/******************************************************
*函数名：GetZR60Ctrl_u_CardSetSt

*形  参：

*返回值：

*描  述：获取卡片设置状态

*备  注：
******************************************************/
uint8_t	GetZR60Ctrl_u_CardSetSt(void)
{
	return SeCardSet_u_St;
}

/******************************************************
*函数名：SetZR60Ctrl_u_PasswordKey

*形  参：

*返回值：

*描  述：设置password 按键状态

*备  注：
******************************************************/
void SetZR60Ctrl_u_PasswordKey(void)
{
	SeCardSet_u_PasswordKeySt = 1U;
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
*函数名：

*形  参：

*返回值：

*描  述：求两个数正的差值

*备  注：
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


/*
	指示灯状态显示
*/
static void ZR60Ctrl_u_LedDisplay(void)
{
	static uint8_t Le_u_cnt = 0U;
	static uint8_t Le_u_LedSt = 0U;//0--熄灭；1--点亮
	static uint16_t Le_w_timer = 0U;
	if((0 != GET_PHY_LINK_STATUS()) && (Se_u_FireALARM == 0U))//正常工作状态
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
				if(Le_w_timer >= (100/ZR60CTRL_SCHEDULING_CYCLE))//大概100ms
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
	else//故障状态
	{//绿灯持续闪烁（间隔200ms）
		if(Le_w_timer >= (200/ZR60CTRL_SCHEDULING_CYCLE))//大概200ms
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
*函数名：GetZR60Ctrl_u_MastCtrlIapSt

*形  参：

*返回值：

*描  述：获取主控在线升级标志

*备  注：
******************************************************/
uint8_t	GetZR60Ctrl_u_MastCtrlIapSt(void)
{
	return SeCardSet_u_MasterCtrlIap;
}


/******************************************************
*函数名：ClrZR60Ctrl_MastCtrlIapSt

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void ClrZR60Ctrl_MastCtrlIapSt(void)
{
	SeCardSet_u_MasterCtrlIap = 0U;
}

/******************************************************
*函数名：GetZR60Ctrl_u_CardReaderIapSt

*形  参：

*返回值：

*描  述：获取读卡器在线升级标志

*备  注：
******************************************************/
uint8_t	GetZR60Ctrl_u_CardReaderIapSt(void)
{
	return SeCardSet_u_CardReaderIap;
}

/******************************************************
*函数名：ClrZR60Ctrl_CardReaderIapSt

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void ClrZR60Ctrl_CardReaderIapSt(void)
{
	SeCardSet_u_CardReaderIap = 0U;
}

