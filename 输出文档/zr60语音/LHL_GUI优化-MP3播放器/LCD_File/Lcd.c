/* Includes ------------------------------------------------------------------*/
#include "Lcd.h"
#include "stdlib.h"

#define LCD_On() GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define LCD_Off() GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define Delay_ms   delay_ms
vu16 DeviceCode=0;
static void Delay_ms(u32 count)
{
	count*=50000;
	while(count--)
	{
	}
}
/*******************************************************************************
* Function Name  : LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{ 
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();
  Delay_ms(10);
/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();

  Delay_ms(40);
  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
  Delay_ms(40);
  GPIO_SetBits(GPIOC,GPIO_Pin_6);

  //printf("复位完成\n");

  Delay_ms(20); /* delay 50 ms */
  LCD_WriteReg(0x0000,0x0001);
  Delay_ms(20); /* delay 50 ms */
  DeviceCode=LCD_ReadReg(0x0000);

  if(DeviceCode==0x9325||DeviceCode==0x9328)//ILI9325
	{
  		LCD_WriteReg(0x00e5,0x78f0);
		LCD_WriteReg(0x00e3,0x3008); 
		LCD_WriteReg(0x00e7,0x0012);
		LCD_WriteReg(0x00ef,0x1231);
		      
        LCD_WriteReg(0x0000,0x0001);//开启内部时钟
        LCD_WriteReg(0x0001,0x0100);     
        LCD_WriteReg(0x0002,0x0700);//电源开启                    
		//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
        LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K  0x1030  
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0207);	           
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);//display setting         
        LCD_WriteReg(0x000c,0x0000);//display setting   16位 system接口       
        LCD_WriteReg(0x000d,0x0000);//0f3c          
        LCD_WriteReg(0x000f,0x0000);
		//电源配置
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007); 
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000); 
		LCD_WriteReg(0x0007,0x0001);                 
        Delay_ms(100); 
        LCD_WriteReg(0x0010,0x17b0);  
        LCD_WriteReg(0x0011,0x0137);  
        Delay_ms(50); 
        LCD_WriteReg(0x0012,0x0139);  //改  0x139               
        Delay_ms(50); 
        LCD_WriteReg(0x0013,0x1700);   
        LCD_WriteReg(0x0029,0x000c); //改  0x000c
        LCD_WriteReg(0x002b,0x000c);
        Delay_ms(50); 
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);           
		Delay_ms(50); 
		//伽马校正
        LCD_WriteReg(0x0030,0x0000);
		LCD_WriteReg(0x0031,0x0507);
		LCD_WriteReg(0x0032,0x0104);
		LCD_WriteReg(0x0035,0x0105);
		LCD_WriteReg(0x0036,0x0404);
		LCD_WriteReg(0x0037,0x0603);
		LCD_WriteReg(0x0038,0x0004);
		LCD_WriteReg(0x0039,0x0007);
		LCD_WriteReg(0x003c,0x0501);
		LCD_WriteReg(0x003d,0x0404);
        Delay_ms(50); 
        LCD_WriteReg(0x0050,0x0000); //水平GRAM起始位置 
        LCD_WriteReg(0x0051,0x00ef); //水平GRAM终止位置                    
        LCD_WriteReg(0x0052,0x0000); //垂直GRAM起始位置                    
        LCD_WriteReg(0x0053,0x013f); //垂直GRAM终止位置  
        
        LCD_WriteReg(0x0060,0xa700);        
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
      
        LCD_WriteReg(0x0090,0x0010);     
        LCD_WriteReg(0x0092,0x0000);  
        LCD_WriteReg(0x0093,0x0003);
        LCD_WriteReg(0x0095,0x0110);
        LCD_WriteReg(0x0097,0x0000);        
        LCD_WriteReg(0x0098,0x0000);  
        //开启显示设置    
        LCD_WriteReg(0x0007,0x0133);   
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);

	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
      Delay_ms(20);                     //根据不同晶振速度可以调整延时，保障稳定显示 
	   LCD_WriteReg(0x00e5, 0x8000); 
	   LCD_WriteReg(0x0000, 0x0001); 
	   LCD_WriteReg(0x002b, 0x0010); 
	   LCD_WriteReg(0x0001, 0x0100); 
	   LCD_WriteReg(0x0002, 0x0700); 
		LCD_WriteReg(0x0003, 0x1018);  
	   LCD_WriteReg(0x0004, 0x0000); 
	   LCD_WriteReg(0x0008, 0x0202); 
	   LCD_WriteReg(0x0009, 0x0000); 
	   LCD_WriteReg(0x000A, 0x0000); 
	   LCD_WriteReg(0x000C, 0x0000); 
	   LCD_WriteReg(0x000D, 0x0000); 
	   LCD_WriteReg(0x000F, 0x0000); 
	//-----Power On sequence----------------------- 
	   LCD_WriteReg(0x0010, 0x0000); 
	   LCD_WriteReg(0x0011, 0x0000); 
	   LCD_WriteReg(0x0012, 0x0000); 
	   LCD_WriteReg(0x0013, 0x0000);  
	   LCD_WriteReg(0x0010, 0x17B0);
	   LCD_WriteReg(0x0011, 0x0004);
	   LCD_WriteReg(0x0012, 0x013e);
	   LCD_WriteReg(0x0013, 0x1f00);
	   LCD_WriteReg(0x0029, 0x000f);
	//-----Gamma control----------------------- 
	   LCD_WriteReg(0x0030, 0x0000); 
	   LCD_WriteReg(0x0031, 0x0505); 
	   LCD_WriteReg(0x0032, 0x0004); 
	   LCD_WriteReg(0x0035, 0x0006); 
	   LCD_WriteReg(0x0036, 0x0707); 
	   LCD_WriteReg(0x0037, 0x0105); 
	   LCD_WriteReg(0x0038, 0x0002); 
	   LCD_WriteReg(0x0039, 0x0707); 
	   LCD_WriteReg(0x003C, 0x0704); 
	   LCD_WriteReg(0x003D, 0x0807); 
	//-----Set RAM area----------------------- 
	   LCD_WriteReg(0x0050, 0x0000); 
	   LCD_WriteReg(0x0051, 0x00EF); 
	   LCD_WriteReg(0x0052, 0x0000); 
	   LCD_WriteReg(0x0053, 0x013F); 
	   LCD_WriteReg(0x0060, 0x2700);
	   LCD_WriteReg(0x0061, 0x0001); 
	   LCD_WriteReg(0x006A, 0x0000); 
	   LCD_WriteReg(0x0021, 0x0000); 
	   LCD_WriteReg(0x0020, 0x0000); 
	//-----Partial Display Control------------ 
	   LCD_WriteReg(0x0080, 0x0000); 
	   LCD_WriteReg(0x0081, 0x0000); 
	   LCD_WriteReg(0x0082, 0x0000); 
	   LCD_WriteReg(0x0083, 0x0000); 
	   LCD_WriteReg(0x0084, 0x0000); 
	   LCD_WriteReg(0x0085, 0x0000); 
	//-----Panel Control---------------------- 
	   LCD_WriteReg(0x0090, 0x0010); 
	   LCD_WriteReg(0x0092, 0x0000); 
	   LCD_WriteReg(0x0093, 0x0003); 
	   LCD_WriteReg(0x0095, 0x0110); 
	   LCD_WriteReg(0x0097, 0x0000); 
	   LCD_WriteReg(0x0098, 0x0000); 
	//-----Display on----------------------- 
	   LCD_WriteReg(0x0007, 0x0173); 
	}
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  LCD_SetCursor(0x00, 0x0000); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 76800; index++)
  {
    *(__IO uint16_t *) (Bank1_LCD_D) = Color;
  }  
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
  LCD_WriteReg(0x20, Ypos);
  LCD_WriteReg(0x21, 319-Xpos);
}
/****************************************************************************
* 名    称：u16 LCD_BGR2RGB(u16 c)
* 功    能：RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* 入口参数：c      BRG 颜色值
* 出口参数：RGB 颜色值
* 说    明：内部函数调用
* 调用方法：
****************************************************************************/
u16 LCD_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  rgb =  (b<<11) + (g<<5) + (r<<0);

  return( rgb );
}
/****************************************************************************
* 名    称：u16 LCD_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=LCD_GetPoint(10,10);
****************************************************************************/
u16 LCD_GetPoint(u16 x,u16 y)
{
  LCD_SetCursor(x,y);
  return (LCD_BGR2RGB(LCD_ReadRAM()));
}
void LCD_DrawSinglePoint(u16 x,u16 y,u16 color)
{
  LCD_SetCursor(x,y);

  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(color);
}
/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
   *(__IO uint16_t *) (Bank1_LCD_R) = LCD_Reg;
  /* Write 16-bit Reg */
   *(__IO uint16_t *) (Bank1_LCD_D)= LCD_RegValue;
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  *(__IO uint16_t *) (Bank1_LCD_R)  = LCD_Reg;
  /* Read 16-bit Reg */
  return (*(__IO uint16_t *) (Bank1_LCD_D));
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  *(__IO uint16_t *) (Bank1_LCD_R) = 0x22;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  *(__IO uint16_t *) (Bank1_LCD_D) = RGB_Code;
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
u16 LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  *(__IO uint16_t *) (Bank1_LCD_R) = 0x22; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  *(__IO uint16_t *) (Bank1_LCD_D);
  return *(__IO uint16_t *) (Bank1_LCD_D);
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn(void)
{
  /* Display On */
  LCD_WriteReg(0x07, 0x0173); /* 262K color and display ON */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteReg(0x07, 0x0); 
}


