#include "lcd12864.h"
#include "delay.h"
#include "lcd_table.h"
/*************************************************************************************
��Ҫʵ��
�������߶����ݺ�����ƴ�գ�
��������д���ݺ�����ƴ�գ�
д����/���ݺ���
��������λ��һ����
2015��4��14�����
2015��4��17���޸�
��������ʵ�ֻ��㺯��
ʵ��һ��[32][32]RAM������
��ʽ����
//[0]0 1 2 3 ... 31(�ֽ�)	
//[1]0 1 2 3 ... 31(�ֽ�)
//[2]0 1 2 3 ... 31(�ֽ�)	
//[3]0 1 2 3 ... 31(�ֽ�)	

���˻�����ֱ�Ӹ��µ�GDRAM�м���
*************************************************************************************/
/*************************************************************************************
  LCD12864
  LCD_RS     PB1
  LCD_RW     PE3
  LCD_EN     PE4
  LCD_PSB    PE5
  LCD_RST    �ӵ�����RST��
  LCD_D0     PE6
  LCD_D1     PF6
  LCD_D2     PF11
  LCD_D3     PG6
  LCD_D4     PG7
  LCD_D5     PG8
  LCD_D6     PG12
  LCD_D7     PG15
  LCD_BL     PB0
 ************************************************************************************/
uint8_t LCD_GRAM[32][32];

void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE|
	                       RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG
	                       ,ENABLE);
	//PB0,PB1
	GPIO_InitStructure.GPIO_Pin=LCD_BL_Pin|LCD_RS_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LCD_RS_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LCD_RS_PORT,LCD_RS_Pin);
//    GPIO_ResetBits(LCD_BL_PORT,LCD_BL_Pin);//��������
	GPIO_SetBits(LCD_BL_PORT,LCD_BL_Pin);//�رձ���
	//PE3,PE4,PE5
	GPIO_InitStructure.GPIO_Pin=LCD_RW_Pin|LCD_EN_Pin|LCD_PSB_Pin;
	GPIO_Init(LCD_EN_PORT,&GPIO_InitStructure);	
    GPIO_SetBits(LCD_EN_PORT,LCD_RW_Pin|LCD_EN_Pin|LCD_PSB_Pin);	

	GPIO_InitStructure.GPIO_Pin=LCD_D0_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����	
	GPIO_Init(LCD_D0_PORT,&GPIO_InitStructure);	
	GPIO_SetBits(LCD_D0_PORT,LCD_D0_Pin);
	GPIO_InitStructure.GPIO_Pin=LCD_D1_Pin|LCD_D2_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����	
	GPIO_Init(LCD_D1_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LCD_D1_PORT,LCD_D1_Pin|LCD_D2_Pin);
	GPIO_InitStructure.GPIO_Pin=LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(LCD_D3_PORT,&GPIO_InitStructure); 
    GPIO_SetBits(LCD_D3_PORT,LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin);	
}
//���ݸ�ʽ
//D7 D6 D5 D4 D3 D2 D1 D0
uint8_t  ReadByteFromLCD(void)
{
    uint8_t res=0;
	res=(LCD_D0_IN<<0)|(LCD_D1_IN<<1)
	   |(LCD_D2_IN<<2)|(LCD_D3_IN<<3)
	   |(LCD_D4_IN<<4)|(LCD_D5_IN<<5)
	   |(LCD_D6_IN<<6)|(LCD_D7_IN<<7);
	return res;

}

void WriteByteToLCD(uint8_t byte)
{

     LCD_D0_OUT=(byte&0x01)>>0;
	 LCD_D1_OUT=(byte&0x02)>>1;
	 LCD_D2_OUT=(byte&0x04)>>2;
	 LCD_D3_OUT=(byte&0x08)>>3;
	 LCD_D4_OUT=(byte&0x10)>>4;
	 LCD_D5_OUT=(byte&0x20)>>5;
	 LCD_D6_OUT=(byte&0x40)>>6;
	 LCD_D7_OUT=(byte&0x80)>>7;
}
/*************************************************************************************
 
 ************************************************************************************/
/*************************************************************************************
 LCD��æ����
 1:æ
 0:��æ
 ************************************************************************************/
