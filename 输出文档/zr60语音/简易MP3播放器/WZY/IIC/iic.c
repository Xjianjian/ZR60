#include   "iic.h"
#include   "delay.h"
/****************************************************************************************
IIC����IOģ��ʱ��ʵ��
IIC_SCL  PB8
IIC_SDA  PB9
****************************************************************************************/
//��ʼ��GPIO��
void  IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}
void  IIC_Start(void)
{
    IIC_SDA_OUT();
	IIC_SCL=1;
	IIC_SDA_WRITE=1;
	delay_us(5);
	IIC_SDA_WRITE=0;
	delay_us(5);
	IIC_SCL=0;
}
void  IIC_Stop(void)
{
    IIC_SDA_OUT();
	IIC_SCL=0;
	IIC_SDA_WRITE=0;
	delay_us(4);
	IIC_SCL=1;
	delay_us(5);
	IIC_SDA_WRITE=1;
	delay_us(5);
	IIC_SDA_WRITE=0;

}
uint8_t IIC_WaitACK(void)
{
	uint8_t TimeErrCounter=0;
    IIC_SDA_IN();
	IIC_SDA_WRITE=1;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	while(IIC_SDA_READ)
	{
	    TimeErrCounter++;
        if(TimeErrCounter>=255)
		{
		   IIC_Stop();
		   return 1;
		}			
	}
    IIC_SCL=0;
	return 0;
}
//��������ACK�ź�
void  IIC_ACK(void)
{
	IIC_SDA_OUT();
    IIC_SCL=0;
    IIC_SDA_WRITE=0;	
    delay_us(2);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
	delay_us(2);
	IIC_SDA_WRITE=1;
}
void  IIC_NACK(void)
{
    IIC_SDA_OUT();
	IIC_SCL=0;
	IIC_SDA_WRITE=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
	delay_us(2);
	IIC_SDA_WRITE=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��	
void  IIC_SendByte(uint8_t byte)
{
	uint8_t i;
    IIC_SDA_OUT();
	IIC_SCL=0;
	for(i=0;i<8;i++)
	{
	   IIC_SDA_WRITE=(byte&0x80)>>7;
	   byte<<=1;
	   delay_us(2);
	   IIC_SCL=1;
	   delay_us(2);
	   IIC_SCL=0;
	   delay_us(2);	
	}		
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK 
uint8_t IIC_ReadByte(uint8_t ack)
{
	uint8_t i,res=0;
	IIC_SDA_IN();
	for(i=0;i<8;i++)
	{
	  IIC_SCL=0;
	  delay_us(2);
	  IIC_SCL=1;
	  res<<=1;
	  if(IIC_SDA_READ)res++;
	  delay_us(1);
	}
	if(!ack)
	{
	  IIC_NACK();
	}else
	{
	  IIC_ACK();
	}
	return res;	
}

