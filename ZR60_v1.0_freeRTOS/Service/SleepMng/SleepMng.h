/******************************************************
文件名：	SleepMng.h

描述�

Data			  Vasion			author
2018/8/6		  V1.0			    liujian
*******************************************************/
#ifndef		SLEEP_MNG_H
#define		SLEEP_MNG_H
/*******************************************************
description：include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description：macro definitions
*******************************************************/
#define  SLEEPMNG_SCHEDULING_CYCLE     (uint16)20U//��������  ��λms



/*******************************************************
description：struct definitions
*******************************************************/

/*******************************************************
description：typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description：variable External declaration
*******************************************************/


/*******************************************************
description：function External declaration
*******************************************************/
extern void InitSleepMng_parameter(void);
extern void TskSleepMng_MainFunction(void);

#endif
