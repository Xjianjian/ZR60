/******************************************************
文件名：	LocalRTC

描述：		
Data			Vasion			author
2018/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"LocalRTC/LocalRTC.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/

/******************************************************
description： function code
******************************************************/

/******************************************************
*函数名：InitBListCache_Parameter

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void InitLocalRTC_Parameter(void)
{

}

/******************************************************
*函数名：GetLocalRTC_datetime

*形  参：

*返回值：

*描  述：读取时钟芯片时间:日期、时间戳

*备  注：
******************************************************/
void GetLocalRTC_datetime(struct rtc_time* time,uint32* timestamp)
{	
	struct tm stm; 
	GetLocalRTCCfg_datetime(time);
    stm.tm_year = time->tm_year - 1900;  
    stm.tm_mon = time->tm_mon - 1;  
    stm.tm_mday = time->tm_mday;  
    stm.tm_hour = time->tm_hour - 8;  
    stm.tm_min = time->tm_min;  
    stm.tm_sec = time->tm_sec;
	
    *timestamp = mktime(&stm);  
}

/******************************************************
*函数名：SetLocalRTC_datetime

*形  参：tm 当前日期时间(字符串形式)
		tmType  时间类型：0--北京时间；1--世界时间

*返回值：

*描  述：设置时钟芯片时间:日期时间

*备  注：
******************************************************/
uint8 SetLocalRTC_datetime(char *str_time,uint8 tmType)
{
	struct rtc_time stm;
	uint8 Le_u_i;
	LocalRTC_DateTime DateTime;
	
	for(Le_u_i = 0;Le_u_i < 14U;Le_u_i++)
	{
		DateTime.date[Le_u_i] = (uint8)(str_time[Le_u_i] - 0x30);
	}
	
	stm.tm_year = DateTime.DT.year[0U]*1000 + DateTime.DT.year[1U]*100 + \
								DateTime.DT.year[2U]*10 + DateTime.DT.year[3U];  
	stm.tm_mon  = (DateTime.DT.mon[0U]*10  + DateTime.DT.mon[1U]);
	stm.tm_mday = DateTime.DT.mday[0U]*10 + DateTime.DT.mday[1U];
	if(TIME_STAMP_UNT == tmType)
	{
		stm.tm_hour = DateTime.DT.hour[0U]*10 + DateTime.DT.hour[1U] +8;
	}
	else
	{
		stm.tm_hour = DateTime.DT.hour[0U]*10 + DateTime.DT.hour[1U];
	}
	stm.tm_min  = DateTime.DT.min[0U]*10  + DateTime.DT.min[1U];
	stm.tm_sec  = DateTime.DT.sec[0U]*10  + DateTime.DT.sec[1U];
	LOCALRTC_PRINTF_DATE("设置的时间：%d.%d.%d %d:%d:%d\n",stm.tm_year,stm.tm_mon,  \
					  stm.tm_mday,stm.tm_hour,stm.tm_min,stm.tm_sec);

	return SetLocalRTCCfg_datetime(stm);
}

/******************************************************
*函数名：SetLocalRTC_datetime

*形  参：

*返回值：当前日期时间(字符串形式的北京时间)

*描  述：时间戳转为字符串时间值"20190304102159"

*备  注：
******************************************************/
void LocalRTC_timestamp2strBJtime(unsigned long timestamp, char *ftime)
{
	struct tm *tm_t;
	timestamp = timestamp + 28800;//补充8小时秒数
	tm_t = localtime(&timestamp);
	strftime(ftime,15,"%Y%m%d%H%M%S",tm_t);
}



