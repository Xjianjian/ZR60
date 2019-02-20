/******************************************************
文件名：	STD_ADFliter

描述：		

Data			  Vasion			author
2018/6/27		V1.0			  liujian
*******************************************************/
#ifndef		STD_ADFLITER_H
#define		STD_ADFLITER_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"STD_ADFliterCfg.h"

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
extern void InitADFliter_parameter(void);
extern void MngADFliter_mainFunction(void);
extern int16 GetADFliter_ADFliterResult(uint8 LeADFliter_u_IndexCH);

#endif
