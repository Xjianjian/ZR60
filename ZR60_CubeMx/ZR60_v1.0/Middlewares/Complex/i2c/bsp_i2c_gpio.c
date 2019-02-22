/**
  ******************************************************************************
  * @file    bsp_i2c_gpio.c
  * @version V1.0
  * @author  liujian
  * @date    2018-06-26
  * @brief   用gpio模拟i2c总线, 适用于STM32系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
  ******************************************************************************
  * @attention
  *
  * 
  * 
  * 
  *
  ******************************************************************************
  */ 

/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO

*/
#include "./i2c/bsp_i2c_gpio.h"
#include "stm32f4xx_i2c.h"


const bsp_iic_confStruct   iicInfo[I2C_DEVICE_NUM] = 
{
	{I2C_HYM8563_ADDRESS,I2C1,I2C_HYM8563_Speed,RCC_APB1Periph_I2C1,RCC_AHB1Periph_GPIOB, \
		GPIOB,GPIO_PinSource8,GPIO_PinSource9,GPIO_AF_I2C1,GPIO_Pin_8,GPIO_Pin_9},
	{I2C_HYM8563_ADDRESS,I2C1,I2C_HYM8563_Speed,RCC_APB1Periph_I2C1,RCC_AHB1Periph_GPIOB, \
		GPIOB,GPIO_PinSource8,GPIO_PinSource9,GPIO_AF_I2C1,GPIO_Pin_8,GPIO_Pin_9},
	{I2C_HYM8563_ADDRESS,I2C1,I2C_HYM8563_Speed,RCC_APB1Periph_I2C1,RCC_AHB1Periph_GPIOB, \
		GPIOB,GPIO_PinSource8,GPIO_PinSource9,GPIO_AF_I2C1,GPIO_Pin_8,GPIO_Pin_9},
	{I2C_HYM8563_ADDRESS,I2C1,I2C_HYM8563_Speed,RCC_APB1Periph_I2C1,RCC_AHB1Periph_GPIOB, \
		GPIOB,GPIO_PinSource8,GPIO_PinSource9,GPIO_AF_I2C1,GPIO_Pin_8,GPIO_Pin_9}
};

#ifdef 	I2C_HARDWARE   //定义硬件iic	
/*
*********************************************************************************************************
*	函 数 名: i2c_CfgGpio
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_CfgGpio(uint8_t Dev,uint8_t Le_u_workType)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(Dev < I2C_DEVICE_NUM)
	{
		if(I2C_TYPE_HARDWARE == Le_u_workType)
		{
			/*使能 I2C 外设时钟 */
			RCC_APB1PeriphClockCmd(iicInfo[Dev].RCC_APB1Periph, ENABLE);
			/*使能 I2C 引脚的 GPIO 时钟*/
			RCC_AHB1PeriphClockCmd(iicInfo[Dev].RCC_AHB1Periph, ENABLE);
			/* Reset I2Cx IP */
			RCC_APB1PeriphResetCmd(iicInfo[Dev].RCC_APB1Periph, ENABLE);
			/* Release reset signal of I2Cx IP */
			RCC_APB1PeriphResetCmd(iicInfo[Dev].RCC_APB1Periph, DISABLE);
			/* 连接引脚源 PXx 到 I2C_SCL*/
			GPIO_PinAFConfig(iicInfo[Dev].GPIOx, I2C_SCL_SOURCE,iicInfo[Dev].GPIO_AF);
			/* 连接引脚源 PXx 到 to I2C_SDA*/
			GPIO_PinAFConfig(iicInfo[Dev].GPIOx,I2C_SDA_SOURCE,iicInfo[Dev].GPIO_AF);
			/*配置 SCL SDA引脚 */
			GPIO_InitStructure.GPIO_Pin = iicInfo[Dev].SCL_pin | iicInfo[Dev].SDA_pin;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(iicInfo[Dev].GPIOx, &GPIO_InitStructure);
		}
		else
		{
			RCC_AHB1PeriphClockCmd(iicInfo[Dev].RCC_AHB1Periph, ENABLE);	/* 打开GPIO时钟 */
			GPIO_InitStructure.GPIO_Pin = iicInfo[Dev].SCL_pin | iicInfo[Dev].SDA_pin;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  	
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  	/* 开漏输出 */
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(iicInfo[Dev].GPIOx, &GPIO_InitStructure);
			/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
			i2c_Stop();
		}
	}
}