/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /****开启时钟****/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOE, ENABLE); 	

  //LCD Reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	 //注意一定要设定为OUT_PP模式，不能为AF_PP模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/****端口复用****/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_FSMC);

  //FSMC的GPIOD管脚
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_14 	//D0
  												| GPIO_Pin_15 		//D1
  												| GPIO_Pin_0		//D2
  												| GPIO_Pin_1		//D3
  												| GPIO_Pin_8 		//D13
  												| GPIO_Pin_9 		//D14
  												| GPIO_Pin_10 		//D15  		
  												| GPIO_Pin_7		//NE1
  												| GPIO_Pin_11		//RS
  												| GPIO_Pin_4		//nRD
  												| GPIO_Pin_5;		//nWE
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15, GPIO_AF_FSMC);

  //FSMC的GPIOE管脚
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7		//D4
  												| GPIO_Pin_8 		//D5
  												| GPIO_Pin_9 		//D6
  												| GPIO_Pin_10 		//D7
  												| GPIO_Pin_11 		//D8
  												| GPIO_Pin_12 		//D9
  												| GPIO_Pin_13		//D10
  												| GPIO_Pin_14 		//D11
  												| GPIO_Pin_15; 	    //D12  												
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

  //  //背光控制
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB,GPIO_Pin_15);//拉高

}
/*******************************************************************************
* Function Name  : LCD_FSMCConfig
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

  /****开启时钟****/
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
  p.FSMC_AddressSetupTime = 3;//4;          //ADDSET 地址建立时间
  p.FSMC_AddressHoldTime = 8; //8          //ADDHOLD 地址保持时间
  p.FSMC_DataSetupTime = 10;//8;	           //DATAST 数据建立时间
  p.FSMC_BusTurnAroundDuration = 0;	   //BUSTURN 总线返转时间
  p.FSMC_CLKDivision = 0;			   //CLKDIV 时钟分频
  p.FSMC_DataLatency = 0;			   //DATLAT 数据保持时间
  p.FSMC_AccessMode = FSMC_AccessMode_A;   //ACCMOD FSMC 访问模式
    																					   
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;							   //选择设置的BANK及片选信号
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;			   //设置是否数据地址总线分时复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;						   //设置存储器类型
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;			   //设置数据宽度
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;	       //设置是否使用迸发访问模式(应该就是连续读写模式）
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	       //设置WAIT信号的有效电平
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;					       //设置是否使用环回模式 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //设置WAIT信号有效时机
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			   //设定是否使能写操作
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;					   //设定是否使用WAIT信号
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;			 	   //设定是否使用单独的写时序
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	       //设定是否使用异步等待信号
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;					   //设定是否使用迸发写模式
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;							   //设定读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* Enable FSMC_Bank1_NORSRAM1 */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

}

//===============================================================
//设置窗口
//入口参数: x0 y0,为显示窗口左上角坐标；x1,y1为显示窗口右下角坐标。
//出口参数: 无
//说明：该函数设置TFT模块的显示窗口。
void LCD_setwindow(unsigned char x0,unsigned int y0,unsigned char x1,unsigned int y1)
{
	LCD_WriteReg(0x0050,x0);//水平起始位置
	LCD_WriteReg(0x0051,x1);//水平终止位置
	LCD_WriteReg(0x0052,y0);//垂直起始位置
	LCD_WriteReg(0x0053,y1);//垂直终止位置
	LCD_WriteReg(0x0020,x0);//设置X坐标位置
	LCD_WriteReg(0x0021,y0);//设置Y坐标位置
	LCD_WriteRAM_Prepare();
}


