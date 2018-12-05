#include   "dma.h"
#include   "iis.h"
#include   "button.h"
//ADC1 DMAͨ������
void  DMA2_Stream0_Channel0_Init(void)
{
   DMA_InitTypeDef DMA_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
   DMA_InitStructure.DMA_Channel=DMA_Channel_0;
   DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)ADC1_DR_ADDRESS;
   //ADCConvertedValue[0]���ADC1ͨ��4ת�����
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
//I2S2 TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
void  DMA1_Stream4_Channel0_Init(uint8_t* buf0,uint8_t* buf1,uint16_t num)
{
   DMA_InitTypeDef DMA_InitStructure;
	
   NVIC_InitTypeDef NVIC_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
 
   DMA_DeInit(DMA1_Stream4);
   while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//�ȴ�DMA1_Stream4������ 
   DMA_InitStructure.DMA_Channel=DMA_Channel_0;
   DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&SPI2->DR;
   DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)buf0;	   
   DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral;
   DMA_InitStructure.DMA_BufferSize=num;//���ݴ�����
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
	   
   DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);//˫����ģʽ����	   
   DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);//����˫����ģʽ
   DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);//����DMA1_Stream4��������ж�

   NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream4_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
   NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStructure);  

}


void  DMA1_Stream4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)//��������ж�
	{
	   DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
		
	   I2S_TX_CallBack();//�ص���������ȡ���ݵȲ������⴦��
	}  
}


