/******************************************************
文件名：	SleepMng.c

描述：		
Data			Vasion			author
2018/8/6		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "SleepMng.h"


/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/
static uint16 SeSleepMng_w_CntdownTimer;

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitSleepMng_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitSleepMng_parameter(void)
{
	SeSleepMng_w_CntdownTimer = (2000U / (SLEEPMNG_SCHEDULING_CYCLE/5U));//初始化倒计时时间，大概10s
}

/******************************************************
*函数名：TskSleepMng_MainFunction

*形  参：void

*返回值：void

*描  述：主任务

*备  注：
******************************************************/
void TskSleepMng_MainFunction(void)
{
	if(0)//睡眠条件满足
	{
		SeSleepMng_w_CntdownTimer--;//睡眠倒计时
		if(0U == SeSleepMng_w_CntdownTimer)//睡眠倒计时时间到
		{//进入睡眠状态
			__dsb( portSY_FULL_READ_WRITE );
			__wfi();
			__isb( portSY_FULL_READ_WRITE );
		}
	}
	else
	{
		SeSleepMng_w_CntdownTimer = (2000U / (SLEEPMNG_SCHEDULING_CYCLE/5U));//重置睡眠倒计时器
	}
}