/**
* @brief I2C 工作模式配置
* @param 无
* @retval 无
*/
void I2C_Mode_Config(uint8_t Dev)
{
	I2C_InitTypeDef I2C_InitStructure;
	/* I2C 配置 */
	I2C_DeInit(iicInfo[Dev].I2Cx);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	/*占空比*/
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	/*I2C 自身地址*/
	I2C_InitStructure.I2C_OwnAddress1 =I2C_OWN_ADDRESS7;
	/*使能响应*/
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	/* I2C 的寻址模式 */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	/* 通信速率 */
	I2C_InitStructure.I2C_ClockSpeed = iicInfo[Dev].I2C_ClkSpeed;
	/*写入配置*/
	I2C_Init(iicInfo[Dev].I2Cx, &I2C_InitStructure);
	/* 使能 I2C */
	I2C_Cmd(iicInfo[Dev].I2Cx, ENABLE);
}


/**
* @brief 将缓冲区中的数据写到 I2C 中，采用单字节写入的方式，速度比页写入慢
* @param Dev:设备索引
* @param pBuffer:缓冲区指针
* @param RegWrAddr:写地址(设备数据寄存器地址，区别设备地址)
* @param NumByteToWrite:写的字节数
* @retval 无
*/
uint8_t I2C_BytesWrite(uint8_t Dev,uint8_t RegWrAddr,uint8_t* pBuffer,uint8_t NumByteToWrite)
{
	uint8_t Le_u_i;
	uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

	
	if((0U != NumByteToWrite) && (NumByteToWrite <= 0xff))
	{
		while (I2C_GetFlagStatus(iicInfo[Dev].I2Cx, I2C_FLAG_BUSY))
		{
			if ((I2CTimeout--) == 0)
			{
				return 0;
			}
		}
		/* 产生 I2C 起始信号 */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		/*设置超时等待时间*/
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV5 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* 发送设备写操作指令 */
		I2C_Send7bitAddress(iicInfo[Dev].I2Cx, iicInfo[Dev].DevAddr|I2C_WR,I2C_Direction_Transmitter);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV6 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* 发送要写入的寄存器起始地址 */
		I2C_SendData(iicInfo[Dev].I2Cx, RegWrAddr);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV8 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		/* 写数据 */
		for (Le_u_i=0;Le_u_i < NumByteToWrite;Le_u_i++)
		{		
			/* 发送一字节要写入的数据 */
			I2C_SendData(iicInfo[Dev].I2Cx, *pBuffer);
			pBuffer++;
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			/* 检测 EV8 事件并清除标志*/
			while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if ((I2CTimeout--) == 0) 
				{
					return 0;
				}
			}
		}
		/* 发送停止信号 */
		I2C_GenerateSTOP(iicInfo[Dev].I2Cx, ENABLE);
	}
	
	return 1;
}

/**
* @brief 读取一块数据
* @param pBuffer:存放读取的数据的缓冲区指针
* @param Dev:设备索引
* @param RegRdAddr:读取数据的地址(设备数据寄存器地址，区别设备地址)
* @param NumByteToRead:要读取的字节数
* @retval 正常返回 1，异常返回 0
*/
uint8_t I2C_BytesRead(uint8_t Dev,uint8_t RegRdAddr,uint8_t* pBuffer,uint8_t NumByteToRead)
{
	uint8_t Le_u_i;
	uint32_t I2CTimeout = 0U;
	I2CTimeout = I2CT_LONG_TIMEOUT;
	
	if((0U != NumByteToRead) && (NumByteToRead <= 0xff))
	{
		while (I2C_GetFlagStatus(iicInfo[Dev].I2Cx, I2C_FLAG_BUSY))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}

		/* 产生 I2C 起始信号 */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV5 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* 发送 设备地址 */
		I2C_Send7bitAddress(iicInfo[Dev].I2Cx,iicInfo[Dev].DevAddr,I2C_Direction_Transmitter);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV6 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}

		/* 发送要读取的内部寄存器地址 */
		I2C_SendData(iicInfo[Dev].I2Cx, RegRdAddr);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV8 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		/* 产生第二次 I2C 起始信号 */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV5 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}

		I2C_Send7bitAddress(iicInfo[Dev].I2Cx, iicInfo[Dev].DevAddr|I2C_RD, I2C_Direction_Receiver);//发送设备读操作指令
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 检测 EV6 事件并清除标志*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		for(Le_u_i = 0U;Le_u_i < NumByteToRead;Le_u_i++)
		{/* 读取 NumByteToRead 个数据*/
			I2CTimeout = I2CT_LONG_TIMEOUT;
			while (I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
			{
				if ((I2CTimeout--) == 0) 
				{
					return 0;
				}
			}
			if(Le_u_i < (NumByteToRead-1))
			{	
				*pBuffer = I2C_ReceiveData(iicInfo[Dev].I2Cx);/*通过 I2C，从设备中读取一个字节的数据 */
				pBuffer++;/* 存储数据的指针指向下一个地址 */
				I2C_AcknowledgeConfig(iicInfo[Dev].I2Cx, ENABLE);	/* 使能应答，方便下一次 I2C 传输 */	
			}
			else
			{//接收最后1个字节
				*pBuffer = I2C_ReceiveData(iicInfo[Dev].I2Cx);/*通过 I2C，从设备中读取一个字节的数据 */
				I2C_AcknowledgeConfig(iicInfo[Dev].I2Cx, DISABLE);/* 发送非应答信号 */
			}
		}
		I2C_GenerateSTOP(iicInfo[Dev].I2Cx, ENABLE);/* 发送停止信号 */	
	}

	return 1;
}

