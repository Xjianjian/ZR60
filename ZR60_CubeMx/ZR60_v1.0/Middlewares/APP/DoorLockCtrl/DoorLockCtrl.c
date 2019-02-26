/******************************************************
�ļ�����	DoorLockCtrl

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "DoorLockCtrl/DoorLockCtrl.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint8  SeDoorLockCtrl_u_OpenLockReq;//��������
static uint8  SeDoorLockCtrl_u_LockSt;//����״̬
static uint16 SeDoorLockCtrl_u_LockTimer;//������ʱ��

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitDoorLockCtrl_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitDoorLockCtrl_parameter(void)
{
	SeDoorLockCtrl_u_OpenLockReq = 0;//��������
	SeDoorLockCtrl_u_LockSt = 0;//����״̬
	SeDoorLockCtrl_u_LockTimer = 0;
}

/******************************************************
*��������TskDoorLockCtrl_MainFunction

*��  �Σ�

*����ֵ��

*��  �������ڽ���

*��  ע�����ڽ����жϵ���
******************************************************/
void TskDoorLockCtrl_MainFunction(void)
{ 
	if(1 == SeDoorLockCtrl_u_OpenLockReq)
	{
		OPEN_DOOR();
		SeDoorLockCtrl_u_OpenLockReq = 0;
		SeDoorLockCtrl_u_LockSt = 1;//����״̬->��
		SeDoorLockCtrl_u_LockTimer = 0;
	}
	
	if(1 == SeDoorLockCtrl_u_LockSt)
	{
		if(SeDoorLockCtrl_u_LockTimer >= (3000/DOORLOCKCTRL_SCHEDULING_CYCLE))
		{
			CLOSE_DOOR();
			SeDoorLockCtrl_u_LockSt = 0;//����״̬->�ر�
		}
		else
		{
			SeDoorLockCtrl_u_LockTimer++;
		}
	}
}

/******************************************************
*��������DoorLockCtrl_OpenDoor

*��  �Σ�

*����ֵ��

*��  ����������

*��  ע��
******************************************************/
void DoorLockCtrl_OpenDoor(void)
{ 
	SeDoorLockCtrl_u_OpenLockReq = 1;//��������
}