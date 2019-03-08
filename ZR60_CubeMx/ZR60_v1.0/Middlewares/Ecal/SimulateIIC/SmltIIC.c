/**
************************************************************
  * @file    SmltIIC.c
  * @version V1.0
  * @author  liujian
  * @date    2018-06-26
  * @brief   ��gpioģ��i2c����, ������STM32ϵ��CPU��
			��ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
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
	Ӧ��˵����
	�ڷ���I2C�豸ǰ�����ȵ��� i2c_CheckDevice() 
	���I2C�豸�Ƿ��������ú���������GPIO
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
*	�� �� ��: i2c_CfgGpio
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
void SmltIIC_Paramter(void)
{
	i2c_Stop();//CPU����I2C����ֹͣ�ź�,�ͷ�����
}

/*
************************************************************
*	�� �� ��: SmltIIC_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��
				Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
************************************************************
*/
uint8 SmltIIC_CheckDevice(uint8 _Address)
{
	uint8 ucAck;
	
	SmltIIC_Paramter();/* ����GPIO�� */
	
	i2c_Stop();/* ����ֹͣ�ź� */
	i2c_Start();/* ���������ź� */	
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte((_Address << 1) | I2C_WR);
	ucAck = i2c_WaitAck();/* ����豸��ACKӦ�� */
	i2c_Stop();/* ����ֹͣ�ź� */

	return ucAck;
}

/*
************************************************************
	ucDevAddr iic�豸��ַ
	ucRegAddr �Ĵ�����ַ
	pucBuf 	  ��д�����ݻ���buffer
	uclength  ���ݳ���
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
	{//���ݳ���������ʹӻ���ַ��δ�յ���ȷӦ��
		goto cmd_fail;
	}
	
	if(i2c_SendByte(ucRegAddr)!=0)
	{//���ʹӻ��Ĵ�����ַ��δ�յ���ȷӦ��
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
	
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return ucRet;
}


/*
************************************************************
	ucDevAddr iic�豸��ַ
	ucRegAddr �Ĵ�����ַ
	pucBuf 	  �������ݻ���buffer
	uclength  ���ݳ���
************************************************************
*/
uint8 SmltIIC_recv(uint8 ucDevAddr, uint8 ucRegAddr, \
						   uint8 *pucBuf, uint8 uclength)
{
	uint8 ucRet = 1;
    uint8 m;

	i2c_Stop();
	i2c_Start();
	if((i2c_SendByte((ucDevAddr << 1) | I2C_WR) !=0))//���ʹӻ���ַ��δ�յ���ȷӦ��
	{
		goto cmd_fail;
	}
	
	if(i2c_SendByte( ucRegAddr )!=0)//���ʹӻ��Ĵ�����ַ��δ�յ���ȷӦ��
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
	
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return ucRet;
}


/*
************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
static void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
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
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
static void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	i2c_Delay();
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
	i2c_Delay();
}


/*
************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
************************************************************
*/
static uint8 i2c_SendByte(uint8 _ucByte)
{
	uint8 i;
	uint8 uRet = 1;

	/* �ȷ����ֽڵĸ�λbit7 */
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
			 I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
	if (i2c_WaitAck() == 0)
	{
			uRet = 0;	/* ��Ӧ�� */
	}
	return uRet;

}


/*
************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
************************************************************
*/
static uint8 i2c_ReadByte(void)
{
	uint8 i;
	uint8 value;

	/* ������1��bitΪ���ݵĸ�λbit7 */
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
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
************************************************************
*/
static uint8 i2c_WaitAck(void)
{
	uint8 re;

	I2C_SDA_1();/* �ͷ�SDA���� */
	i2c_Delay();
	I2C_SCL_1();/* ����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (I2C_SDA_READ())/* ��ȡSDA����״̬ */
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
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
static void i2c_Ack(void)
{
	I2C_SDA_0();/* ����SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();/* ����1��ʱ�� */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();/* �ͷ�SDA���� */
}

/*
************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
static void i2c_NAck(void)
{
	I2C_SDA_1();/* ����SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();/* ����1��ʱ�� */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();	
}


/*
************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
************************************************************
*/
static void i2c_Delay(void)
{
	uint8 i;

	/*�������߼������ǲ���I2CͨѶʱ��Ƶ�ʹ���������CPU��Ƶ168MHz ��
		MDK���뻷����1���Ż������ԣ�ѭ������Ϊ20~250ʱ����ͨѶ����*/
	for (i = 0; i < 25; i++);
}