#endif

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　可用逻辑分析仪测量I2C通讯时的频率
    工作条件：CPU主频168MHz ，MDK编译环境，1级优化经测试，循环次数为20~250时都能通讯正常*/
	for (i = 0; i < 25; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	EEPROM_I2C_SDA_1();
	EEPROM_I2C_SCL_1();
	i2c_Delay();
	EEPROM_I2C_SDA_0();
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	EEPROM_I2C_SDA_0();
	EEPROM_I2C_SCL_1();
	i2c_Delay();
	EEPROM_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			EEPROM_I2C_SDA_1();
		}
		else
		{
			EEPROM_I2C_SDA_0();
		}
		i2c_Delay();
		EEPROM_I2C_SCL_1();
		i2c_Delay();	
		EEPROM_I2C_SCL_0();
		if (i == 7)
		{
			 EEPROM_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

uint8_t send_byte(uint8_t _ucByte)
{
	uint8_t i;
	uint8_t uRet = 1;
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			EEPROM_I2C_SDA_1();
		}
		else
		{
			EEPROM_I2C_SDA_0();
		}
		i2c_Delay();
		EEPROM_I2C_SCL_1();
		i2c_Delay();	
		EEPROM_I2C_SCL_0();
		if (i == 7)
		{
			 EEPROM_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
	if (i2c_WaitAck() == 0)
	{
			uRet = 0;	/* EEPROM器件无应答 */
	}
	return uRet;

}


/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		EEPROM_I2C_SCL_1();
		i2c_Delay();
		if (EEPROM_I2C_SDA_READ())
		{
			value++;
		}
		EEPROM_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	EEPROM_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (EEPROM_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	EEPROM_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	EEPROM_I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	EEPROM_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	//i2c_CfgGpio();		/* 配置GPIO */

	
	i2c_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

	i2c_Stop();			/* 发送停止信号 */

	return ucAck;
}


uint8_t i2c_master_reg8_send(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength)
{ 
	uint8_t ucRet = 1;
	uint8_t i;
	
	i2c_CfgGpio(IIC_HYM8563,I2C_TYPE_SIMULATE);		/* 配置GPIO */
	i2c_Stop();	
	i2c_Start();
	if((send_byte(ucDevAddr)==0)&& (uclength!=0))
	{
		if ( send_byte(ucRegAddr)==0)
		{
			ucRet = 0;
			for ( i = 0 ; i  < uclength; i ++)
			{
				if (send_byte(*pucBuf)==0)
				{
					pucBuf ++;
                
				}			
				else
				{
					ucRet = 1;
					break;
				}
			}
		}
	}
	
	i2c_Stop();	
	return ucRet;
}


/*
	ucDevAddr iic设备地址
	ucRegAddr 寄存器地址，此处为读取按键值得寄存器地址
	pucBuf 	  数据寄存器
	uclength  数据长度
*/
uint8_t i2c_master_reg8_recv(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength)
{
	uint8_t ucRet = 1;
    uint8_t i,j,m;

	i2c_CfgGpio(IIC_TOUCHKEY,I2C_TYPE_SIMULATE);		/* 配置GPIO */
	for(i = 0; i < 20; i++)
	{
		i2c_Start();
		  
		if((send_byte(ucDevAddr)==0))
		{
			  i = 200;
			  if (send_byte( ucRegAddr )==0)
			  {
					i2c_Stop();
				 
					for(j = 0; j < 20;j++)
					{
						i2c_Start();
						if (send_byte(ucDevAddr + I2C_RD)==0)
						{
							j = 200;
							ucRet = 0;
							for ( m = 1 ; m < uclength; m++, pucBuf++)
							{				
								*pucBuf= i2c_ReadByte();
								i2c_Ack();
							}
							*pucBuf = i2c_ReadByte();
							i2c_NAck();
						}
					}
			  }
		}
	}
        
    i2c_Stop();
        
	return ucRet;
}


uint8_t I2C_Write_Byte(uint8_t ucDevAddr,uint8_t data)
{
	i2c_CfgGpio(IIC_LCD,I2C_TYPE_SIMULATE);		/* 配置GPIO */

	i2c_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(  (ucDevAddr<<1)| I2C_WR);
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
	i2c_SendByte(data);
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
	i2c_Stop();			/* 发送停止信号 */
	
	return 0;
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

