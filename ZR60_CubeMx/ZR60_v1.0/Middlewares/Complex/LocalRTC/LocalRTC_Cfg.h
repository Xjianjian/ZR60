/******************************************************
æ–‡ä»¶åï¼š	LocalRTC_Cfg

æè¿°ï¼	

Data			  Vasion			author
2018/4/19		  V1.0				liujian
*******************************************************/
#ifndef		_LOCAL_RTC_CFG_H
#define		_LOCAL_RTC_CFG_H
/*******************************************************
descriptionï¼include the header file
*******************************************************/
#include	"Include.h"


/*******************************************************
descriptionï¼macro definitions
*******************************************************/
/**********å®å¼€å…³å®šä¹********/
#define LOCAL_RTC_ON			//ä½¿ç”¨ç‰‡ä¸ŠRTC
#define TIME_STAMP_UNT			0
#define TIME_STAMP_BJT			1
/**********å®å¸¸é‡å®šä¹********/
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
/***********å®å‡½æ•**********/

/*******************************************************
descriptionï¼struct definitions
*******************************************************/

/*******************************************************
descriptionï¼typedef definitions
*******************************************************/
/*****struct definitions*****/
struct rtc_time{
	uint8 tm_sec;//Ãë
	uint8 tm_min;//·Ö
	uint8 tm_hour;//Ê±
	uint8 tm_mday;//ÈÕ
	uint8 tm_mon;//ÔÂ
	uint16 tm_year;//Äê
	uint8 tm_wday;//ĞÇÆÚ
	uint8 tm_yday;//Æ½Äê/ÈòÄê
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
descriptionï¼variable External declaration
*******************************************************/

/*******************************************************
descriptionï¼function External declaration
*******************************************************/
extern void GetLocalRTCCfg_datetime(struct rtc_time* time);
extern uint8 SetLocalRTCCfg_datetime(struct rtc_time time);
#endif
