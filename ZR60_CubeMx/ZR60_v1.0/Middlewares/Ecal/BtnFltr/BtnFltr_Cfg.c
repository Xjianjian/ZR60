/******************************************************
�ļ�����	BtnFltr_Cfg

������		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"BtnFltr/BtnFltr_Cfg.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/
const uint8 CaBtnFltr_u_BtnActive[BTNFLTR_BTN_NUM] = /*������Ч���ò��������ɼ����ݸ�����һ������Ϊ������Ч*/
{
	1U,/*ĸ�����ð���*/
	//1U,/*������֤����*/
	1U/*���ڿ�������*/
	//0U/*������������*/
};

const uint16 CaBtnFltr_w_BtnStuckThre[BTNFLTR_BTN_NUM] = /*���������о�ʱ����������*/
{
	4000U,/*ĸ�����ð���*/
	//4000U,/*������֤����*/
	4000U/*���ڿ�������*/
	//4000U/*������������*/
};
/*Static function declaration*/



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
*��������  GetBtnFltrCfg_u_BtnRTSt

*��  �Σ�

*����ֵ������ʵʱ״̬

*��  ����

*��  ע��
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


