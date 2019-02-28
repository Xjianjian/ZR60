/******************************************************
�ļ�����	BleUnlock_cfg.h

������

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		BLEUNLOCK_CFG_H
#define		BLEUNLOCK_CFG_H
/*******************************************************
description?nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description?acro definitions
*******************************************************/
/**********�꿪�ض��e*******/
#define BLEUNLOCK_DEBUG

#ifdef 	BLEUNLOCK_DEBUG
#define	BLEUNLOCK_PRINTF_S(x)   	 printf("%s\n",x)
#define BLEUNLOCK_PRINTF_D(x,d)   	 printf(x,d)
#define BLEUNLOCK_PRINTF_MAC(x,d1,d2,d3,d4,d5,d6)    	printf(x,d1,d2,d3,d4,d5,d6)
#define BLEUNLOCK_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)   		printf(x,d1,d2,d3,d4,d5,d6)
#else
#define	BLEUNLOCK_PRINTF_S(x)
#define BLEUNLOCK_PRINTF_D(x,d)
#define BLEUNLOCK_PRINTF_MAC(x,d1,d2,d3,d4,d5,d6)
#define BLEUNLOCK_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)
#endif

/**********�곣�����e*******/
/********************************************************/
#define  BLEUNLOCK_SCHEDULING_CYCLE		  10U//��������


/********************************************************/
#define BLEUNLOCK_MAC_INFO    EepromCfg_MacInfo

#define BLEUNLOCK_DOOROPEN	  1//AudioIO_DoorOpen
#define BLEUNLOCK_ERRKEY	  1//AudioIO_ErrKey

/***********�꺯��*********/

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/******enum definitions******/
typedef enum 
{
	BleUnlock_MAC_ADDR = 0U,/*��ȡMAC��ַ*/
	BleUnlock_DISA = 1U/*ɨ���������*/
}BleUnlock_TxMsgStyle;

/*****struct definitions*****/
typedef struct{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
}BleUnlock_rtcTime ;


/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/

/*******************************************************/
extern uint8 GetBleUnlockCfg_u_CheckSum(uint8 Le_u_Obj);
extern uint8  BleUnlockCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern void  BleUnlockCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern uint8 GetBleUnlockCfg_u_DtVild(uint8 Obj);
extern void  SetBleUnlockCfg_EEVild(uint8 Obj);
extern void  BleUnlockCfg_UnlockRecord(char* Le_u_UnlockInfo);
extern void  BleUnlockCfg_timeCali(char* time);//У׼ʱ��оƬʱ��
extern uint8 BleUnlockCfg_u_NetUnlockSt(void);
extern void  BleUnlockCfg_datetime(BleUnlock_rtcTime* time,uint32* timestamp);
extern void  BleUnlockCfg_BleTxMsg(uint8 ReqType);
extern uint8 BleUnlockCfg_u_machineType(void);
extern uint8 BleUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password);
extern void  BleUnlockCfg_OpenDoor(void);
extern uint8 GetBleUnlockCfg_u_DoorSt(void);
extern void BleUnlockCfg_Play(uint8 playfile);
#endif

