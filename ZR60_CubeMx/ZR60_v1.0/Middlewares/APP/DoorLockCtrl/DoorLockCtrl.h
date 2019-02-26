/******************************************************
�ļ�����	DoorLockCtrl

������		

Data			  Vasion			author
2018/04/7		  V1.0			    liujian
*******************************************************/
#ifndef		_DOOR_LOCK_CTRL_H
#define		_DOOR_LOCK_CTRL_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/

/**********�곣������*********/
#define  DOORLOCKCTRL_SCHEDULING_CYCLE		  10U//��������


/***********�꺯��***********/
#define OPEN_DOOR()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define CLOSE_DOOR()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)

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
extern void  InitDoorLockCtrl_parameter(void);
extern void  TskDoorLockCtrl_MainFunction(void);
extern void  DoorLockCtrl_OpenDoor(void);
#endif 
