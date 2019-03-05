/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;
static  uint32_t  SysTick_T = 0U;
static void SysTick_Timer(void); 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 100000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

/**
  * @brief   us��ʱ����,10usΪһ����λ
  * @param  
  *		@arg nTime: Delay_10us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
  * @retval  ��
  */
void delay_10us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	while(TimingDelay != 0);
}


/**
  * @brief   us��ʱ����,1msΪһ����λ
  * @param  
  *		@arg nTime: Delay_10us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 1ms = 1ms
  * @retval  ��
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
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
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
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� TimingDelay_Decrement()����
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
  * @brief  ��ȡϵͳʱ��,��λms
  * @param  ��
  * @retval ��
  * @attention  
  */
uint32_t GetSysTick_Timer(void)
{
	return SysTick_T;
}

/**
  * @brief  ��ȡʱ��������T2 - T1��
  * @param  ��
  * @retval ��
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
