/**
  ******************************************************************************
  * @file    bsp_i2c_gpio.c
  * @version V1.0
  * @author  liujian
  * @date    2018-06-26
  * @brief   ��gpioģ��i2c����, ������STM32ϵ��CPU����ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
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
	Ӧ��˵����
	�ڷ���I2C�豸ǰ�����ȵ��� i2c_CheckDevice() ���I2C�豸�Ƿ��������ú���������GPIO

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

#ifdef 	I2C_HARDWARE   //����Ӳ��iic	
/*
*********************************************************************************************************
*	�� �� ��: i2c_CfgGpio
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_CfgGpio(uint8_t Dev,uint8_t Le_u_workType)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(Dev < I2C_DEVICE_NUM)
	{
		if(I2C_TYPE_HARDWARE == Le_u_workType)
		{
			/*ʹ�� I2C ����ʱ�� */
			RCC_APB1PeriphClockCmd(iicInfo[Dev].RCC_APB1Periph, ENABLE);
			/*ʹ�� I2C ���ŵ� GPIO ʱ��*/
			RCC_AHB1PeriphClockCmd(iicInfo[Dev].RCC_AHB1Periph, ENABLE);
			/* Reset I2Cx IP */
			RCC_APB1PeriphResetCmd(iicInfo[Dev].RCC_APB1Periph, ENABLE);
			/* Release reset signal of I2Cx IP */
			RCC_APB1PeriphResetCmd(iicInfo[Dev].RCC_APB1Periph, DISABLE);
			/* ��������Դ PXx �� I2C_SCL*/
			GPIO_PinAFConfig(iicInfo[Dev].GPIOx, I2C_SCL_SOURCE,iicInfo[Dev].GPIO_AF);
			/* ��������Դ PXx �� to I2C_SDA*/
			GPIO_PinAFConfig(iicInfo[Dev].GPIOx,I2C_SDA_SOURCE,iicInfo[Dev].GPIO_AF);
			/*���� SCL SDA���� */
			GPIO_InitStructure.GPIO_Pin = iicInfo[Dev].SCL_pin | iicInfo[Dev].SDA_pin;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(iicInfo[Dev].GPIOx, &GPIO_InitStructure);
		}
		else
		{
			RCC_AHB1PeriphClockCmd(iicInfo[Dev].RCC_AHB1Periph, ENABLE);	/* ��GPIOʱ�� */
			GPIO_InitStructure.GPIO_Pin = iicInfo[Dev].SCL_pin | iicInfo[Dev].SDA_pin;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  	
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  	/* ��©��� */
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(iicInfo[Dev].GPIOx, &GPIO_InitStructure);
			/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
			i2c_Stop();
		}
	}
}

/**
* @brief I2C ����ģʽ����
* @param ��
* @retval ��
*/
void I2C_Mode_Config(uint8_t Dev)
{
	I2C_InitTypeDef I2C_InitStructure;
	/* I2C ���� */
	I2C_DeInit(iicInfo[Dev].I2Cx);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	/*ռ�ձ�*/
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	/*I2C �����ַ*/
	I2C_InitStructure.I2C_OwnAddress1 =I2C_OWN_ADDRESS7;
	/*ʹ����Ӧ*/
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	/* I2C ��Ѱַģʽ */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	/* ͨ������ */
	I2C_InitStructure.I2C_ClockSpeed = iicInfo[Dev].I2C_ClkSpeed;
	/*д������*/
	I2C_Init(iicInfo[Dev].I2Cx, &I2C_InitStructure);
	/* ʹ�� I2C */
	I2C_Cmd(iicInfo[Dev].I2Cx, ENABLE);
}


/**
* @brief ���������е�����д�� I2C �У����õ��ֽ�д��ķ�ʽ���ٶȱ�ҳд����
* @param Dev:�豸����
* @param pBuffer:������ָ��
* @param RegWrAddr:д��ַ(�豸���ݼĴ�����ַ�������豸��ַ)
* @param NumByteToWrite:д���ֽ���
* @retval ��
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
		/* ���� I2C ��ʼ�ź� */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		/*���ó�ʱ�ȴ�ʱ��*/
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV5 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* �����豸д����ָ�� */
		I2C_Send7bitAddress(iicInfo[Dev].I2Cx, iicInfo[Dev].DevAddr|I2C_WR,I2C_Direction_Transmitter);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV6 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* ����Ҫд��ļĴ�����ʼ��ַ */
		I2C_SendData(iicInfo[Dev].I2Cx, RegWrAddr);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV8 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		/* д���� */
		for (Le_u_i=0;Le_u_i < NumByteToWrite;Le_u_i++)
		{		
			/* ����һ�ֽ�Ҫд������� */
			I2C_SendData(iicInfo[Dev].I2Cx, *pBuffer);
			pBuffer++;
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			/* ��� EV8 �¼��������־*/
			while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if ((I2CTimeout--) == 0) 
				{
					return 0;
				}
			}
		}
		/* ����ֹͣ�ź� */
		I2C_GenerateSTOP(iicInfo[Dev].I2Cx, ENABLE);
	}
	
	return 1;
}

