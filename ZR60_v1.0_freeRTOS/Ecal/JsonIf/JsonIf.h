/******************************************************
文件名：	Json

描述：		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		_JSON_H
#define		_JSON_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/
#define JSON_HEART_BEAT     0U
#define JSON_GET_BLIST      1U
#define JSON_REPORT_DOORST  2U
#define JSON_REPORT_UNLOCKLOG  3U
#define JSON_DEVICE_INIT  4U
#define JSON_DEVICE_ECHO_REMOTEUNLOCK  5U

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
extern void Json_HexToJson(void *Le_u_in,uint16* Len,unsigned char type, char *Le_u_out);
extern void Json_HexToStr(char *pszDest, unsigned char *pbSrc, int nLen);
extern uint32 Json_DecNumber(const char* str);
#endif
