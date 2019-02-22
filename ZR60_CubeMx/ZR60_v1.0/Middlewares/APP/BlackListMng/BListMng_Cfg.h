/******************************************************
�ļ�����	BListMng_cfg.h

������

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
/**********�꿪�ض��e*******/
//#define TEST
//#define BLISTMNG_SAVEPOWEROFF

/**********�곣�����e*******/
/********************************************************/
//���֧�ֵĺ�������������2000�����ŷֵ�10�����ݿ�洢


#define BLISTMNG_CARD_NO_LNG     4//���ų���
#define BLISTMNG_CARD_NUM        200//��������RAM��������С


#define BLISTMNG_BLIST_NUM      	 10U//���������ݿ�����


/********************************************************/


/***********�꺯��*********/

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{
	uint8   CardNum[BLISTMNG_CARD_NO_LNG];/*����*/
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
