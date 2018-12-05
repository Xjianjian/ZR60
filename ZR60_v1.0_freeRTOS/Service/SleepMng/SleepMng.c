/******************************************************
�ļ�����	SleepMng.c

������		
Data			Vasion			author
2018/8/6		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "SleepMng.h"


/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/
static uint16 SeSleepMng_w_CntdownTimer;

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitSleepMng_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitSleepMng_parameter(void)
{
	SeSleepMng_w_CntdownTimer = (2000U / (SLEEPMNG_SCHEDULING_CYCLE/5U));//��ʼ������ʱʱ�䣬���10s
}

/******************************************************
*��������TskSleepMng_MainFunction

*��  �Σ�void

*����ֵ��void

*��  ����������

*��  ע��
******************************************************/
void TskSleepMng_MainFunction(void)
{
	if(0)//˯����������
	{
		SeSleepMng_w_CntdownTimer--;//˯�ߵ���ʱ
		if(0U == SeSleepMng_w_CntdownTimer)//˯�ߵ���ʱʱ�䵽
		{//����˯��״̬
			__dsb( portSY_FULL_READ_WRITE );
			__wfi();
			__isb( portSY_FULL_READ_WRITE );
		}
	}
	else
	{
		SeSleepMng_w_CntdownTimer = (2000U / (SLEEPMNG_SCHEDULING_CYCLE/5U));//����˯�ߵ���ʱ��
	}
}