/**
* @brief ��ȡһ������
* @param pBuffer:��Ŷ�ȡ�����ݵĻ�����ָ��
* @param Dev:�豸����
* @param RegRdAddr:��ȡ���ݵĵ�ַ(�豸���ݼĴ�����ַ�������豸��ַ)
* @param NumByteToRead:Ҫ��ȡ���ֽ���
* @retval �������� 1���쳣���� 0
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

		/* ���� I2C ��ʼ�ź� */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV5 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		/* ���� �豸��ַ */
		I2C_Send7bitAddress(iicInfo[Dev].I2Cx,iicInfo[Dev].DevAddr,I2C_Direction_Transmitter);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV6 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}

		/* ����Ҫ��ȡ���ڲ��Ĵ�����ַ */
		I2C_SendData(iicInfo[Dev].I2Cx, RegRdAddr);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV8 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		/* �����ڶ��� I2C ��ʼ�ź� */
		I2C_GenerateSTART(iicInfo[Dev].I2Cx, ENABLE);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV5 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}

		I2C_Send7bitAddress(iicInfo[Dev].I2Cx, iicInfo[Dev].DevAddr|I2C_RD, I2C_Direction_Receiver);//�����豸������ָ��
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ��� EV6 �¼��������־*/
		while (!I2C_CheckEvent(iicInfo[Dev].I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if ((I2CTimeout--) == 0) 
			{
				return 0;
			}
		}
		
		for(Le_u_i = 0U;Le_u_i < NumByteToRead;Le_u_i++)
		{/* ��ȡ NumByteToRead ������*/
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
				*pBuffer = I2C_ReceiveData(iicInfo[Dev].I2Cx);/*ͨ�� I2C�����豸�ж�ȡһ���ֽڵ����� */
				pBuffer++;/* �洢���ݵ�ָ��ָ����һ����ַ */
				I2C_AcknowledgeConfig(iicInfo[Dev].I2Cx, ENABLE);	/* ʹ��Ӧ�𣬷�����һ�� I2C ���� */	
			}
			else
			{//�������1���ֽ�
				*pBuffer = I2C_ReceiveData(iicInfo[Dev].I2Cx);/*ͨ�� I2C�����豸�ж�ȡһ���ֽڵ����� */
				I2C_AcknowledgeConfig(iicInfo[Dev].I2Cx, DISABLE);/* ���ͷ�Ӧ���ź� */
			}
		}
		I2C_GenerateSTOP(iicInfo[Dev].I2Cx, ENABLE);/* ����ֹͣ�ź� */	
	}

	return 1;
}

#endif

/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*�������߼������ǲ���I2CͨѶʱ��Ƶ��
    ����������CPU��Ƶ168MHz ��MDK���뻷����1���Ż������ԣ�ѭ������Ϊ20~250ʱ����ͨѶ����*/
	for (i = 0; i < 25; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
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
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	EEPROM_I2C_SDA_0();
	EEPROM_I2C_SCL_1();
	i2c_Delay();
	EEPROM_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
			 EEPROM_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}

uint8_t send_byte(uint8_t _ucByte)
{
	uint8_t i;
	uint8_t uRet = 1;
	/* �ȷ����ֽڵĸ�λbit7 */
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
			 EEPROM_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
	if (i2c_WaitAck() == 0)
	{
			uRet = 0;	/* EEPROM������Ӧ�� */
	}
	return uRet;

}


/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	EEPROM_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (EEPROM_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
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
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	EEPROM_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	EEPROM_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	EEPROM_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	//i2c_CfgGpio();		/* ����GPIO */

	
	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | I2C_WR);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

	i2c_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}


uint8_t i2c_master_reg8_send(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength)
{ 
	uint8_t ucRet = 1;
	uint8_t i;
	
	i2c_CfgGpio(IIC_HYM8563,I2C_TYPE_SIMULATE);		/* ����GPIO */
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
	ucDevAddr iic�豸��ַ
	ucRegAddr �Ĵ�����ַ���˴�Ϊ��ȡ����ֵ�üĴ�����ַ
	pucBuf 	  ���ݼĴ���
	uclength  ���ݳ���
*/
uint8_t i2c_master_reg8_recv(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength)
{
	uint8_t ucRet = 1;
    uint8_t i,j,m;

	i2c_CfgGpio(IIC_TOUCHKEY,I2C_TYPE_SIMULATE);		/* ����GPIO */
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
	i2c_CfgGpio(IIC_LCD,I2C_TYPE_SIMULATE);		/* ����GPIO */

	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(  (ucDevAddr<<1)| I2C_WR);
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	i2c_SendByte(data);
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	i2c_Stop();			/* ����ֹͣ�ź� */
	
	return 0;
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 1;
}

