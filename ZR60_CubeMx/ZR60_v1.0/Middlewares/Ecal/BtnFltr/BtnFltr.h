/******************************************************
�ļ�����	BtnFltr

������		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		BTNFLTR_H
#define		BTNFLTR_H
/*******************************************************
description�� include the header file
*******************************************************/
#include	"BtnFltr_Cfg.h"

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
extern void InitBtnFltr_Parameter(void);
extern void TskBtnFltr_MainFunction(void);
//extern uint8 GetBtnFltr_u_BtnShrtValid(uint8 LeBtnFltr_u_Index);
extern uint8 GetBtnFltr_u_BtnShrtSt(uint8 LeBtnFltr_u_Index);
//extern uint8 GetBtnFltr_u_BtnLngValid(uint8 LeBtnFltr_u_Index);
extern uint8 GetBtnFltr_u_BtnLngSt(uint8 LeBtnFltr_u_Index);
extern uint8 GetBtnFltr_u_BtnStuckSt(uint8 LeBtnFltr_u_Index);
extern void ClrBtnFltr_BtnLngValid(uint8 LeBtnFltr_u_Index);
extern void ClrBtnFltr_BtnShrtValid(uint8 LeBtnFltr_u_Index);

#endif
