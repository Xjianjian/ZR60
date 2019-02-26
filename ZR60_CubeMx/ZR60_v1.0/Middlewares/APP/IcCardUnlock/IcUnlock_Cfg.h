/******************************************************
�ļ�����	BListMng_cfg.h

������

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
/**********�꿪�ض��e*******/


/**********�곣�����e*******/

#define IC_UNLOCK_SETCARD    					1//AudioIO_ConfDevice
#define IC_UNLOCK_SETCARDSUCCESS     	1//AudioIO_ConfSuccess
#define IC_UNLOCK_SETCARDCANCEL     	1//AudioIO_ConfCancel
#define IC_UNLOCK_CARD_INVALID     		1//AudioIO_CardInvalid
#define IC_UNLOCK_DOOROPEN     		1//AudioIO_DoorOpen

#define IC_UNLOCK_EE_CARD_INFO     		EepromCfg_CardInfo

#define IC_UNLOCK_LOGCACHE_CARD     		1//UNLOCKLOGCACHE_CARD
/********************************************************/


/***********�꺯��*********/
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
		unsigned char Block[59];/*����������*/
		struct
		{
			unsigned char Seqnr;/* ����֡����*/
			unsigned char St;/* ״̬*/
			unsigned char DtLng;/* ������Ч���ݶγ��ȱ�ʾ*/
			unsigned char ValidDt[56];/* ���յ���Ч���ݶΣ�����󳤶ȶ���洢�ռ�*/
		}DtSrt;
	}Dt;
}IcUnlock_RxMsgStruct; /*���Ľ��սṹ��*/ 


typedef struct 
{		
	char CardID[4U];//����
	char lng;//���ų�
}CardNumInfo; /*������Ϣ�ṹ��*/ 


typedef struct //ĸ����Ϣ
{
	unsigned char community_id[16];//С��id
	unsigned char build_numOne[16];//¥������б�1(0x15)
	unsigned char build_numTwo[16];//¥������б�2(0x16)
	unsigned char build_id[16];//¥��id
	unsigned char door_id[16];//��id
	unsigned short suffix;//�±�
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
