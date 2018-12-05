/******************************************************
文件名：	time_stamp.c

描述：		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "./timeStamp/time_stamp.h"
#include "./sm3/SM3.h"
#include "time.h"
#include "./led/bsp_led.h"  
#include "./systick/bsp_SysTick.h"
#include "wm8978/bsp_wm8978.h"
#include "./wdata/wdata.h"
#include "./beep/bsp_beep.h" 
#include  "math.h"
#include  "Include.h"

u8 key_buf[6];


/*
	生成时间戳
	@param:
			tm 当前日期时间
	@return: 时间戳值
*/
uint32_t GetTick(struct rtc_time *tm)  
{  
    struct tm stm;  

    stm.tm_year = tm->tm_year - 1900;  
    stm.tm_mon = tm->tm_mon - 1;  
    stm.tm_mday = tm->tm_mday;  
    stm.tm_hour = tm->tm_hour - 8;  
    stm.tm_min = tm->tm_min;  
    stm.tm_sec = tm->tm_sec;  
  
    return mktime(&stm);  
}

/*
	时间戳转为字符串
	@param:
			timestamp  时间戳值
			ftime 当前日期时间(字符串形式的世界时间)
			
	@return: 时间戳值
*/
void timestamp_strBJtime(unsigned long timestamp, char *ftime)
{
	struct tm *tm_t;
	timestamp = timestamp + 28800;//补充8小时秒数
	tm_t = localtime(&timestamp);
	strftime(ftime,15,"%Y%m%d%H%M%S",tm_t);
	USART_PRINTF_D("时间戳转换的当前时间：%s",ftime);
}

/*
	校准时钟芯片时间
	@param:
			tm 当前日期时间(字符串形式)
			tmType  时间类型：0--北京时间；1--世界时间
*/
void timestamp_timeCalibration(char *tm,uint8_t tmType)  
{
	struct rtc_time stm;
	time_stamp_Date Date;
	u8 i;
	u8 buf = 0U;
	
	for(i = 0;i < 14U;i++)
	{
		Date.currentdate[i] = tm[i];
		Date.currentdate[i] = (uint8_t)(Date.currentdate[i] - 0x30);
	}
	
	stm.tm_year = Date.date.year[0U]*1000 + Date.date.year[1U]*100 + Date.date.year[2U]*10 + Date.date.year[3U];  
	stm.tm_mon  = (Date.date.mon[0U]*10  + Date.date.mon[1U]);
	stm.tm_mday = Date.date.mday[0U]*10 + Date.date.mday[1U];
	if(TIME_STAMP_UNT == tmType)
	{
		stm.tm_hour = Date.date.hour[0U]*10 + Date.date.hour[1U] +8;
	}
	else
	{
		stm.tm_hour = Date.date.hour[0U]*10 + Date.date.hour[1U];
	}
	stm.tm_min  = Date.date.min[0U]*10  + Date.date.min[1U];
	stm.tm_sec  = Date.date.sec[0U]*10  + Date.date.sec[1U];
	USART_PRINTF_DATE("设置的时间：%d.%d.%d %d:%d:%d\n",stm.tm_year,stm.tm_mon,  \
					  stm.tm_mday,stm.tm_hour,stm.tm_min,stm.tm_sec);
#ifdef  HYM8563
	buf |= STOP;
	if (hym8563_i2c_set_regs(RTC_CTL1, &buf, 1) == 0)
	{
		USART_PRINTF_S("关闭hym8563成功  √\n");
	}
	/****设置日期、时间****/	
	if(hym8563_set_time(&stm) == 0)
	{
		USART_PRINTF_S("set hym8563 time成功  √\n");
	}
	/****读取日期、时间****/	
	buf = 0U;
	if (hym8563_i2c_set_regs(RTC_CTL1, &buf, 1) == 0)
	{
		USART_PRINTF_S("开启HYM863时钟  √\n");
	}
#else
	//使用片上rtc
	Set_RTC_TimeAndDate(&stm);
#endif
}


