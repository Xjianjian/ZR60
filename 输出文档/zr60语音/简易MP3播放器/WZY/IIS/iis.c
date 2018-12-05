#include   "iis.h"


//MCK���ʹ��ʱfs���㹫ʽ���£�
//fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//fs=(1000*PLLI2SN/PLLI2SR)/[256*(2*I2SDIV+ODD)] ��λ��KHz
//PLLI2SNȡֵ��192-432
//PLLI2SRȡֵ��2-7
//I2SDIVȡֵ��2-255
//ODDȡֵ��0/1
//���ʽ:������/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz������
	{1102,429,4,19,0},		//11.025Khz������ 
	{1600,213,2,13,0},		//16Khz������
	{2205,429,4, 9,1},		//22.05Khz������
	{3200,213,2, 6,1},		//32Khz������
	{4410,271,2, 6,0},		//44.1Khz������
	{4800,258,3, 3,1},		//48Khz������
	{8820,316,2, 3,1},		//88.2Khz������
	{9600,344,2, 3,1},  	//96Khz������
	{17640,361,2,2,0},  	//176.4Khz������ 
	{19200,393,2,2,0},  	//192Khz������
};  
//��Ҫ����ODD��DIV�Լ�ʹ��MCK���
uint8_t I2S2_SamplingRate_Set(uint32_t samplingrate)
{
	uint8_t i;
	uint32_t temp=0;
	samplingrate/=10;
    for(i=0;i<sizeof(I2S_PSC_TBL)/10;i++)//�ж��Ƿ�֧�ִ���Ĳ�����
    {
	    if(samplingrate==I2S_PSC_TBL[i][0])break;
	 }
	RCC_PLLI2SCmd(DISABLE);//�ر�I2S
	if(i==sizeof(I2S_PSC_TBL)/10)return 1;//��֧�ָò�����
	RCC_PLLI2SConfig(I2S_PSC_TBL[i][1],I2S_PSC_TBL[i][2]);
	
	RCC->CR|=1<<26;//����I2Sʱ��
	while((RCC->CR&1<<27)==0);//�ȴ�I2Sʱ�ӿ����ɹ�
	 
	temp=I2S_PSC_TBL[i][3]<<0;//I2SPR�Ĵ����ĵ�0-7λΪI2SDIV
    temp|=I2S_PSC_TBL[i][4]<<8;//����ODDλ	 
	temp|=1<<9;//ʹ��MCK 
	SPI2->I2SPR=temp;//����SPI2->I2SPR�Ĵ���
	return 0;	 	 
}
//I2S2��ʼ��

//����I2S_Standard:  @ref SPI_I2S_Standard  I2S��׼,
//I2S_Standard_Phillips,�����ֱ�׼;
//I2S_Standard_MSB,MSB�����׼(�Ҷ���);
//I2S_Standard_LSB,LSB�����׼(�����);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM��׼
//����I2S_Mode:  @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:�ӻ�����;I2S_Mode_SlaveRx:�ӻ�����;I2S_Mode_MasterTx:��������;I2S_Mode_MasterRx:��������;
//����I2S_Clock_Polarity   @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,ʱ�ӵ͵�ƽ��Ч;I2S_CPOL_High,ʱ�Ӹߵ�ƽ��Ч
//����I2S_DataFormat�� @ref SPI_I2S_Data_Format :���ݳ���,I2S_DataFormat_16b,16λ��׼;I2S_DataFormat_16bextended,16λ��չ(frame=32bit);I2S_DataFormat_24b,24λ;I2S_DataFormat_32b,32λ.
void I2S2_Init(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
    I2S_InitTypeDef I2S_InitStructure;
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);//ʹ��SPI2
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
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2Sʹ��
}
void  (*I2S_TX_CallBack)(void);//I2S_TX�ص�����
void  I2S_Play_Start(void)
{
   DMA_Cmd(DMA1_Stream4,ENABLE);
}
void  I2S_Play_Stop(void)
{
   DMA_Cmd(DMA1_Stream4,DISABLE);
}
