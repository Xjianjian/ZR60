/******************************************************
文件名：	PwUnlock

描述：		

Data			  Vasion			author
2018/04/7		  V1.0			    liujian
*******************************************************/
#ifndef		_PW_UNLOCK_H
#define		_PW_UNLOCK_H
/*******************************************************
description： include the header file
*******************************************************/
#include "PasswordUnlock/PwUnlock_Cfg.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/

/**********宏常量定义*********/
#define  PW_UNLOCK_SCHEDULING_CYCLE		  20U//调度周期


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
extern void  InitPwUnlock_parameter(void);
extern void  TskPwUnlock_MainFunction(void);

#endif 
