/******************************************************
�ļ�����	PSFltr_Cfg

������		

Data			  Vasion			author
2018/6/28		   V1.0			    liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"PSFltr_Cfg.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/
const uint8 CaPSFltr_u_Active[PSFLTR_NUM] = /*������Ч���ò��������ɼ����ݸ�����һ������Ϊ���迪��*/
{
	1U,/*����״̬--����*/
	1U
};




/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/


/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������  GetPSFltrCfg_u_RTSt

*��  �Σ�

*����ֵ������ʵʱ״̬

*��  ����

*��  ע��
******************************************************/
uint8 GetPSFltrCfg_u_RTSt(uint8 LePSFltr_u_Index)
{
	uint8 Ret = 0U;
	switch(LePSFltr_u_Index)
	{
		case DoorLockSt:
		{
			Ret = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
		}
		break;
		case Key_Alarm:
		{
			Ret = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
		}
		break;
		default:
		break;
	}
	return Ret;
}


