/******************************************************
�ļ�����	PSFltr

������		

Data			  Vasion			author
2018/6/28		   V1.0			    liujian
*******************************************************/
#ifndef		PSFLTR_H
#define		PSFLTR_H
/*******************************************************
description�� include the header file
*******************************************************/
#include	"PSFltr_Cfg.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/


/**********�곣������*********/


/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern void InitPSFltr_Parameter(void);
extern void TskPSFltr_MainFunction(void);
extern uint8 GetPSFltr_u_PeriSt(uint8 LePSFltr_u_Index);
#endif
