#include   "wm8978.h"
#include   "iic.h"
#include   "iis.h"
/***************************************************************************************
ͨ��IIS�ӿڴ�����Ƶ����
I2S_LRCK-------PB12 �������Ҷ���ʱ�� LRCK=fs WS/SPI2_NSS
I2S_SCLK-------PB13 λʱ������ͬ��   BCLK       SPI2_SCK
I2S_SDOUT------PC2  I2S2ext_SD                  SPI2_MISO
I2S_SDIN-------PC3                              SPI2_MOSI
I2S_MCLK-------PC6  MCLK=fs*256              I2S2_MCK
֧��5�ֲ�ͬ����Ƶ���ݴ����ʽ
MSB��LSB�������֡�DSP_A��DSP_B
ͨ��IIC�ӿڽ�������(MODE=0)
IIC_SCL--------PB8
IIC_SDA--------PB9
WM8978IIC��24C02��MPU6050����
***************************************************************************************/


//WM8978�Ĵ���ֵ������(0-57)��ռ��116���ֽ��ڴ�
//WM8978��IIC��֧�ֶ��������ڱ��ػ������мĴ���ֵ
//д�Ĵ���ʱ��ͬ�����´˻����������Ĵ���ʱֱ�ӷ��ش˻�����
//�Ĵ�����ַ��7bit�� �Ĵ���������9bit
//WM8978�Ĵ�����9λ�ģ���Ҫ��u16�����
static  uint16_t   WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
};
//����0���ɹ�
//����������
uint8_t WM8978_Write_Reg(uint8_t reg,uint16_t val)
{
   IIC_Start();
   //IIC������ַΪ7λ����IIC_SendByte�����ȷ��͸�λ���ݣ�������Ҫ����λ
   IIC_SendByte((WM8978_ADDR<<1)|0);//������ַ+д����
   if(IIC_WaitACK())return 1;//�ȴ�Ӧ��
   IIC_SendByte((reg<<1)|((val>>8)&0x01));//д�Ĵ�����ַ+�������λ
                                          //���ݳ���Ϊ9bits
   if(IIC_WaitACK())return 2;//�ȴ�Ӧ��
   IIC_SendByte(val&0xFF);//�������ݵ�8λ
   if(IIC_WaitACK())return 3;
   IIC_Stop();
   WM8978_REGVAL_TBL[reg]=val;//���±��ؼĴ���ֵ
   return 0;
}
//WM8978���Ĵ���
//���Ƕ�ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
//reg:�Ĵ�����ַ 
//����ֵ:�Ĵ���ֵ
uint16_t WM8978_Read_Reg(uint8_t reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
//WM8978��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
uint8_t  WM8978_Init(void)
{
	uint8_t res;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);
	//PB12 PB13���츴�ù������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);
	
	IIC_Init();
	res=WM8978_Write_Reg(WM8978_R0,0);//��λWM8978
	if(res)return 1;//����ָ��ʧ�ܣ�WM8978�쳣
	//����Ϊͨ������
	WM8978_Write_Reg(WM8978_R1,0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write_Reg(WM8978_R2,0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write_Reg(WM8978_R3,0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
	WM8978_Write_Reg(WM8978_R6,0);		//R6,MCLK���ⲿ�ṩ
	WM8978_Write_Reg(WM8978_R43,1<<4);	//R43,INVROUT2����,��������
	WM8978_Write_Reg(WM8978_R47,1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write_Reg(WM8978_R48,1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	WM8978_Write_Reg(WM8978_R49,1<<1);	//R49,TSDEN,�������ȱ��� 
	WM8978_Write_Reg(WM8978_R10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write_Reg(WM8978_R14,1<<3);	//R14,ADC 128x������
	return 0;
}

//WM8978 DAC/ADC����
//adcen:adcʹ��(1)/�ر�(0)
//dacen:dacʹ��(1)/�ر�(0)
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen)
{
   uint16_t regval;
   regval=WM8978_Read_Reg(WM8978_R3);//��ȡR3
   if(dacen)regval|=3<<0;    //R3���2��λ����Ϊ1,����DACR&DACL
   else     regval&=~(3<<0); //R3���2��λ����,�ر�DACR&DACL
   WM8978_Write_Reg(WM8978_R3,regval);//����R3
   regval=WM8978_Read_Reg(WM8978_R2);//��ȡR2
   if(adcen)regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
   else regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
   WM8978_Write_Reg(WM8978_R2,regval);//����R2	
}
//WM8978 ����ͨ������ 
//micen:MIC����(1)/�ر�(0)
//lineinen:Line In����(1)/�ر�(0)
//auxen:aux����(1)/�ر�(0) 
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
	uint16_t regval;  
	regval=WM8978_Read_Reg(WM8978_R2);	//��ȡR2
	if(micen)regval|=3<<2;		//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else regval&=~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(WM8978_R2,regval);	//����R2 
	
	regval=WM8978_Read_Reg(WM8978_R44);	//��ȡR44
	if(micen)regval|=3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(WM8978_R44,regval);//����R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);//LINE IN 0dB����
	else WM8978_LINEIN_Gain(0);	//�ر�LINE IN
	if(auxen)WM8978_AUX_Gain(7);//AUX 6dB����
	else WM8978_AUX_Gain(0);	//�ر�AUX����  
}
//WM8978 ������� 
//dacen:DAC���(����)����(1)/�ر�(0)
//bpsen:Bypass���(¼��,����MIC,LINE IN,AUX��)����(1)/�ر�(0) 
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen)
{
	uint16_t regval=0;
	if(dacen)regval|=1<<0;	//DAC���ʹ��
	if(bpsen)
	{
		regval|=1<<1;		//BYPASSʹ��
		regval|=5<<2;		//0dB����
	} 
	WM8978_Write_Reg(WM8978_R50,regval);//R50����
	WM8978_Write_Reg(WM8978_R51,regval);//R51���� 
}
//WM8978 MIC��������(������BOOST��20dB,MIC-->ADC���벿�ֵ�����)
//gain:0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
void WM8978_MIC_Gain(uint8_t gain)
{
	gain&=0X3F;
	WM8978_Write_Reg(WM8978_R45,gain);		//R45,��ͨ��PGA���� 
	WM8978_Write_Reg(WM8978_R46,gain|1<<8);	//R46,��ͨ��PGA����
}
//WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(WM8978_R47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_R47,regval|gain<<4);//����R47
	regval=WM8978_Read_Reg(WM8978_R48);	//��ȡR48
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_R48,regval|gain<<4);//����R48
} 
//WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(WM8978_R47);	//��ȡR47
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_R47,regval|gain<<0);//����R47
	regval=WM8978_Read_Reg(WM8978_R48);	//��ȡR48
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_R48,regval|gain<<0);//����R48
}  
//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;  
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	WM8978_Write_Reg(WM8978_R4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}	

