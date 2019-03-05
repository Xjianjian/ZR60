/******************************************************
文件名：	LocalRTC_Cfg

描述�	

Data			  Vasion			author
2018/4/19		  V1.0				liujian
*******************************************************/
#ifndef		_LOCAL_RTC_CFG_H
#define		_LOCAL_RTC_CFG_H
/*******************************************************
description�include the header file
*******************************************************/
#include	"Include.h"


/*******************************************************
description�macro definitions
*******************************************************/
/**********宏开关定�********/
#define LOCAL_RTC_ON			//使用片上RTC
#define TIME_STAMP_UNT			0
#define TIME_STAMP_BJT			1
/**********宏常量定�********/
#define LOCALRTC_DEBUG
#if (defined(LOCALRTC_DEBUG) && defined(USART_DEBUGE_PRINTF))
#define	LOCALRTC_PRINTF_S(x)   	 printf("%s\n",x)
#define LOCALRTC_PRINTF_D(x,d)   	 printf(x,d)
#define LOCALRTC_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)   		printf(x,d1,d2,d3,d4,d5,d6)
#else
#define	LOCALRTC_PRINTF_S(x)
#define LOCALRTC_PRINTF_D(x,d)
#define LOCALRTC_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)
#endif
/***********宏函�**********/

/*******************************************************
description�struct definitions
*******************************************************/

/*******************************************************
description�typedef definitions
*******************************************************/
/*****struct definitions*****/
struct rtc_time{
	uint8 tm_sec;//��
	uint8 tm_min;//��
	uint8 tm_hour;//ʱ
	uint8 tm_mday;//��
	uint8 tm_mon;//��
	uint16 tm_year;//��
	uint8 tm_wday;//����
	uint8 tm_yday;//ƽ��/����
	uint16 tm_isdst;
};

typedef union
{
	unsigned char date[14U];
	struct 
	{	
		unsigned char year[4U];
		unsigned char mon[2U];
		unsigned char mday[2U];
		unsigned char hour[2U];
		unsigned char min[2U];
		unsigned char sec[2U];
	}DT;
}LocalRTC_DateTime;
/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�variable External declaration
*******************************************************/

/*******************************************************
description�function External declaration
*******************************************************/
extern void GetLocalRTCCfg_datetime(struct rtc_time* time);
extern uint8 SetLocalRTCCfg_datetime(struct rtc_time time);
#endif
