/******************************************************
文件名：	PwUnlock_cfg.h

描述）

Data			  Vasion			author
2018/04/12		  V1.0			    liujian
*******************************************************/
#ifndef		PW_UNLOCK_CFG_H
#define		PW_UNLOCK_CFG_H
/*******************************************************
description?nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description?acro definitions
*******************************************************/
/**********宏开关定乪*******/
#define PWUNLOCK_DEBUG
#if (defined(PWUNLOCK_DEBUG) && defined(USART_DEBUGE_PRINTF))
#define	PWUNLOCK_PRINTF_S(x)   	 printf("%s\n",x)
#define PWUNLOCK_PRINTF_D(x,d)   	 printf(x,d)
#define PWUNLOCK_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)   		printf(x,d1,d2,d3,d4,d5,d6)
#else
#define	PWUNLOCK_PRINTF_S(x)
#define PWUNLOCK_PRINTF_D(x,d)
#define PWUNLOCK_PRINTF_DATE(x,d1,d2,d3,d4,d5,d6)
#endif

/**********宏常量定乪*******/
#define PW_UNLOCK_KEYNUM  			12
#define PW_UNLOCK_ERRKEY    					1//AudioIO_ErrKey
#define PW_UNLOCK_DOOROPEN     		1//AudioIO_DoorOpen
#define PW_UNLOCK_KEYVOICE			1//AudioIO_keyVoice


#define PW_UNLOCK_LOGCACHE_CARD     		1//UNLOCKLOGCACHE_TEMPPSWORD
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
	uint16 RegValue;//按键对应的寄存器值
	uint8  keyValue;//按键值
}PwUnlockCfg_Key;

/*****struct definitions*****/
typedef struct{
	uint8 tm_sec;
	uint8 tm_min;
	uint8 tm_hour;
	uint8 tm_mday;
	uint8 tm_mon;
	uint16 tm_year;
	uint8 tm_wday;
	uint8 tm_yday;
	uint16 tm_isdst;
}PwUnlockCfg_rtcTime ;
/******enum definitions******/



/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/
extern const PwUnlockCfg_Key PwUnlock_Key[PW_UNLOCK_KEYNUM];


/*******************************************************
description?unction External declaration
*******************************************************/
extern uint8 GetPwUnlockCfg_u_cardSetKeySt(void);
extern uint8 PwUnlockCfg_u_machineType(void);
extern void PwUnlockCfg_u_RecordUnlockLog(char* Le_u_UnlockInfo,uint8 Le_u_UnlockEvent);
extern uint8 PwUnlockCfg_ReadKeyValue(uint8* Le_u_buffer,uint8 bufsize);
extern void SetPwUnlockCfg_TxMsg(uint8* LeUartCmn_u_TxData,uint8 LeUartCmn_u_Lng);
extern void SetPwUnlockCfg_openDoor(void);
extern void PwUnlockCfg_WrEE(uint8 Le_u_Obj,void* Le_u_Data,uint16 Le_w_Lng);
extern uint8 PwUnlockCfg_ReadEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng);
extern uint8 GetPwUnlockCfg_u_CheckSum(uint8 Le_u_Obj);
extern uint8 GetPwUnlockCfg_u_DtVild(uint8 Le_u_Obj);
extern void SetPwUnlockCfg_EEVild(uint8 Le_u_Obj);
extern void PwUnlockCfg_datetime(PwUnlockCfg_rtcTime* time,uint32* timestamp);
extern uint8 PwUnlockCfg_PasswordAuth(uint32 timestamp,uint8 machine_type,uint32 Password);
extern void PwUnlockCfg_Play(uint8 playfile);
#endif
