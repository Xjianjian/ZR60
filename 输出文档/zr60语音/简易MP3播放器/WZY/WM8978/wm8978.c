#include   "wm8978.h"
#include   "iic.h"
#include   "iis.h"
/***************************************************************************************
通过IIS接口传送音频数据
I2S_LRCK-------PB12 数据左右对齐时钟 LRCK=fs WS/SPI2_NSS
I2S_SCLK-------PB13 位时钟用于同步   BCLK       SPI2_SCK
I2S_SDOUT------PC2  I2S2ext_SD                  SPI2_MISO
I2S_SDIN-------PC3                              SPI2_MOSI
I2S_MCLK-------PC6  MCLK=fs*256              I2S2_MCK
支持5种不同的音频数据传输格式
MSB、LSB、飞利浦、DSP_A、DSP_B
通过IIC接口进行配置(MODE=0)
IIC_SCL--------PB8
IIC_SDA--------PB9
WM8978IIC与24C02和MPU6050共用
***************************************************************************************/


//WM8978寄存器值缓冲区(0-57)，占用116个字节内存
//WM8978的IIC不支持读操作，在本地缓存所有寄存器值
//写寄存器时，同步更新此缓冲区，读寄存器时直接返回此缓冲区
//寄存器地址是7bit， 寄存器数据是9bit
//WM8978寄存器是9位的，需要用u16来存放
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
//返回0：成功
//其他：错误
uint8_t WM8978_Write_Reg(uint8_t reg,uint16_t val)
{
   IIC_Start();
   //IIC器件地址为7位，而IIC_SendByte函数先发送高位数据，所以需要先移位
   IIC_SendByte((WM8978_ADDR<<1)|0);//器件地址+写命令
   if(IIC_WaitACK())return 1;//等待应答
   IIC_SendByte((reg<<1)|((val>>8)&0x01));//写寄存器地址+数据最高位
                                          //数据长度为9bits
   if(IIC_WaitACK())return 2;//等待应答
   IIC_SendByte(val&0xFF);//发送数据低8位
   if(IIC_WaitACK())return 3;
   IIC_Stop();
   WM8978_REGVAL_TBL[reg]=val;//更新本地寄存器值
   return 0;
}
//WM8978读寄存器
//就是读取本地寄存器值缓冲区内的对应值
//reg:寄存器地址 
//返回值:寄存器值
uint16_t WM8978_Read_Reg(uint8_t reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
//WM8978初始化
//返回值:0,初始化正常
//    其他,错误代码
uint8_t  WM8978_Init(void)
{
	uint8_t res;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);
	//PB12 PB13推挽复用功能输出
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
	res=WM8978_Write_Reg(WM8978_R0,0);//软复位WM8978
	if(res)return 1;//发送指令失败，WM8978异常
	//以下为通用设置
	WM8978_Write_Reg(WM8978_R1,0X1B);	//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
	WM8978_Write_Reg(WM8978_R2,0X1B0);	//R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	WM8978_Write_Reg(WM8978_R3,0X6C);	//R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能	
	WM8978_Write_Reg(WM8978_R6,0);		//R6,MCLK由外部提供
	WM8978_Write_Reg(WM8978_R43,1<<4);	//R43,INVROUT2反向,驱动喇叭
	WM8978_Write_Reg(WM8978_R47,1<<8);	//R47设置,PGABOOSTL,左通道MIC获得20倍增益
	WM8978_Write_Reg(WM8978_R48,1<<8);	//R48设置,PGABOOSTR,右通道MIC获得20倍增益
	WM8978_Write_Reg(WM8978_R49,1<<1);	//R49,TSDEN,开启过热保护 
	WM8978_Write_Reg(WM8978_R10,1<<3);	//R10,SOFTMUTE关闭,128x采样,最佳SNR 
	WM8978_Write_Reg(WM8978_R14,1<<3);	//R14,ADC 128x采样率
	return 0;
}