uint8_t  LCD_Busy(void)
{
  uint8_t  res=0;
  LCD_RS = 0;
  LCD_RW = 1;
  LCD_EN = 1;
  //������IO�����趨
  GPIO_Set(GPIOE,PIN6,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
  GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); 
  GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
  delay_us(5);
  //������
  res=(ReadByteFromLCD()&0x80);
  LCD_EN = 0;	
  
  return res;
}
//дָ��/���ݵ�LCD
void  WriteToLCD(uint8_t mode,uint8_t byte)
{
    
	if(mode==LCD_CMD)//дָ��
	{
	  while(LCD_Busy());
	  LCD_RS = 0;//ָ��
      LCD_RW = 0;//д
      LCD_EN = 0;
	  delay_us(5);
      GPIO_Set(GPIOE,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
      GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU); 
      GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
	  WriteByteToLCD(byte);//д
	  delay_us(5);
	  
	  LCD_EN = 1;
	  delay_us(5);
	  LCD_EN = 0;
      		
	}else            //д����
	{
	   while(LCD_Busy());
	   LCD_RS = 1;//����
	   LCD_RW = 0;
	   LCD_EN = 0;
	   delay_us(5);
       GPIO_Set(GPIOE,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
       GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU); 
       GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
	   WriteByteToLCD(byte);//д
	   delay_us(5);
	  
	   LCD_EN = 1;
	   delay_us(5);
	   LCD_EN = 0;	   
	}
}
void  LCD12864_Init(void)
{ 
   LCD_PSB=1;
   WriteToLCD(LCD_CMD,0x34);//����ָ�����
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x30);//����ָ�����
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x0C);//��ʾ�����ع��
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x01);//��LCD
   delay_ms(5);   	
}
void LCD_Clear(void)
{
   WriteToLCD(LCD_CMD,0x01);//��LCD
   delay_ms(5);  
}
//�趨��ʾλ��
//x=0-3,y=0-7
//128*64=16*8*16*4
void LCD_SetCursor(uint8_t x,uint8_t y)
{                          
  if (x==0)
    {x=0x80;}
  else if (x==1)
    {x=0x90;}
  else if (x==2)
    {x=0x88;}
  else if (y==3)
    {x=0x98;}
  
  WriteToLCD(LCD_CMD,x+y);     //��ʾ��ַ
}
void LCD_ShowString(uint8_t x,uint8_t y, uint8_t *s)
{
    LCD_SetCursor(x,y);
	while(*s!='\0')
	{
	  WriteToLCD(LCD_DATA,*s++);
	}
	*s=0;
}
//���㺯��
/*
                            GDRAM�ֲ�
                  256(16*16)                                         X
   |-----------------------------|-----------------------------------|
   |0                            |                                   |
   |      �ϰ���128*32           |      �°���128*32                 |
  Y|32                           |                                   |
   |-----------------------------|-----------------------------------|
ˮƽx����ÿ��16�������һ��λַ����˸ı�ĳ������Ҫ֪���õ�λ����16������ĸ�λ��
��ֱy�����ڴ���31ʱ�����°�����С��31�����ϰ���
*/
void LCD_SetPoint(uint8_t x,uint8_t y)
{
    uint16_t volatile  readdata=0;
    //�жϴ�����������
	uint8_t x_pos,x_bit;//x_pos�����жϴ���λַ��x_bit�����жϴ���λַ�е�λ��
    uint8_t y_pos,y_bit;//y_pos�����жϴ����ϰ��������°��� y_bit�����ж�λ������              
	y_pos=y/32;   //0:�ϰ�����1:�°���
	y_bit=y%32;   //�õ�������λ��
	x_pos=x/16;
	x_bit=x%16;
	WriteToLCD(LCD_CMD,LCD_EXTERN_SET);//��չָ�
    WriteToLCD(LCD_CMD,LCD_DRAW_OFF);//�ص���ͼ��ʾ
	WriteToLCD(LCD_CMD,0x80+y_bit); 
	WriteToLCD(LCD_CMD,0x80+8*y_pos+x_pos);
	 //������IO�����趨
    GPIO_Set(GPIOE,PIN6,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
    GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_IN,0,0,GPIO_PUPD_PD); 
    GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	delay_ms(1);
    ReadByteFromLCD();//�ն�һ��
	readdata=ReadByteFromLCD();
	readdata<<=8;
	readdata|=ReadByteFromLCD();
	WriteToLCD(LCD_CMD,0x80+y_bit); 
	WriteToLCD(LCD_CMD,0x80+8*y_pos+x_pos);	
	if(x_bit<8)
	{
	  WriteToLCD(LCD_DATA,((uint8_t)(readdata>>8))|(0x01<<(7-x_bit)));
	  WriteToLCD(LCD_DATA,(uint8_t)readdata);
	}else
	{
	  WriteToLCD(LCD_DATA,(uint8_t)(readdata>>8));
	  WriteToLCD(LCD_DATA,(uint8_t)readdata|(0x01<<(15-x_bit)));
	}

	WriteToLCD(LCD_CMD,LCD_DRAW_ON);//����ͼ��ʾ
    WriteToLCD(LCD_CMD,LCD_BASIC_SET);//�ص�����ָ�
}

