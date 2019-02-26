/******************************************************
文件名：	BListMng_cfg.h

描述）

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		IC_UNLOCK_CFG_H
#define		IC_UNLOCK_CFG_H
/*******************************************************
description?nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description?acro definitions
*******************************************************/
/**********宏开关定乪*******/


/**********宏常量定乪*******/

#define IC_UNLOCK_SETCARD    					1//AudioIO_ConfDevice
#define IC_UNLOCK_SETCARDSUCCESS     	1//AudioIO_ConfSuccess
#define IC_UNLOCK_SETCARDCANCEL     	1//AudioIO_ConfCancel
#define IC_UNLOCK_CARD_INVALID     		1//AudioIO_CardInvalid
#define IC_UNLOCK_DOOROPEN     		1//AudioIO_DoorOpen

#define IC_UNLOCK_EE_CARD_INFO     		EepromCfg_CardInfo

#define IC_UNLOCK_LOGCACHE_CARD     		1//UNLOCKLOGCACHE_CARD
/********************************************************/


/***********宏函敪*********/
#define IcUnlock_VrfyCardNumber(x)  0//BListMng_u_ContrastBList(x)

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{		
	union
	{
		unsigned char Block[59];/*接收数据数*/
		struct
		{
			unsigned char Seqnr;/* 接收帧包号*/
			unsigned char St;/* 状态*/
			unsigned char DtLng;/* 接收有效数据段长度表示*/
			unsigned char ValidDt[56];/* 接收的有效数据段：按最大长度定义存储空间*/
		}DtSrt;
	}Dt;
}IcUnlock_RxMsgStruct; /*报文接收结构体*/ 


typedef struct 
{		
	char CardID[4U];//卡号
	char lng;//卡号长
}CardNumInfo; /*卡号信息结构体*/ 


typedef struct //母卡信息
{
	unsigned char community_id[16];//小区id
	unsigned char build_numOne[16];//楼栋编号列表1(0x15)
	unsigned char build_numTwo[16];//楼栋编号列表2(0x16)
	unsigned char build_id[16];//楼栋id
	unsigned char door_id[16];//门id
	unsigned short suffix;//下标
}IcUnlock_CardConf;
/******enum definitions******/



/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/



/*******************************************************
description?unction External declaration
*******************************************************/
extern uint8 GetIcUnlockCfg_u_cardSetKeySt(void);
extern void SetIcUnlockCfg_PlayFile(uint8 Le_u_PlayFile);
extern void IcUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);
extern uint8 GetIcUnlockCfg_u_RxMsgRenew(uint8* Le_u_RxMsg);
extern void SetIcUnlockCfg_TxMsg(uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng);
extern void SetIcUnlockCfg_openDoor(void);
extern void IcUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng);
extern uint8 IcUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern uint8 GetIcUnlockCfg_u_CheckSum(uint8 Le_u_Obj);
extern uint8 GetIcUnlockCfg_u_DtVild(uint8 Le_u_Obj);
extern void SetIcUnlockCfg_EEVild(uint8 Le_u_Obj);
extern void IcUnlockCfg_CardRenewCallback(IcUnlock_CardConf* CardConf);
#endif
