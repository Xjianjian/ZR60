/******************************************************
文件名：	DoorLockCtrl

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "DoorLockCtrl/DoorLockCtrl.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static uint8  SeDoorLockCtrl_u_OpenLockReq;//开锁请求
static uint8  SeDoorLockCtrl_u_LockSt;//门锁状态
static uint16 SeDoorLockCtrl_u_LockTimer;//闭锁计时器

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitDoorLockCtrl_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitDoorLockCtrl_parameter(void)
{
	SeDoorLockCtrl_u_OpenLockReq = 0;//开锁请求
	SeDoorLockCtrl_u_LockSt = 0;//门锁状态
	SeDoorLockCtrl_u_LockTimer = 0;
}

/******************************************************
*函数名：TskDoorLockCtrl_MainFunction

*形  参：

*返回值：

*描  述：串口接收

*备  注：串口接收中断调用
******************************************************/
void TskDoorLockCtrl_MainFunction(void)
{ 
	if(1 == SeDoorLockCtrl_u_OpenLockReq)
	{
		OPEN_DOOR();
		SeDoorLockCtrl_u_OpenLockReq = 0;
		SeDoorLockCtrl_u_LockSt = 1;//门锁状态->打开
		SeDoorLockCtrl_u_LockTimer = 0;
	}
	
	if(1 == SeDoorLockCtrl_u_LockSt)
	{
		if(SeDoorLockCtrl_u_LockTimer >= (3000/DOORLOCKCTRL_SCHEDULING_CYCLE))
		{
			CLOSE_DOOR();
			SeDoorLockCtrl_u_LockSt = 0;//门锁状态->关闭
		}
		else
		{
			SeDoorLockCtrl_u_LockTimer++;
		}
	}
}

/******************************************************
*函数名：DoorLockCtrl_OpenDoor

*形  参：

*返回值：

*描  述：请求开门

*备  注：
******************************************************/
void DoorLockCtrl_OpenDoor(void)
{ 
	SeDoorLockCtrl_u_OpenLockReq = 1;//开锁请求
}