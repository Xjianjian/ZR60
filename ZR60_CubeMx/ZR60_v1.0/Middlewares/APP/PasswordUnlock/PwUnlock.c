/******************************************************
文件名：	PwUnlock

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "PasswordUnlock/PwUnlock.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static uint8  keyValue_buffer[12];//缓存触摸按键信息
static uint8  u_TouchKeyScanflag = 1;//允许读取触摸按键值标志
static uint8  key_num = 0;//读取触摸按键值个数

static uint8  SeCardSet_u_PasswordKeySt = 0U;//密码按键状态
static uint8  SeCardSet_u_BtnFltrFlg = 0U;//无效按键滤除标志
static uint16 SeCardSet_w_ClrTimer = 0U;//清按键值接收buff计时器


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static char  ZR60Ctrl_TonesPlay(uint16 temp);

static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);

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
void InitPwUnlock_parameter(void)
{
	
}

/******************************************************
*函数名：TskZR60Ctrl_MainFunction

*形  参：

*返回值：

*描  述：串口接收

*备  注：串口接收中断调用
******************************************************/
void TskPwUnlock_MainFunction(void)
{
	uint8  rcvbuf[2]={0x00};
	uint8  machine_type;
	uint32 timestamp;
	char  u_ActualKeyValue = 0U;//iic读取的键值转换后的实际触摸按键值
	uint16 w_iickeyVlaue = 0;
	uint32 Le_dw_password = 0;//临时密码
	PwUnlockCfg_rtcTime rtcTime;
	uint8 ret;
	if(u_TouchKeyScanflag == 1)
	{
		if((SeCardSet_u_PasswordKeySt == STD_ACTIVE) &&(0U == SeCardSet_u_BtnFltrFlg))//按键
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
						case 'C'://按键 -> 取消
						{
							PWUNLOCK_PRINTF_S("\n键值=>取消");
							key_num = 0;
							memset(keyValue_buffer,0,sizeof(keyValue_buffer));	
						}
						break;
						case 'A'://按键 -> 确认
						{
							PWUNLOCK_PRINTF_S("\n键值=>确认");
							if(key_num == 6U)
							{//当前为临时密码开门
								PwUnlockCfg_datetime(&rtcTime,&timestamp);
								PWUNLOCK_PRINTF_DATE("\n读取的时间：%d.%d.%d %d:%d:%d\n",rtcTime.tm_year, \
								rtcTime.tm_mon, rtcTime.tm_mday,rtcTime.tm_hour,rtcTime.tm_min,rtcTime.tm_sec);
								machine_type = PwUnlockCfg_u_machineType();
								Le_dw_password = (keyValue_buffer[0] - 0x30)*100000 + (keyValue_buffer[1] - 0x30)*10000 + \
												 (keyValue_buffer[2] - 0x30)*1000 + (keyValue_buffer[3] - 0x30)*100 + \
												 (keyValue_buffer[4] - 0x30)*10 + (keyValue_buffer[5] - 0x30);
								if(PwUnlockCfg_PasswordAuth(timestamp,machine_type,Le_dw_password) == 1)
								{									
									PWUNLOCK_PRINTF_S("\n临时密码 -> 开门");
									SetPwUnlockCfg_openDoor();
									PwUnlockCfg_Play(PW_UNLOCK_DOOROPEN);
									
									/*记录开锁信息*/
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
							{//当前为固件刷新
								if(strcmp(keyValue_buffer,"110911060")==0)
								{
									PWUNLOCK_PRINTF_S("\n当前进入主控升级");
									//SeCardSet_u_MasterCtrlIap = 1U;//主控在线升级标志
									
								}
								else if(strcmp(keyValue_buffer,"110911061")==0)
								{
									PWUNLOCK_PRINTF_S("\n读卡器升级");
									//SeCardSet_u_CardReaderIap = 1U;//读卡器模块在线升级标志
									
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
							PWUNLOCK_PRINTF_D("\n键值 %d",(u_ActualKeyValue -0x30));	
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
				PWUNLOCK_PRINTF_D("\nErrorLogging：读取按键值失败,返回值：%d",ret);
			}
		}
		else
		{
			if(SeCardSet_u_PasswordKeySt == STD_ACTIVE)
			{
				//PWUNLOCK_PRINTF_S("\n无效按键中断滤除");
				SeCardSet_u_BtnFltrFlg = 0U;
				SeCardSet_u_PasswordKeySt = 0U;
			}
		}
	}
	else
	{
		SeCardSet_u_PasswordKeySt = 0U;
	}

	
	if(SeCardSet_w_ClrTimer >= (12000/PW_UNLOCK_SCHEDULING_CYCLE))//10ms调度周期，大概12s
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
*函数名：SetZR60Ctrl_PasswordKey

*形  参：

*返回值：

*描  述：设置password 按键状态

*备  注：
******************************************************/
void SetZR60Ctrl_PasswordKey(void)
{
	SeCardSet_u_PasswordKeySt = STD_ACTIVE;
}

/******************************************************
*函数名：return_data

*形  参：

*返回值：

*描  述：

*备  注：
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
	记录开锁信息
*/
static void ZR60Ctrl_u_RecordUnlockInfo(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent)
{

}