//���ö���������������
//voll:����������(0~63)
//volr:����������(0~63)
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write_Reg(WM8978_R52,voll);			//R52,������������������
	WM8978_Write_Reg(WM8978_R53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}
//������������
//voll:����������(0~63) 
void WM8978_SPKvol_Set(uint8_t volx)
{ 
	volx&=0X3F;//�޶���Χ
	if(volx==0)volx|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	WM8978_Write_Reg(WM8978_R54,volx);			//R54,������������������
	WM8978_Write_Reg(WM8978_R55,volx|(1<<8));	//R55,������������������,ͬ������(SPKVU=1)	
}
//����3D������
//depth:0~15(3Dǿ��,0����,15��ǿ)
void WM8978_3D_Set(uint8_t depth)
{ 
	depth&=0XF;//�޶���Χ 
 	WM8978_Write_Reg(WM8978_R41,depth);	//R41,3D�������� 	
}
//����EQ/3D���÷���
//dir:0,��ADC������
//    1,��DAC������(Ĭ��)
void WM8978_EQ_3D_Dir(uint8_t dir)
{
	uint16_t regval; 
	regval=WM8978_Read_Reg(WM8978_R18);
	if(dir)regval|=1<<8;
	else regval&=~(1<<8); 
 	WM8978_Write_Reg(WM8978_R18,regval);//R18,EQ1�ĵ�9λ����EQ/3D����
}

//����EQ1
//cfreq:��ֹƵ��,0~3,�ֱ��Ӧ:80/105/135/175Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ1_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain;
	regval=WM8978_Read_Reg(WM8978_R18);
	regval&=0X100;
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_R18,regval);//R18,EQ1���� 	
}
//����EQ2
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:230/300/385/500Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ2_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_R19,regval);//R19,EQ2���� 	
}
//����EQ3
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:650/850/1100/1400Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ3_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_R20,regval);//R20,EQ3���� 	
}
//����EQ4
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:1800/2400/3200/4100Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ4_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_R21,regval);//R21,EQ4���� 	
}
//����EQ5
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:5300/6900/9000/11700Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ5_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_R22,regval);//R22,EQ5���� 	
}
