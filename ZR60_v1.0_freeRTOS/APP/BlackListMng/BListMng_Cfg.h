/******************************************************
文件名：	BListMng_cfg.h

描述）

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		BLISTMNG_CFG_H
#define		BLISTMNG_CFG_H
/*******************************************************
description?nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description?acro definitions
*******************************************************/
/**********宏开关定乪*******/
//#define TEST
//#define BLISTMNG_SAVEPOWEROFF

/**********宏常量定乪*******/
/********************************************************/
//设计支持的黑名单卡号数量2000，卡号分到10个数据块存储


#define BLISTMNG_CARD_NO_LNG     4//卡号长度
#define BLISTMNG_CARD_NUM        200//卡号数据RAM缓存区大小


#define BLISTMNG_BLIST_NUM      	 10U//黑名单数据块数量


/********************************************************/


/***********宏函敪*********/

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{
	uint8   CardNum[BLISTMNG_CARD_NO_LNG];/*卡号*/
}BListMng_cardNumStruct;


typedef struct 
{
	uint8  validity;
	uint16  lng;
	BListMng_cardNumStruct  CardNumArray[BLISTMNG_CARD_NUM];
}BListMng_Struct;

/******enum definitions******/



/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/
extern BListMng_Struct  CardNumCache[BLISTMNG_BLIST_NUM];
extern BListMng_Struct  CardNumCacheBkUp[BLISTMNG_BLIST_NUM];
#ifdef BLISTMNG_SAVEPOWEROFF
extern const uint8 CeBListMng_u_BlockIndex[BLISTMNG_BLIST_NUM];
#endif
extern BListMng_cardNumStruct CARD_Array[BLISTMNG_CARD_NUM];
/*******************************************************
description?unction External declaration
*******************************************************/
extern void BListMngCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern void BListMngCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern uint8  GetBListMngCfg_EEIdle(void);
extern uint8  GetBListMngCfg_u_DtVild(uint8 Obj);
extern uint16 GetBListMngCfg_w_DtLng(uint8 Obj);
#endif
