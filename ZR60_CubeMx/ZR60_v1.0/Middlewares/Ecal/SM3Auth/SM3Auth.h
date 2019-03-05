/******************************************************
文件名：	SM3Auth.h

描述:

Data			  Vasion			author
2018/1/4		  V1.0			    liujian
*******************************************************/
#ifndef		_SM3_AUTH_H
#define		_SM3_AUTH_H
/*******************************************************
description：include the header file
*******************************************************/
#include "SM3Auth/SM3.h"

/*******************************************************
description：macro definitions
*******************************************************/
/**********宏开关定�*******/
#define SM3AUTH_DEBUG
#if (defined(SM3AUTH_DEBUG) && defined(USART_DEBUGE_PRINTF))
#define	SM3AUTH_PRINTF_S(x)   	 printf("%s\n",x)
#define SM3AUTH_PRINTF_D(x,d)   	 printf(x,d)
#else
#define	SM3AUTH_PRINTF_S(x)   	
#define SM3AUTH_PRINTF_D(x,d)   	
#endif

/**********宏常量定�*******/
#define PASSWORD_TEMPORARY	0
#define PASSWORD_BLE		1

/***********宏函�*********/

/*******************************************************
description：struct definitions
*******************************************************/

/*******************************************************
description：typedef definitions
*******************************************************/
/*****struct definitions*****/
struct sm3AuthInfo //用来验证的数据信�
{
	unsigned char community_id[8];//小区id
	unsigned char build_num[8];//楼栋�
	unsigned char cell_num[8];//单元�
	unsigned char null;//空字�\0'
};
/******enum definitions******/

/******union definitions*****/

/*******************************************************
description：variable External declaration
*******************************************************/


/*******************************************************
description：function External declaration
*******************************************************/
extern void InitSM3Auth_Paramter(void* SM3Info,uint8 machineType);
extern uint8_t SM3Auth_PasswordAuth(uint32_t timestamp,uint8 mach_type,uint32 Password,uint8 pwType);


#endif
