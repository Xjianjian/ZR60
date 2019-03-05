/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
  *          常用的有 1us 10us 1ms 中断。     
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;
static  uint32_t  SysTick_T = 0U;
static void SysTick_Timer(void); 
/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemCoreClock / 100000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

/**
  * @brief   us延时程序,10us为一个单位
  * @param  
  *		@arg nTime: Delay_10us( 1 ) 则实现的延时为 1 * 10us = 10us
  * @retval  无
  */
void delay_10us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	while(TimingDelay != 0);
}


/**
  * @brief   us延时程序,1ms为一个单位
  * @param  
  *		@arg nTime: Delay_10us( 1 ) 则实现的延时为 1 * 1ms = 1ms
  * @retval  无
  */
void delay_ms(__IO u32 mTime)
{ 
	  u32 i;
		for(i = 0;i < mTime;i++)
		{
				delay_10us(100);
		}
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	SysTick_Timer();
}

/**
  * @brief  
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 TimingDelay_Decrement()调用
  */
static void SysTick_Timer(void)
{
	static uint8_t Cnt = 0U;
	Cnt++;
	if(Cnt >=100)
	{
		Cnt = 0U;
		SysTick_T++;
		if(SysTick_T == 0xFFFFFFFF)
		{
			SysTick_T = 0U;
		}
		
		//Vemain_w_5msTskTimer++;
	}
}
/**
  * @brief  获取系统时间,单位ms
  * @param  无
  * @retval 无
  * @attention  
  */
uint32_t GetSysTick_Timer(void)
{
	return SysTick_T;
}

/**
  * @brief  获取时间增量（T2 - T1）
  * @param  无
  * @retval 无
  * @attention  
  */
uint32_t GetSysTick_delta_T(uint32_t T1,uint32_t T2)
{
	uint32_t delta_T;
	if(T2 >= T1)
	{
		delta_T = T2 - T1;
	}
	else
	{
		delta_T = T2 + (0xFFFFFFFF - T1);
	}
	return delta_T;
}


/*********************************************END OF FILE**********************/
