/******************************************************
�ļ�����	BleUnlock

������		

Data			  Vasion			author
2018/04/7		  V1.0			    liujian
*******************************************************/
#ifndef		_BLEUNLOCK_H
#define		_BLEUNLOCK_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "BluetoothUnlock/BleUnlock_Cfg.h"

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
extern void  InitBleUnlock_parameter(void);
extern void  TskBleUnlock_MainFunction(void);
extern void  BleUnlock_RxMsgCallback(char* Le_u_rxMsg,uint8 Le_u_lng);

#endif 
