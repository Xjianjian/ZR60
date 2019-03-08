/**
************************************************************
  * @file    SmltIIC.c
  * @version V1.0
  * @author  liujian
  * @date    2018-06-26
  * @brief   用gpio模拟i2c总线, 适用于STM32系列CPU。
			该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
************************************************************
  * @attention
  *
  * 
  * 
  * 
  *
************************************************************
  */ 

/*
************************************************************
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 
	检测I2C设备是否正常，该函数会配置GPIO
************************************************************
*/
#include "SimulateIIC/SmltIIC.h"

static void i2c_Start(void);
static void i2c_Stop(void);
static uint8 i2c_SendByte(uint8 _ucByte);
static uint8 i2c_ReadByte(void);
static uint8 i2c_WaitAck(void);
static void i2c_Ack(void);
static void i2c_NAck(void);
static void i2c_Delay(void);

/*
************************************************************
*	函 数 名: i2c_CfgGpio
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参：无
*	返 回 值: 无
************************************************************
*/
void SmltIIC_Paramter(void)
{
	i2c_Stop();//CPU发起I2C总线停止信号,释放总线
}

/*
************************************************************
*	函 数 名: SmltIIC_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，
				然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
************************************************************
*/
uint8 SmltIIC_CheckDevice(uint8 _Address)
{
	uint8 ucAck;
	
	SmltIIC_Paramter();/* 配置GPIO口 */
	
	i2c_Stop();/* 发送停止信号 */
	i2c_Start();/* 发送启动信号 */	
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte((_Address << 1) | I2C_WR);
	ucAck = i2c_WaitAck();/* 检测设备的ACK应答 */
	i2c_Stop();/* 发送停止信号 */

	return ucAck;
}

/*
************************************************************
	ucDevAddr iic设备地址
	ucRegAddr 寄存器地址
	pucBuf 	  待写入数据缓存buffer
	uclength  数据长度
************************************************************
*/
uint8 SmltIIC_send(uint8 ucDevAddr, uint8 ucRegAddr, \
						   uint8 *pucBuf, uint8 uclength)
{ 
	uint8 ucRet = 1;
	uint8 i;
	
	i2c_Stop();	
	i2c_Start();
	if((i2c_SendByte(ucDevAddr)!=0)&& (uclength==0))
	{//数据长度有误或发送从机地址，未收到正确应答
		goto cmd_fail;
	}
	
	if(i2c_SendByte(ucRegAddr)!=0)
	{//发送从机寄存器地址，未收到正确应答
		goto cmd_fail;
	}
	
	ucRet = 0;
	for( i = 0 ; i  < uclength; i++)
	{
		if(i2c_SendByte(*pucBuf)==0)
		{
			pucBuf ++;
		}			
		else
		{
			ucRet = 1;
			break;
		}
	}
	
	i2c_Stop();	
	return ucRet;
	
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return ucRet;
}


/*
************************************************************
	ucDevAddr iic设备地址
	ucRegAddr 寄存器地址
	pucBuf 	  读入数据缓存buffer
	uclength  数据长度
************************************************************
*/
uint8 SmltIIC_recv(uint8 ucDevAddr, uint8 ucRegAddr, \
						   uint8 *pucBuf, uint8 uclength)
{
	uint8 ucRet = 1;
    uint8 m;

	i2c_Stop();
	i2c_Start();
	if((i2c_SendByte((ucDevAddr << 1) | I2C_WR) !=0))//发送从机地址，未收到正确应答
	{
		goto cmd_fail;
	}
	
	if(i2c_SendByte( ucRegAddr )!=0)//发送从机寄存器地址，未收到正确应答
	{
		goto cmd_fail;
	}
	
	i2c_Stop();
	i2c_Start();
	
	if(i2c_SendByte((ucDevAddr << 1) | I2C_RD)==0)
	{
		for ( m = 1 ; m < uclength; m++, pucBuf++)
		{				
			*pucBuf= i2c_ReadByte();
			i2c_Ack();
		}
		*pucBuf = i2c_ReadByte();
		i2c_NAck();
		ucRet = 0;
	}    
    i2c_Stop();
    return ucRet;
	
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return ucRet;
}


/*
************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
************************************************************
*/
static void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	i2c_Delay();
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/*
************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
************************************************************
*/
static void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	i2c_Delay();
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
	i2c_Delay();
}


/*
************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
************************************************************
*/
static uint8 i2c_SendByte(uint8 _ucByte)
{
	uint8 i;
	uint8 uRet = 1;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();	
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
	if (i2c_WaitAck() == 0)
	{
			uRet = 0;	/* 无应答 */
	}
	return uRet;

}


/*
************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
************************************************************
*/
static uint8 i2c_ReadByte(void)
{
	uint8 i;
	uint8 value;

	/* 读到第1个bit为数据的高位bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
************************************************************
*/
static uint8 i2c_WaitAck(void)
{
	uint8 re;

	I2C_SDA_1();/* 释放SDA总线 */
	i2c_Delay();
	I2C_SCL_1();/* 驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (I2C_SDA_READ())/* 读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
************************************************************
*/
static void i2c_Ack(void)
{
	I2C_SDA_0();/* 驱动SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();/* 产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();/* 释放SDA总线 */
}

/*
************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
************************************************************
*/
static void i2c_NAck(void)
{
	I2C_SDA_1();/* 驱动SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();/* 产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();	
}


/*
************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
************************************************************
*/
static void i2c_Delay(void)
{
	uint8 i;

	/*　可用逻辑分析仪测量I2C通讯时的频率工作条件：CPU主频168MHz ，
		MDK编译环境，1级优化经测试，循环次数为20~250时都能通讯正常*/
	for (i = 0; i < 25; i++);
}
