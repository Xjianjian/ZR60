/******************************************************
文件名：	LocalRTC

描述：		

Data			  Vasion			author
2018/4/19		V1.0				liujian
*******************************************************/
#ifndef		BLISTCACHE_H
#define		BLISTCACHE_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"LocalRTC/LocalRTC_Cfg.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/

/***********宏函数***********/

/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/

/*******************************************************
description： function External declaration
*******************************************************/
extern void GetLocalRTC_datetime(struct rtc_time* time,uint32* timestamp);
extern void LocalRTC_timestamp2strBJtime(unsigned long timestamp, char *ftime);
extern uint8 SetLocalRTC_datetime(char *str_time,uint8 tmType);
#endif