//WM8978 DAC/ADC配置
//adcen:adc使能(1)/关闭(0)
//dacen:dac使能(1)/关闭(0)
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen)
{
   uint16_t regval;
   regval=WM8978_Read_Reg(WM8978_R3);//读取R3
   if(dacen)regval|=3<<0;    //R3最低2个位设置为1,开启DACR&DACL
   else     regval&=~(3<<0); //R3最低2个位清零,关闭DACR&DACL
   WM8978_Write_Reg(WM8978_R3,regval);//设置R3
   regval=WM8978_Read_Reg(WM8978_R2);//读取R2
   if(adcen)regval|=3<<0;		//R2最低2个位设置为1,开启ADCR&ADCL
   else regval&=~(3<<0);		//R2最低2个位清零,关闭ADCR&ADCL.
   WM8978_Write_Reg(WM8978_R2,regval);//设置R2	
}
//WM8978 输入通道配置 
//micen:MIC开启(1)/关闭(0)
//lineinen:Line In开启(1)/关闭(0)
//auxen:aux开启(1)/关闭(0) 
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
	uint16_t regval;  
	regval=WM8978_Read_Reg(WM8978_R2);	//读取R2
	if(micen)regval|=3<<2;		//开启INPPGAENR,INPPGAENL(MIC的PGA放大)
	else regval&=~(3<<2);		//关闭INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(WM8978_R2,regval);	//设置R2 
	
	regval=WM8978_Read_Reg(WM8978_R44);	//读取R44
	if(micen)regval|=3<<4|3<<0;	//开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(WM8978_R44,regval);//设置R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);//LINE IN 0dB增益
	else WM8978_LINEIN_Gain(0);	//关闭LINE IN
	if(auxen)WM8978_AUX_Gain(7);//AUX 6dB增益
	else WM8978_AUX_Gain(0);	//关闭AUX输入  
}
//WM8978 输出配置 
//dacen:DAC输出(放音)开启(1)/关闭(0)
//bpsen:Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0) 
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen)
{
	uint16_t regval=0;
	if(dacen)regval|=1<<0;	//DAC输出使能
	if(bpsen)
	{
		regval|=1<<1;		//BYPASS使能
		regval|=5<<2;		//0dB增益
	} 
	WM8978_Write_Reg(WM8978_R50,regval);//R50设置
	WM8978_Write_Reg(WM8978_R51,regval);//R51设置 
}
//WM8978 MIC增益设置(不包括BOOST的20dB,MIC-->ADC输入部分的增益)
//gain:0~63,对应-12dB~35.25dB,0.75dB/Step
void WM8978_MIC_Gain(uint8_t gain)
{
	gain&=0X3F;
	WM8978_Write_Reg(WM8978_R45,gain);		//R45,左通道PGA设置 
	WM8978_Write_Reg(WM8978_R46,gain|1<<8);	//R46,右通道PGA设置
}
//WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(WM8978_R47);	//读取R47
	regval&=~(7<<4);			//清除原来的设置 
 	WM8978_Write_Reg(WM8978_R47,regval|gain<<4);//设置R47
	regval=WM8978_Read_Reg(WM8978_R48);	//读取R48
	regval&=~(7<<4);			//清除原来的设置 
 	WM8978_Write_Reg(WM8978_R48,regval|gain<<4);//设置R48
} 
//WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(WM8978_R47);	//读取R47
	regval&=~(7<<0);			//清除原来的设置 
 	WM8978_Write_Reg(WM8978_R47,regval|gain<<0);//设置R47
	regval=WM8978_Read_Reg(WM8978_R48);	//读取R48
	regval&=~(7<<0);			//清除原来的设置 
 	WM8978_Write_Reg(WM8978_R48,regval|gain<<0);//设置R48
}  
//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位;1,20位;2,24位;3,32位;  
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len)
{
	fmt&=0X03;
	len&=0X03;//限定范围
	WM8978_Write_Reg(WM8978_R4,(fmt<<3)|(len<<5));	//R4,WM8978工作模式设置	
}	

