/******************************************************
文件名：	BListMng.h

描述�	

Data			  Vasion			author
2018/4/12		  V1.0			    liujian
*******************************************************/
#ifndef		BLISTMNG_H
#define		BLISTMNG_H
/*******************************************************
description�include the header file
*******************************************************/
#include "BListMng_cfg.h"
#include "MyType.h"
#include "stm32f4xx.h"

/*******************************************************
description�macro definitions
*******************************************************/
/**********宏开关定�********/

/**********宏常量定�********/



/***********宏函�**********/

/*******************************************************
description�struct definitions
*******************************************************/


/*******************************************************
description�typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�variable External declaration
*******************************************************/


/*******************************************************
description�function External declaration
*******************************************************/
extern void BListMng_Init(void);
extern void TskBListMng_MainFunction(void);
extern uint8 BListMng_u_ContrastBList(uint32 Le_dw_data);
extern void ClrBListMng_ListData(void);
#endif
