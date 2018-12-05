/******************************************************
文件名：	PSFltr_Cfg

描述：		

Data			  Vasion			author
2018/6/28		   V1.0			    liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"PSFltr_Cfg.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/
const uint8 CaPSFltr_u_Active[PSFLTR_NUM] = /*开关有效配置参数，当采集数据跟配置一致则认为外设开启*/
{
	1U,/*门锁状态--开启*/
	1U
};




/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：  GetPSFltrCfg_u_RTSt

*形  参：

*返回值：按键实时状态

*描  述：

*备  注：
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


