#include  "keyboard.h"
/***************************************************************************************
4*4���������������
��1-4 PB6  PB7 PA0 PC7 
��1-4 PC13 PD6 PD7 PE2
2015��4��15�����
***************************************************************************************/
//����Ϊ���� Ĭ��Ϊ1 ����Ϊ���Ĭ��Ϊ0	ɨ���� 
void  KEYBoard_LRInit_LScan(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);
	//PB6 PB7
	GPIO_InitStructure.GPIO_Pin=ROW_0_PIN|ROW_1_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_0_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(ROW_0_PORT,ROW_0_PIN|ROW_1_PIN);	
	// PA0
	GPIO_InitStructure.GPIO_Pin=ROW_2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_2_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(ROW_2_PORT,ROW_2_PIN);	
	// PC7
	GPIO_InitStructure.GPIO_Pin=ROW_3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_3_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(ROW_3_PORT,ROW_3_PIN);	
	//PC13
	GPIO_InitStructure.GPIO_Pin=LINE_0_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_0_PORT,&GPIO_InitStructure);	
	//PD6 PD7
	GPIO_InitStructure.GPIO_Pin=LINE_1_PIN|LINE_2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_1_PORT,&GPIO_InitStructure);	
	//PE2
	GPIO_InitStructure.GPIO_Pin=LINE_3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_3_PORT,&GPIO_InitStructure);
}
//����Ϊ��� Ĭ��Ϊ0 ����Ϊ����Ĭ��Ϊ1	ɨ����
void  KEYBoard_LRInit_RScan(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);
	//PB6 PB7
	GPIO_InitStructure.GPIO_Pin=ROW_0_PIN|ROW_1_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_0_PORT,&GPIO_InitStructure);
	//PA0
	GPIO_InitStructure.GPIO_Pin=ROW_2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_2_PORT,&GPIO_InitStructure);
	// PC7
	GPIO_InitStructure.GPIO_Pin=ROW_3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//��ͨ����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(ROW_3_PORT,&GPIO_InitStructure);
	//PC13
	GPIO_InitStructure.GPIO_Pin=LINE_0_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_0_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(LINE_0_PORT,LINE_0_PIN);
	//PD6 PD7
	GPIO_InitStructure.GPIO_Pin=LINE_1_PIN|LINE_2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_1_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(LINE_1_PORT,LINE_1_PIN|LINE_2_PIN);
	//PE2
	GPIO_InitStructure.GPIO_Pin=LINE_3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LINE_3_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(LINE_3_PORT,LINE_3_PIN);
}
uint8_t KEYBoard_GetInputVal(void)
{
    uint8_t res;
    res=(uint8_t)((LINE_0_IN<<0)|(LINE_1_IN<<1)|(LINE_2_IN<<2)
	   |(LINE_3_IN<<3)|(ROW_0_IN<<4) |(ROW_1_IN<<5)
	   |(ROW_2_IN<<6) |(ROW_3_IN<<7));    
    return res;
}
uint8_t KEYBoard_LRScan(void)
{
    uint8_t key=0;
	uint8_t temp=0;
	KEYBoard_LRInit_LScan();//ɨ����
	temp=(KEYBoard_GetInputVal()&0x0f);
	if(temp==0x0e)
	{
	   key=1;//��һ���м�������
	}else
	if(temp==0x0d)
	{
	   key=2;//�ڶ����м�������
	}else 
	if(temp==0x0b)
	{
	   key=3;
	}else 
	if(temp==0x07)
	{
	   key=4;
	}
	else key=0;
	KEYBoard_LRInit_RScan();//ɨ����
	temp=(KEYBoard_GetInputVal()&0xf0);
	if((temp==0xe0)&&(key!=0))
	{
	   key=(key-1)*4+1;
	}else
   if((temp==0xd0)&&(key!=0))
   {
      key=(key-1)*4+2;
   }else
   if((temp==0xb0)&&(key!=0))
   {
      key=(key-1)*4+3;
   }else
   if((temp==0x70)&&(key!=0))
   {
      key=(key-1)*4+4;
   }
   else key=0;
   return key;
}

