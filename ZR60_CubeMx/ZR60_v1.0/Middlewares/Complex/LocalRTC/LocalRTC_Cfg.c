/******************************************************
文件名：	LocalRTC_Cfg

描述：		
Data			Vasion			author
2018/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"LocalRTC/LocalRTC_Cfg.h"

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
*函数名：GetLocalRTCCfg_datetime

*形  参：

*返回值：

*描  述：读取时钟芯片时间:日期时间

*备  注：
******************************************************/
void GetLocalRTCCfg_datetime(struct rtc_time* time)
{
#ifdef LOCAL_RTC_ON
	RTC_TimeTypeDef stimestructure;
	RTC_DateTypeDef sdatestructure;
	/* Get the RTC current Time ,must get time first*/
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	time->tm_sec = stimestructure.Seconds;
	time->tm_min = stimestructure.Minutes;
	time->tm_hour = stimestructure.Hours;
	time->tm_mday = sdatestructure.Date;
	time->tm_mon = sdatestructure.Month;
	time->tm_year = 2000 + sdatestructure.Year;
#endif
}


/******************************************************
*函数名：SetLocalRTCCfg_datetime

*形  参：

*返回值：

*描  述：设置时钟芯片时间:日期时间

*备  注：
******************************************************/
uint8 SetLocalRTCCfg_datetime(struct rtc_time time)
{
	uint8 ret = 0;
#ifdef LOCAL_RTC_ON
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
    /** Set the Time and Date **/
	//if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2)
	{
		sTime.TimeFormat = RTC_HOURFORMAT12_AM;
		sTime.Hours = time.tm_hour;
		sTime.Minutes = time.tm_min;
		sTime.Seconds = time.tm_sec;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK)
		{
			sDate.WeekDay = 0;
			sDate.Month = time.tm_mon;
			sDate.Date = time.tm_mday;
			sDate.Year = (uint8_t)(time.tm_year -2000);

			if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) == HAL_OK)
			{
				ret = 1U;
			}
		}
		//HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);//写RTC备份寄存器，寄存器值为0x32F2时表示RTC被设置过
	}
#endif
	return ret;
}

