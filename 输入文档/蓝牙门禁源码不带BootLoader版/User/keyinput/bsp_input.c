/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./keyinput/bsp_input.h" 
#include "./systick/bsp_SysTick.h"


void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK,ENABLE);
	
//  /*选择按键的引脚*/
//	GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
//  
//  /*设置引脚为输入模式*/
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
//  
//  /*设置引脚不上拉也不下拉*/
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	
//  /*使用上面的结构体初始化按键*/
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
	
	
	  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = ALARM_PIN; 
  
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  /*使用上面的结构体初始化按键*/
	GPIO_Init(ALARM_GPIO_PORT, &GPIO_InitStructure);  
  

}

/**
  * @brief   检测是否有按键按下     
  *	@param 	GPIOx:具体的端口, x可以是（A...K） 
	*	@param 	GPIO_PIN:具体的端口位， 可以是GPIO_PIN_x（x可以是0...15）
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
			delay_ms(10);
				if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
			{	
					return KEY_ON;
			}
	}
	else
		return KEY_OFF;
}
/*********************************************END OF FILE**********************/
