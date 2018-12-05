/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA8   ------> RCC_MCO
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

}

/* USER CODE BEGIN 2 */
//初始化IIC
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     				
	WRITE_SDA=1;	  	  
	WRITE_SCL=1;
	delay_us(4);
 	WRITE_SDA=0;					
	delay_us(4);
	WRITE_SCL=0;					
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();
	WRITE_SCL=0;
	WRITE_SDA=0;
 	delay_us(4);
	WRITE_SCL=1; 
	WRITE_SDA=1;
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      
	WRITE_SDA=1;delay_us(1);	   
	WRITE_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	WRITE_SCL=0;
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	WRITE_SCL=0;
	SDA_OUT();
	WRITE_SDA=0;
	delay_us(2);
	WRITE_SCL=1;
	delay_us(2);
	WRITE_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	WRITE_SCL=0;
	SDA_OUT();
	WRITE_SDA=1;
	delay_us(2);
	WRITE_SCL=1;
	delay_us(2);
	WRITE_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t t;   
	SDA_OUT(); 	    
	WRITE_SCL=0;
	for(t=0;t<8;t++)
	{              
		if((txd&0x80)>>7)
			WRITE_SDA=1;
		else
			WRITE_SDA=0;
		txd<<=1; 	  
		delay_us(2);   
		WRITE_SCL=1;
		delay_us(2); 
		WRITE_SCL=0;	
		delay_us(2);
	}	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++ )
	{
		WRITE_SCL=0; 
		delay_us(2);
		WRITE_SCL=1;
		receive<<=1;
		if(READ_SDA)
			receive++;   
		delay_us(1); 
	}					 
  if (!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}

void delay_us(uint16_t us)
{
	uint16_t i;
	__IO uint16_t j;
	for(i=0;i<us;i++)
		for(j=0;j<65;j++);
}

uint8_t IIC_Write_One_Byte(uint8_t DevAddr,uint8_t RegAddr,uint8_t Data)
{
	uint8_t res;
	IIC_Start();  
	IIC_Send_Byte(DevAddr&0xFE);
	IIC_Wait_Ack();	   
	IIC_Send_Byte(RegAddr);
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Data);						   
	res=IIC_Wait_Ack();  		    	   
	IIC_Stop();
	return res;
	//HAL_Delay(10);	 
}

uint8_t IIC_Read_One_Byte(uint8_t DevAddr,uint8_t RegAddr)
{
	uint8_t temp=0;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(DevAddr&0xFE);
	IIC_Wait_Ack();
	IIC_Send_Byte(RegAddr);
	IIC_Wait_Ack();		  
	IIC_Start();  	 	   
	IIC_Send_Byte(DevAddr|0x01);	   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();    
	return temp;	
}

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
