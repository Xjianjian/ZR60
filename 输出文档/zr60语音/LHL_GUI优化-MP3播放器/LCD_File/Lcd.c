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

  //printf("��λ���\n");

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
		      
        LCD_WriteReg(0x0000,0x0001);//�����ڲ�ʱ��
        LCD_WriteReg(0x0001,0x0100);     
        LCD_WriteReg(0x0002,0x0700);//��Դ����                    
		//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(�Ĵ��� 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D �����������.
		// 1  1   1	   U->D	L->R
        LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K  0x1030  
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0207);	           
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);//display setting         
        LCD_WriteReg(0x000c,0x0000);//display setting   16λ system�ӿ�       
        LCD_WriteReg(0x000d,0x0000);//0f3c          
        LCD_WriteReg(0x000f,0x0000);
		//��Դ����
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007); 
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000); 
		LCD_WriteReg(0x0007,0x0001);                 
        Delay_ms(100); 
        LCD_WriteReg(0x0010,0x17b0);  
        LCD_WriteReg(0x0011,0x0137);  
        Delay_ms(50); 
        LCD_WriteReg(0x0012,0x0139);  //��  0x139               
        Delay_ms(50); 
        LCD_WriteReg(0x0013,0x1700);   
        LCD_WriteReg(0x0029,0x000c); //��  0x000c
        LCD_WriteReg(0x002b,0x000c);
        Delay_ms(50); 
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);           
		Delay_ms(50); 
		//٤��У��
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
        LCD_WriteReg(0x0050,0x0000); //ˮƽGRAM��ʼλ�� 
        LCD_WriteReg(0x0051,0x00ef); //ˮƽGRAM��ֹλ��                    
        LCD_WriteReg(0x0052,0x0000); //��ֱGRAM��ʼλ��                    
        LCD_WriteReg(0x0053,0x013f); //��ֱGRAM��ֹλ��  
        
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
        //������ʾ����    
        LCD_WriteReg(0x0007,0x0133);   
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);

	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
      Delay_ms(20);                     //���ݲ�ͬ�����ٶȿ��Ե�����ʱ�������ȶ���ʾ 
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

//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
  LCD_WriteReg(0x20, Ypos);
  LCD_WriteReg(0x21, 319-Xpos);
}
/****************************************************************************
* ��    �ƣ�u16 LCD_BGR2RGB(u16 c)
* ��    �ܣ�RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
* ��ڲ�����c      BRG ��ɫֵ
* ���ڲ�����RGB ��ɫֵ
* ˵    �����ڲ���������
* ���÷�����
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
* ��    �ƣ�u16 LCD_GetPoint(u16 x,u16 y)
* ��    �ܣ���ȡָ���������ɫֵ
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������ǰ������ɫֵ
* ˵    ����
* ���÷�����i=LCD_GetPoint(10,10);
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

  /****����ʱ��****/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOE, ENABLE); 	

  //LCD Reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	 //ע��һ��Ҫ�趨ΪOUT_PPģʽ������ΪAF_PPģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/****�˿ڸ���****/
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

  //FSMC��GPIOD�ܽ�
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

  //FSMC��GPIOE�ܽ�
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

  //  //�������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB,GPIO_Pin_15);//����

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

  /****����ʱ��****/
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
  p.FSMC_AddressSetupTime = 3;//4;          //ADDSET ��ַ����ʱ��
  p.FSMC_AddressHoldTime = 8; //8          //ADDHOLD ��ַ����ʱ��
  p.FSMC_DataSetupTime = 10;//8;	           //DATAST ���ݽ���ʱ��
  p.FSMC_BusTurnAroundDuration = 0;	   //BUSTURN ���߷�תʱ��
  p.FSMC_CLKDivision = 0;			   //CLKDIV ʱ�ӷ�Ƶ
  p.FSMC_DataLatency = 0;			   //DATLAT ���ݱ���ʱ��
  p.FSMC_AccessMode = FSMC_AccessMode_A;   //ACCMOD FSMC ����ģʽ
    																					   
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;							   //ѡ�����õ�BANK��Ƭѡ�ź�
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;			   //�����Ƿ����ݵ�ַ���߷�ʱ����
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;						   //���ô洢������
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;			   //�������ݿ��
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;	       //�����Ƿ�ʹ�ñŷ�����ģʽ(Ӧ�þ���������дģʽ��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	       //����WAIT�źŵ���Ч��ƽ
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;					       //�����Ƿ�ʹ�û���ģʽ 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //����WAIT�ź���Чʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			   //�趨�Ƿ�ʹ��д����
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;					   //�趨�Ƿ�ʹ��WAIT�ź�
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;			 	   //�趨�Ƿ�ʹ�õ�����дʱ��
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	       //�趨�Ƿ�ʹ���첽�ȴ��ź�
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;					   //�趨�Ƿ�ʹ�ñŷ�дģʽ
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;							   //�趨��дʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* Enable FSMC_Bank1_NORSRAM1 */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

}

//===============================================================
//���ô���
//��ڲ���: x0 y0,Ϊ��ʾ�������Ͻ����ꣻx1,y1Ϊ��ʾ�������½����ꡣ
//���ڲ���: ��
//˵�����ú�������TFTģ�����ʾ���ڡ�
void LCD_setwindow(unsigned char x0,unsigned int y0,unsigned char x1,unsigned int y1)
{
	LCD_WriteReg(0x0050,x0);//ˮƽ��ʼλ��
	LCD_WriteReg(0x0051,x1);//ˮƽ��ֹλ��
	LCD_WriteReg(0x0052,y0);//��ֱ��ʼλ��
	LCD_WriteReg(0x0053,y1);//��ֱ��ֹλ��
	LCD_WriteReg(0x0020,x0);//����X����λ��
	LCD_WriteReg(0x0021,y0);//����Y����λ��
	LCD_WriteRAM_Prepare();
}