/*
	对接收的sm3密码处理得到用于验证的验证密码
	@param:
			type_value  原始密码
			mach_type  设备类型  
	@return: 用于验证的密码
*/
uint32_t  check_passwd(u32 type_value,u8 mach_type)
{
	 u32 now_value;
     u32 value1,value2,value3;

	switch(mach_type)
	{
		 case 1:
		 {
			value1 = type_value /100000;
			value2 = type_value / 1000 %10;
			value3 = type_value /10 % 10;
			now_value = value1 * 100 + value2 * 10 + value3;
			break;
		 }
		 case 2:
		 {
			// value1 = type_value /10000 %10;
			// value2 = type_value/ 100 %10;
			// value3 = type_value  % 10;
			// now_value = value1 * 100 + value2 * 10 + value3;
			now_value = type_value;
			break;
		 }
		 default:
		 {
			 now_value = 0;
			 break;
		 }		 		        
	 }
	return now_value;
}


/****根据机器类型生成不同的密码***
***
参数1：buf  所储存密码的6位数组
参数2：mach_type 机器类型
   1：表示围墙机
   2：表示门口机
@return: sm3密码
****/
uint32_t  generate_password(u32 *passwd2,u32 *passwd1)
{
	u32 now_passwd;
    u32 value1,value2,value3,value4,value5,value6;
		

	value1 = (*passwd1 /100) *100000;
	value2 = (*passwd2 / 100) *10000;
	value3 = (*passwd1 / 10 %10) * 1000;
	value4 = (*passwd2 / 10 %10) * 100;
	value5 = (*passwd1  %10) *10;
	value6 = *passwd2 % 10;
	now_passwd = value1 + value2 + value3 + value4 + value5 + value6 ;
	//USART_PRINTF_D("now_passwd is: %d\n",now_passwd);

	return now_passwd;
}

/*
	sm3密码验证
	@param:
			now_time  时间戳
			sm        母卡信息
			mach_type  设备类型  
	@return: sm3密码验证状态：0--失败；1--成功
*/
uint8_t sm3_time_PasswordAuth(uint32_t now_time,struct sm3_info *sm,u8 n,u8 mach_type)
{	
	u8 pass_success = 0;
	u8 type[3] = {64,64,8};
	unsigned char build_num[8U] ={'F','F','F','F','F','F','F','F'};
	unsigned long i_num ;
	u32 passwd ;
	u32 b_passwd;
	u32 test = 0;
	int i;	
	
	USART_PRINTF_D("时间戳now_time is: %d\n",now_time);
	 switch(n)
	 {
		 case TIME_STAMP_PASSWORD:
		 {
			 USART_PRINTF_S("key is:\n");	
			 USART_PRINTF_S(key_buf);	
			 passwd =  check_passwd(atoi(key_buf),mach_type);
			 i_num  = now_time/60 - 60;
		 }
		 break;
		 case TIME_STAMP_SOUND:
		 {
			  passwd = 	check_passwd(ulong,mach_type);
			  i_num  = now_time/60 - 60;
		 }
		 break;
		 case TIME_STAMP_BLE:
		 {
			  passwd = 	check_passwd(atoi(BleValue),mach_type);
			  i_num  = now_time/60 - 5;
		 }
		break;				 	 
	 }
	 USART_PRINTF_D("passwd is: %d\n",passwd);				
				
	for(i = 0; i < type[n];i++)
	{
		if(mach_type == 2)
		{	
			//USART_PRINTF_S("门口机密码验证...\n");
			test = get_sm3_pass(i_num, sm->community_id, sm->build_num, sm->cell_num);
			b_passwd = get_sm3_pass(i_num, sm->community_id,build_num, sm->cell_num);
			if(generate_password(&test,&b_passwd) == passwd)
			{
				USART_PRINTF_S("门口机密码验证...\n");
				USART_PRINTF_S("密码正确  √\n");
				pass_success = 1;
				return 1;	
			}
		}
		else
		{
			if(passwd == get_sm3_pass(i_num, sm->community_id, sm->build_num, sm->cell_num))
			{
				USART_PRINTF_S("围墙机密码验证...\n");
				USART_PRINTF_S("密码正确  √\n");
				pass_success = 1;
				return 1;	
			}
		}
		i_num++;
	}
				
	USART_PRINTF_D("sm3密码验证失败,sm3密码验证数据信息：%s\n",sm);
	return 0;
}





 
