#include "lcd12864.h"
#include "delay.h"
#include "lcd_table.h"
/*************************************************************************************
需要实现
从数据线读数据函数（拼凑）
向数据线写数据函数（拼凑）
写命令/数据函数
点亮任意位置一个点
2015年4月14日完成
2015年4月17日修改
更换方法实现画点函数
实现一个[32][32]RAM缓冲区
格式如下
//[0]0 1 2 3 ... 31(字节)	
//[1]0 1 2 3 ... 31(字节)
//[2]0 1 2 3 ... 31(字节)	
//[3]0 1 2 3 ... 31(字节)	

将此缓冲区直接更新到GDRAM中即可
*************************************************************************************/
/*************************************************************************************
  LCD12864
  LCD_RS     PB1
  LCD_RW     PE3
  LCD_EN     PE4
  LCD_PSB    PE5
  LCD_RST    接到板子RST上
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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
	GPIO_Init(LCD_RS_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LCD_RS_PORT,LCD_RS_Pin);
//    GPIO_ResetBits(LCD_BL_PORT,LCD_BL_Pin);//点亮背光
	GPIO_SetBits(LCD_BL_PORT,LCD_BL_Pin);//关闭背光
	//PE3,PE4,PE5
	GPIO_InitStructure.GPIO_Pin=LCD_RW_Pin|LCD_EN_Pin|LCD_PSB_Pin;
	GPIO_Init(LCD_EN_PORT,&GPIO_InitStructure);	
    GPIO_SetBits(LCD_EN_PORT,LCD_RW_Pin|LCD_EN_Pin|LCD_PSB_Pin);	

	GPIO_InitStructure.GPIO_Pin=LCD_D0_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉	
	GPIO_Init(LCD_D0_PORT,&GPIO_InitStructure);	
	GPIO_SetBits(LCD_D0_PORT,LCD_D0_Pin);
	GPIO_InitStructure.GPIO_Pin=LCD_D1_Pin|LCD_D2_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉	
	GPIO_Init(LCD_D1_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LCD_D1_PORT,LCD_D1_Pin|LCD_D2_Pin);
	GPIO_InitStructure.GPIO_Pin=LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
	GPIO_Init(LCD_D3_PORT,&GPIO_InitStructure); 
    GPIO_SetBits(LCD_D3_PORT,LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin);	
}
//数据格式
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
 LCD判忙函数
 1:忙
 0:不忙
 ************************************************************************************/
