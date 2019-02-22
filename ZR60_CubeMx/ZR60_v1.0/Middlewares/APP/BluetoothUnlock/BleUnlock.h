/******************************************************
�ļ�����	BleUnlock

������		

Data			  Vasion			author
2019/02/20		  V1.0			    liujian
*******************************************************/
#ifndef		_BLE_UNLOCK_H
#define		_BLE_UNLOCK_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
/*���պ궨��*/
//#define	 UARTCMN_USART		USART3
#define  ZR60CTRL_SCHEDULING_CYCLE		  20U//��������



#define ZR60_CTRL_REMOTE_UNLOCK    Setcp_client_u_Open
#define ZR60_CTRL_WX_UNLOCK    		Setcp_client_u_wxOpen

#define ZR60_CTRL_AUTOSEARCHCARD   isSectedCard_flag
#define ZR60_CTRL_SETKEY   		   Sereadcard_SetKeyflag

#define ZR60_CTRL_SETCARD          setcard

/***********�꺯��***********/
#define SetBleUnlock_PlayFile()  //SetAudioIO_PlayFile(AudioIO_DoorOpen,2U)

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
extern struct sm3_info sm;
extern struct rtc_time Ve_h_tm;

/*******************************************************
description�� function External declaration
*******************************************************/
extern void  InitZR60Ctrl_parameter(void);
extern void  TskZR60Ctrl_MainFunction(void);
extern uint8_t	GetZR60Ctrl_u_Alarm(void);
extern uint8_t	GetZR60Ctrl_u_CardSetSt(void);
extern void SetZR60Ctrl_u_PasswordKey(void);
extern void ClrZR60Ctrl_BleSt(void);
extern void ClrZR60Ctrl_BleCnntTimeout(void);
extern char ZR60Ctrl_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len);
extern uint8_t	GetZR60Ctrl_u_MastCtrlIapSt(void);
extern void ClrZR60Ctrl_MastCtrlIapSt(void);
extern uint8_t	GetZR60Ctrl_u_CardReaderIapSt(void);
extern void ClrZR60Ctrl_CardReaderIapSt(void);
#endif 