void LCD_Refresh_GRAM(void)
{
     uint8_t i;uint16_t j;
	 WriteToLCD(LCD_CMD,LCD_EXTERN_SET);//��չָ�
     WriteToLCD(LCD_CMD,LCD_DRAW_OFF);//�ص���ͼ��ʾ
	 for(i=0;i<32;i++)//����0-31��
	 {
       WriteToLCD(LCD_CMD,0x80+i);//д���е�ַ
	   WriteToLCD(LCD_CMD,0x80);  //д���е�ַ
       for(j=0;j<32;j++)
       {
	     WriteToLCD(LCD_DATA,LCD_GRAM[i][j]);  
	   }		 
	 }	
	WriteToLCD(LCD_CMD,LCD_DRAW_ON);//������ͼ��ʾ
	WriteToLCD(LCD_CMD,LCD_BASIC_SET);//�ص�����ָ�	
  
}
void  LCD_Clear_DGRAM(void)
{
	uint8_t i,j;
	for(i=0;i<32;i++)//����
	{
	   for(j=0;j<32;j++)//ÿ��ռ��λ�� 16*16/8=32���ֽ�
	   {
	       LCD_GRAM[i][j]=0x00;           		   
		} 
	}
//   LCD_Refresh_GRAM();
}
//x:0-127
//y:0-63
//mode:1,����
//mode:0,���
void  LCD_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
    //�жϴ�����������
	uint8_t x_pos,x_bit;//x_pos�����жϴ���λַ��x_bit�����жϴ���λַ�е�λ��
    uint8_t y_pos,y_bit;//y_pos�����жϴ����ϰ��������°��� y_bit�����ж�λ������ 
//	uint8_t x_pos_temp;
//    if((x>127)||(y>63)||(x<0)||(y<0))return;//ȥ�����������	
	y_pos=y/32;   //0:�ϰ�����1:�°���
	y_bit=y%32;   //�õ�������λ��
	x_pos=x/16;
	x_bit=x%16;
    if(y_pos>0)//�°���
	{
		  if(mode) 
		 {
			if(x_bit<8)
			{
			  LCD_GRAM[y_bit][x_pos*2+16]|=(1<<(7-x_bit));
			  LCD_GRAM[y_bit][x_pos*2+1+16]|=0x00;
			}else
			{
			  LCD_GRAM[y_bit][x_pos*2+16]|=0x00;
			  LCD_GRAM[y_bit][x_pos*2+1+16]|=(1<<(15-x_bit));
			}
		 }
		else 
		{	
			if(x_bit<8)
			{
			  LCD_GRAM[y_bit][x_pos*2+16]&=~(1<<(7-x_bit));
			  LCD_GRAM[y_bit][x_pos*2+1+16]&=~0x00;
			}else
			{
			  LCD_GRAM[y_bit][x_pos*2+16]&=~0x00;
			  LCD_GRAM[y_bit][x_pos*2+1+16]&=~(1<<(15-x_bit));
			}
		} 
	}else//�ϰ���
	{
		 if(mode) 
		 {
			if(x_bit<8)
			{
			  LCD_GRAM[y_bit][x_pos*2]|=(1<<(7-x_bit));
			  LCD_GRAM[y_bit][x_pos*2+1]|=0x00;
			}else
			{
			  LCD_GRAM[y_bit][x_pos*2]|=0x00;
			  LCD_GRAM[y_bit][x_pos*2+1]|=(1<<(15-x_bit));
			}
		 }
		else 
		{	
			if(x_bit<8)
			{
			  LCD_GRAM[y_bit][x_pos*2]&=~(1<<(7-x_bit));
			  LCD_GRAM[y_bit][x_pos*2+1]&=~0x00;
			}else
			{
			  LCD_GRAM[y_bit][x_pos*2]&=~0x00;
			  LCD_GRAM[y_bit][x_pos*2+1]&=~(1<<(15-x_bit));
			}
		 }
	}

//   	LCD_Refresh_GRAM();
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void LCD_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=lcd_asc2_1206[chr][t]; //����1206����
		else if(size==16) temp=lcd_asc2_1608[chr][t];		 //����1608���� 	                          
        else if(size==24)temp=lcd_asc2_2412[chr][t];        //����2412����
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }     
//    LCD_Refresh_GRAM();	
}