//设置耳机左右声道音量
//voll:左声道音量(0~63)
//volr:右声道音量(0~63)
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr)
{
	voll&=0X3F;
	volr&=0X3F;//限定范围
	if(voll==0)voll|=1<<6;//音量为0时,直接mute
	if(volr==0)volr|=1<<6;//音量为0时,直接mute 
	WM8978_Write_Reg(WM8978_R52,voll);			//R52,耳机左声道音量设置
	WM8978_Write_Reg(WM8978_R53,volr|(1<<8));	//R53,耳机右声道音量设置,同步更新(HPVU=1)
}
//设置喇叭音量
//voll:左声道音量(0~63) 
void WM8978_SPKvol_Set(uint8_t volx)
{ 
	volx&=0X3F;//限定范围
	if(volx==0)volx|=1<<6;//音量为0时,直接mute 
 	WM8978_Write_Reg(WM8978_R54,volx);			//R54,喇叭左声道音量设置
	WM8978_Write_Reg(WM8978_R55,volx|(1<<8));	//R55,喇叭右声道音量设置,同步更新(SPKVU=1)	
}
//设置3D环绕声
//depth:0~15(3D强度,0最弱,15最强)
void WM8978_3D_Set(uint8_t depth)
{ 
	depth&=0XF;//限定范围 
 	WM8978_Write_Reg(WM8978_R41,depth);	//R41,3D环绕设置 	
}
//设置EQ/3D作用方向
//dir:0,在ADC起作用
//    1,在DAC起作用(默认)
void WM8978_EQ_3D_Dir(uint8_t dir)
{
	uint16_t regval; 
	regval=WM8978_Read_Reg(WM8978_R18);
	if(dir)regval|=1<<8;
	else regval&=~(1<<8); 
 	WM8978_Write_Reg(WM8978_R18,regval);//R18,EQ1的第9位控制EQ/3D方向
}

//设置EQ1
//cfreq:截止频率,0~3,分别对应:80/105/135/175Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ1_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval;
	cfreq&=0X3;//限定范围 
	if(gain>24)gain=24;
	gain=24-gain;
	regval=WM8978_Read_Reg(WM8978_R18);
	regval&=0X100;
	regval|=cfreq<<5;	//设置截止频率 
	regval|=gain;		//设置增益	
 	WM8978_Write_Reg(WM8978_R18,regval);//R18,EQ1设置 	
}
//设置EQ2
//cfreq:中心频率,0~3,分别对应:230/300/385/500Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ2_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//限定范围 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//设置截止频率 
	regval|=gain;		//设置增益	
 	WM8978_Write_Reg(WM8978_R19,regval);//R19,EQ2设置 	
}
//设置EQ3
//cfreq:中心频率,0~3,分别对应:650/850/1100/1400Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ3_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//限定范围 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//设置截止频率 
	regval|=gain;		//设置增益	
 	WM8978_Write_Reg(WM8978_R20,regval);//R20,EQ3设置 	
}
//设置EQ4
//cfreq:中心频率,0~3,分别对应:1800/2400/3200/4100Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ4_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//限定范围 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//设置截止频率 
	regval|=gain;		//设置增益	
 	WM8978_Write_Reg(WM8978_R21,regval);//R21,EQ4设置 	
}
//设置EQ5
//cfreq:中心频率,0~3,分别对应:5300/6900/9000/11700Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ5_Set(uint8_t cfreq,uint8_t gain)
{ 
	uint16_t regval=0;
	cfreq&=0X3;//限定范围 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//设置截止频率 
	regval|=gain;		//设置增益	
 	WM8978_Write_Reg(WM8978_R22,regval);//R22,EQ5设置 	
}
