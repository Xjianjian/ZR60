/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./key/bsp_exti.h"
static void NVIC_Configuration(void);
/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ����NVICΪ���ȼ���1 */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* �����ж�Դ������2 */
	NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
	/* ������ռ���ȼ���1 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	/* ���������ȼ���1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* ʹ���ж�ͨ�� */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */

void Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK|DOORLOCK_GPIO_CLK|KEY2_INT_GPIO_CLK| \
						   KEY3_INT_GPIO_CLK|KEY4_INT_GPIO_CLK ,ENABLE);
	/* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* ѡ�񰴼�1������ */ 
	GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
	/* ��������Ϊ����ģʽ */ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
	/* �������Ų�����Ҳ������ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* ʹ������Ľṹ���ʼ������ */
	GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ѡ������״̬�������� */ 
	GPIO_InitStructure.GPIO_Pin = DOORLOCK_GPIO_PIN; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/* ����������������ͬ */
	GPIO_Init(DOORLOCK_GPIO_PORT, &GPIO_InitStructure);
	
	/* ѡ�񰴼�2������ */ 
	GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* ����������������ͬ */
	GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      


	  /* ѡ�񰴼�3������ */ 
	GPIO_InitStructure.GPIO_Pin = KEY3_INT_GPIO_PIN;  
	GPIO_Init(KEY3_INT_GPIO_PORT, &GPIO_InitStructure);      

	/*ѡ�񰴼�4������*/
	GPIO_InitStructure.GPIO_Pin = KEY4_INT_GPIO_PIN;   
	GPIO_Init(KEY4_INT_GPIO_PORT, &GPIO_InitStructure); 
	
	
	
	/* ���� EXTI �ж�Դ ��key2 ���� */
	SYSCFG_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE,KEY2_INT_EXTI_PINSOURCE);

	/* ѡ�� EXTI �ж�Դ */
	EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �����ش��� */
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_Configuration();
}



/*********************************************END OF FILE**********************/