uint8_t  LCD_Busy(void)
{
  uint8_t  res=0;
  LCD_RS = 0;
  LCD_RW = 1;
  LCD_EN = 1;
  //数据线IO方向设定
  GPIO_Set(GPIOE,PIN6,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
  GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); 
  GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
  delay_us(5);
  //读数据
  res=(ReadByteFromLCD()&0x80);
  LCD_EN = 0;	
  
  return res;
}
//写指令/数据到LCD
void  WriteToLCD(uint8_t mode,uint8_t byte)
{
    
	if(mode==LCD_CMD)//写指令
	{
	  while(LCD_Busy());
	  LCD_RS = 0;//指令
      LCD_RW = 0;//写
      LCD_EN = 0;
	  delay_us(5);
      GPIO_Set(GPIOE,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
      GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU); 
      GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
	  WriteByteToLCD(byte);//写
	  delay_us(5);
	  
	  LCD_EN = 1;
	  delay_us(5);
	  LCD_EN = 0;
      		
	}else            //写数据
	{
	   while(LCD_Busy());
	   LCD_RS = 1;//数据
	   LCD_RW = 0;
	   LCD_EN = 0;
	   delay_us(5);
       GPIO_Set(GPIOE,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
       GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU); 
       GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
	   WriteByteToLCD(byte);//写
	   delay_us(5);
	  
	   LCD_EN = 1;
	   delay_us(5);
	   LCD_EN = 0;	   
	}
}
void  LCD12864_Init(void)
{ 
   LCD_PSB=1;
   WriteToLCD(LCD_CMD,0x34);//扩充指令操作
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x30);//基本指令操作
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x0C);//显示开，关光标
   delay_ms(100);
   WriteToLCD(LCD_CMD,0x01);//清LCD
   delay_ms(5);   	
}
void LCD_Clear(void)
{
   WriteToLCD(LCD_CMD,0x01);//清LCD
   delay_ms(5);  
}
//设定显示位置
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
  
  WriteToLCD(LCD_CMD,x+y);     //显示地址
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
//画点函数
/*
                            GDRAM分布
                  256(16*16)                                         X
   |-----------------------------|-----------------------------------|
   |0                            |                                   |
   |      上半屏128*32           |      下半屏128*32                 |
  Y|32                           |                                   |
   |-----------------------------|-----------------------------------|
水平x坐标每隔16个点才有一个位址，因此改变某个点需要知道该点位于这16个点的哪个位置
垂直y坐标在大于31时处于下半屏，小于31处于上半屏
*/
void LCD_SetPoint(uint8_t x,uint8_t y)
{
    uint16_t volatile  readdata=0;
    //判断处于哪行哪列
	uint8_t x_pos,x_bit;//x_pos用来判断处于位址，x_bit用来判断处于位址中的位置
    uint8_t y_pos,y_bit;//y_pos用来判断处于上半屏还是下半屏 y_bit用来判断位于哪行              
	y_pos=y/32;   //0:上半屏；1:下半屏
	y_bit=y%32;   //得到具体行位置
	x_pos=x/16;
	x_bit=x%16;
	WriteToLCD(LCD_CMD,LCD_EXTERN_SET);//扩展指令集
    WriteToLCD(LCD_CMD,LCD_DRAW_OFF);//关掉绘图显示
	WriteToLCD(LCD_CMD,0x80+y_bit); 
	WriteToLCD(LCD_CMD,0x80+8*y_pos+x_pos);
	 //数据线IO方向设定
    GPIO_Set(GPIOE,PIN6,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
    GPIO_Set(GPIOF,PIN6|PIN11,GPIO_MODE_IN,0,0,GPIO_PUPD_PD); 
    GPIO_Set(GPIOG,PIN6|PIN7|PIN8|PIN12|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	delay_ms(1);
    ReadByteFromLCD();//空读一次
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

	WriteToLCD(LCD_CMD,LCD_DRAW_ON);//开绘图显示
    WriteToLCD(LCD_CMD,LCD_BASIC_SET);//回到基本指令集
}

void LCD_Refresh_GRAM(void)
{
     uint8_t i;uint16_t j;
	 WriteToLCD(LCD_CMD,LCD_EXTERN_SET);//扩展指令集
     WriteToLCD(LCD_CMD,LCD_DRAW_OFF);//关掉绘图显示
	 for(i=0;i<32;i++)//遍历0-31行
	 {
       WriteToLCD(LCD_CMD,0x80+i);//写入行地址
	   WriteToLCD(LCD_CMD,0x80);  //写入列地址
       for(j=0;j<32;j++)
       {
	     WriteToLCD(LCD_DATA,LCD_GRAM[i][j]);  
	   }		 
	 }	
	WriteToLCD(LCD_CMD,LCD_DRAW_ON);//开启绘图显示
	WriteToLCD(LCD_CMD,LCD_BASIC_SET);//回到基本指令集	
  
}
void  LCD_Clear_DGRAM(void)
{
	uint8_t i,j;
	for(i=0;i<32;i++)//清屏
	{
	   for(j=0;j<32;j++)//每行占用位置 16*16/8=32个字节
	   {
	       LCD_GRAM[i][j]=0x00;           		   
		} 
	}
//   LCD_Refresh_GRAM();
}
//x:0-127
//y:0-63
//mode:1,画点
//mode:0,清空
void  LCD_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
    //判断处于哪行哪列
	uint8_t x_pos,x_bit;//x_pos用来判断处于位址，x_bit用来判断处于位址中的位置
    uint8_t y_pos,y_bit;//y_pos用来判断处于上半屏还是下半屏 y_bit用来判断位于哪行 
//	uint8_t x_pos_temp;
//    if((x>127)||(y>63)||(x<0)||(y<0))return;//去掉不合理参数	
	y_pos=y/32;   //0:上半屏；1:下半屏
	y_bit=y%32;   //得到具体行位置
	x_pos=x/16;
	x_bit=x%16;
    if(y_pos>0)//下半屏
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
	}else//上半屏
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
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void LCD_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=lcd_asc2_1206[chr][t]; //调用1206字体
		else if(size==16) temp=lcd_asc2_1608[chr][t];		 //调用1608字体 	                          
        else if(size==24)temp=lcd_asc2_2412[chr][t];        //调用2412字体
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
