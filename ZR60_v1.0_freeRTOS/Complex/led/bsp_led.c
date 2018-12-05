/***********************************************************
  * @file    bsp_led.c
  * @author  liujian
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ledӦ�ú����ӿ�
***********************************************************/
  
#include "./led/bsp_led.h"   
#include "stm32f4xx_gpio.h"
 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd ( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK | OPEN_DOOR_GPIO_CLK, ENABLE); 

	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;	

	/*��������ģʽΪ���ģʽ*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   

	/*�������ŵ��������Ϊ�������*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	/*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	/*������������Ϊ2MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	

	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;	
	GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);	

	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStructure.GPIO_Pin = LED3_PIN;	
	GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);	
	
	/*ѡ��Ҫ���Ƶ�read_card en����*/															   
	GPIO_InitStructure.GPIO_Pin = READ_CARD_EN_PIN;	
	GPIO_Init(READ_CARD_EN_GPIO_PORT, &GPIO_InitStructure);	
	
	/*ѡ��Ҫ���ƵĿ�������*/													   
	GPIO_InitStructure.GPIO_Pin = OPEN_DOOR_PIN;	
	GPIO_Init(OPEN_DOOR_GPIO_PORT, &GPIO_InitStructure);	

	/*�ر�RGB��*/
	LED_RGBOFF;
	/*�򿪶�������Դ*/
	digitalHi(READ_CARD_EN_GPIO_PORT,READ_CARD_EN_PIN)
}
/*********************************************END OF FILE**********************/
