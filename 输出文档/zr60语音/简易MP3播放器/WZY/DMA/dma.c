#include   "dma.h"
#include   "iis.h"
#include   "button.h"
//ADC1 DMA通道设置
void  DMA2_Stream0_Channel0_Init(void)
{
   DMA_InitTypeDef DMA_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
   DMA_InitStructure.DMA_Channel=DMA_Channel_0;
   DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)ADC1_DR_ADDRESS;
   //ADCConvertedValue[0]存放ADC1通道4转换结果
   DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)&ADCConvertedValue;	
   DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;
   DMA_InitStructure.DMA_BufferSize=2;
   DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
   DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;	
   DMA_InitStructure.DMA_Priority=DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;
   DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOThreshold_HalfFull;
   DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
   DMA_Init(DMA2_Stream0,&DMA_InitStructure);
   DMA_Cmd(DMA2_Stream0,ENABLE);   	
}
//I2S2 TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void  DMA1_Stream4_Channel0_Init(uint8_t* buf0,uint8_t* buf1,uint16_t num)
{
   DMA_InitTypeDef DMA_InitStructure;
	
   NVIC_InitTypeDef NVIC_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
 
   DMA_DeInit(DMA1_Stream4);
   while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//等待DMA1_Stream4可配置 
   DMA_InitStructure.DMA_Channel=DMA_Channel_0;
   DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&SPI2->DR;
   DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)buf0;	   
   DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral;
   DMA_InitStructure.DMA_BufferSize=num;//数据传输量
   DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
   DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;	
   DMA_InitStructure.DMA_Priority=DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;
   DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
   DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
   DMA_Init(DMA1_Stream4,&DMA_InitStructure);
	   
   DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);//双缓冲模式配置	   
   DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);//启用双缓冲模式
   DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);//开启DMA1_Stream4传输完成中断

   NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream4_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
   NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStructure);  

}


void  DMA1_Stream4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)//传输完成中断
	{
	   DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
		
	   I2S_TX_CallBack();//回调函数，读取数据等操作在这处理
	}  
}


