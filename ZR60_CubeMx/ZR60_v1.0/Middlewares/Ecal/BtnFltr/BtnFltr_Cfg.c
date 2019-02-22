/******************************************************
文件名：	BtnFltr_Cfg

描述：		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"BtnFltr/BtnFltr_Cfg.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/
const uint8 CaBtnFltr_u_BtnActive[BTNFLTR_BTN_NUM] = /*按键有效配置参数，当采集数据跟配置一致则认为按键有效*/
{
	1U,/*母卡设置按键*/
	//1U,/*密码验证按键*/
	1U/*门内开锁按键*/
	//0U/*消防报警按键*/
};

const uint16 CaBtnFltr_w_BtnStuckThre[BTNFLTR_BTN_NUM] = /*按键卡键判决时间门限配置*/
{
	4000U,/*母卡设置按键*/
	//4000U,/*密码验证按键*/
	4000U/*门内开锁按键*/
	//4000U/*消防报警按键*/
};
/*Static function declaration*/



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
*函数名：  GetBtnFltrCfg_u_BtnRTSt

*形  参：

*返回值：按键实时状态

*描  述：

*备  注：
******************************************************/
uint8 GetBtnFltrCfg_u_BtnRTSt(uint8 LeBtnFltr_u_Index)
{
	uint8 Ret = 0U;
	switch(LeBtnFltr_u_Index)
	{
		case Key_card_set:
		{
			Ret = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		}
		break;
		//case Key_PasswodAuth:
		//{
		//	Ret = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2);
		//}
		//break;
		case Key_Unlock:
		{
			Ret = HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_15);
		}
		break;
		//case Key_Alarm:
		//{
		//	Ret = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
		//}
		//break;
		default:
		break;
	}
	return Ret;
}


