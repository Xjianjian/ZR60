/******************************************************
文件名：	MemIf.h

描述�	

Data			  Vasion			author
2018/1/13		  V1.0			    liujian
*******************************************************/
#ifndef		MEMIF_H
#define		MEMIF_H
/*******************************************************
description�include the header file
*******************************************************/
#include "MemIf_cfg.h"


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
extern void  MemIf_Init(void);
extern void  TskMemIf_MainFunction(void);
extern uint8 MemIf_ReadEE(uint8 Le_u_Object,uint8* Le_u_Data,uint16 Le_w_Lng);
extern uint8 MemIf_WriteEE(uint8 Le_u_Object,void* Le_u_Data,uint16 Le_w_Lng);
extern uint8 GetMemIf_u_EEVild(uint8 Object);
extern uint8 GetMemIf_u_Idle(void);
extern void SetMemIf_EEVild(uint8 Object);
extern uint8 GetMemIf_u_DtVild(uint8 Obj);
extern uint16 GetMemIf_w_DtLng(uint8 Obj);
extern uint8 GetMemIf_u_CheckSum(uint8 Obj);
extern void MemIf_Timer(void);
#ifdef EE_DEBUG
extern void MemIf_TestEE(void);
#endif
#endif
