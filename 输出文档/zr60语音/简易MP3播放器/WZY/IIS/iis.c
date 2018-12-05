#include   "iis.h"


//MCK输出使能时fs计算公式如下：
//fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//fs=(1000*PLLI2SN/PLLI2SR)/[256*(2*I2SDIV+ODD)] 单位：KHz
//PLLI2SN取值：192-432
//PLLI2SR取值：2-7
//I2SDIV取值：2-255
//ODD取值：0/1
//表格式:采样率/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz采样率
	{1102,429,4,19,0},		//11.025Khz采样率 
	{1600,213,2,13,0},		//16Khz采样率
	{2205,429,4, 9,1},		//22.05Khz采样率
	{3200,213,2, 6,1},		//32Khz采样率
	{4410,271,2, 6,0},		//44.1Khz采样率
	{4800,258,3, 3,1},		//48Khz采样率
	{8820,316,2, 3,1},		//88.2Khz采样率
	{9600,344,2, 3,1},  	//96Khz采样率
	{17640,361,2,2,0},  	//176.4Khz采样率 
	{19200,393,2,2,0},  	//192Khz采样率
};  
//主要配置ODD和DIV以及使能MCK输出
uint8_t I2S2_SamplingRate_Set(uint32_t samplingrate)
{
	uint8_t i;
	uint32_t temp=0;
	samplingrate/=10;
    for(i=0;i<sizeof(I2S_PSC_TBL)/10;i++)//判断是否支持传入的采样率
    {
	    if(samplingrate==I2S_PSC_TBL[i][0])break;
	 }
	RCC_PLLI2SCmd(DISABLE);//关闭I2S
	if(i==sizeof(I2S_PSC_TBL)/10)return 1;//不支持该采样率
	RCC_PLLI2SConfig(I2S_PSC_TBL[i][1],I2S_PSC_TBL[i][2]);
	
	RCC->CR|=1<<26;//开启I2S时钟
	while((RCC->CR&1<<27)==0);//等待I2S时钟开启成功
	 
	temp=I2S_PSC_TBL[i][3]<<0;//I2SPR寄存器的第0-7位为I2SDIV
    temp|=I2S_PSC_TBL[i][4]<<8;//设置ODD位	 
	temp|=1<<9;//使能MCK 
	SPI2->I2SPR=temp;//配置SPI2->I2SPR寄存器
	return 0;	 	 
}
//I2S2初始化

//参数I2S_Standard:  @ref SPI_I2S_Standard  I2S标准,
//I2S_Standard_Phillips,飞利浦标准;
//I2S_Standard_MSB,MSB对齐标准(右对齐);
//I2S_Standard_LSB,LSB对齐标准(左对齐);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM标准
//参数I2S_Mode:  @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:从机发送;I2S_Mode_SlaveRx:从机接收;I2S_Mode_MasterTx:主机发送;I2S_Mode_MasterRx:主机接收;
//参数I2S_Clock_Polarity   @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,时钟低电平有效;I2S_CPOL_High,时钟高电平有效
//参数I2S_DataFormat： @ref SPI_I2S_Data_Format :数据长度,I2S_DataFormat_16b,16位标准;I2S_DataFormat_16bextended,16位扩展(frame=32bit);I2S_DataFormat_24b,24位;I2S_DataFormat_32b,32位.
void I2S2_Init(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
    I2S_InitTypeDef I2S_InitStructure;
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);//使能SPI2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);
	
	I2S_InitStructure.I2S_Mode=I2S_Mode;
	I2S_InitStructure.I2S_Standard=I2S_Standard;
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;
	I2S_Init(SPI2,&I2S_InitStructure);
	
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2S使能
}
void  (*I2S_TX_CallBack)(void);//I2S_TX回调函数
void  I2S_Play_Start(void)
{
   DMA_Cmd(DMA1_Stream4,ENABLE);
}
void  I2S_Play_Stop(void)
{
   DMA_Cmd(DMA1_Stream4,DISABLE);
}
