#include "TouuchDriver.h"
void Touch_Pin_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void Touch_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  Touch_Pin_Init();    
  /*!< Deselect the FLASH: Chip Select high */
  TOUCH_CS_HIGH();
  /*!< SPI configuration */
 /* SPI configuration *///初始化SPI结构体
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI1设置为两线全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //设置SPI1为主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //串行时钟在不操作时，时钟为低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //第一个时钟沿开始采样数据
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS信号由软件（使用SSI位）管理
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //SPI波特率预分频值为8
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC值计算的多项式
  SPI_Init(SPI1, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(SPI1, ENABLE);
}
u8 Touch_SendReadByte(u8 byte)
{
 unsigned char Data = 0; 

   //Wait until the transmit buffer is empty 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
  // Send the byte  
  SPI_I2S_SendData(SPI1,byte); 

   //Wait until a data is received 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
  // Get the received data 
  Data = SPI_I2S_ReceiveData(SPI1); 

  // Return the shifted data 
  return Data; 
}
/*=====================================================================*/
u16 Touch_TPReadX(void)
{ 
   u16 x=0;
   TOUCH_CS_LOW();
   Touch_SendReadByte(CHX);
   x=Touch_SendReadByte(0x00);
   x<<=8;
   x+=Touch_SendReadByte(0x00);
   TOUCH_CS_HIGH(); 
	x=(x-TOUCHTOP)/77.8;
   return (x);
}

/*=====================================================================*/
u16 Touch_TPReadY(void)
{
  u16 y=0;
  TOUCH_CS_LOW();
  Touch_SendReadByte(CHY);
  y=Touch_SendReadByte(0x00);
  y<<=8;
  y+=Touch_SendReadByte(0x00);
  TOUCH_CS_HIGH();
	y=(y-TOUCHTOP)/106.25;
  return (y);
}

