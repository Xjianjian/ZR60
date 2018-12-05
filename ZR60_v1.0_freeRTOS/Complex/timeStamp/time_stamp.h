#ifndef __TIME_STAMP_H__
#define __TIME_STAMP_H__

#include "stm32f4xx.h"
#include "./hym8563/hym8563.h"
#include "./rtc/bsp_rtc.h"
#include "./wdata/wdata.h"


#define TIME_STAMP_PASSWORD  0
#define TIME_STAMP_SOUND     1
#define TIME_STAMP_BLE       2


#define TIME_STAMP_BJT     0//时间类型为北京时间
#define TIME_STAMP_UNT     1//时间类型为世界时间


typedef union
{
	unsigned char currentdate[14U];
	struct 
	{	
		unsigned char year[4U];
		unsigned char mon[2U];
		unsigned char mday[2U];
		unsigned char hour[2U];
		unsigned char min[2U];
		unsigned char sec[2U];
	}date;
}time_stamp_Date;


extern u8 key_buf[6];

extern uint32_t GetTick(struct rtc_time *tm);
extern uint32_t  check_passwd(u32 type_value,u8 mach_type);
extern uint8_t sm3_time_PasswordAuth(uint32_t now_time,struct sm3_info *sm,u8 n,u8 mach_type);
extern uint32_t  generate_password(u32 *passwd2,u32 *passwd1);
extern void timestamp_timeCalibration(char *tm,uint8_t tmType);  
extern void timestamp_strBJtime(unsigned long timestamp, char *ftime);
#endif
