#ifndef _SMLT_IIC_H
#define _SMLT_IIC_H

#include "Include.h"



#define I2C_WR	0x00			/* д����bit */
#define I2C_RD	0x01			/* ������bit */

/* STM32 ����� I2C ��ַ�������ַֻҪ�� STM32 ��ҵ� I2C ������ַ��һ������ */
#define I2C_OWN_ADDRESS7 		0X0A

/* iic����hym8563��ַ */
#define I2C_TOUCH_KEY_ADDRESS 	0x37
/* STM32<->hym8563 I2C ���� */
#define I2C_TOUCH_KEY_SPEED		100000 //100kbit/s


/*I2C �ӿ�*/
#define I2C 					I2C1
#define I2C_SCL_PIN 			GPIO_PIN_8
#define I2C_SCL_GPIO_PORT 		GPIOB
#define I2C_SDA_PIN 			GPIO_PIN_9
#define I2C_SDA_GPIO_PORT 		GPIOB


/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#define I2C_SCL_1()  HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_PIN,GPIO_PIN_SET)/* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_PIN,GPIO_PIN_RESET)/* SCL = 0 */
	
#define I2C_SDA_1()  HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_PIN,GPIO_PIN_SET)/* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_PIN,GPIO_PIN_RESET)/* SDA = 0 */
	
#define I2C_SDA_READ()  HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT,I2C_SDA_PIN)	/* ��SDA����״̬ */



typedef enum
{
	IIC_TOUCHKEY,/*��������*/
	I2C_DEVICE_NUM//iic�豸����
}bsp_iic_DevIndex;//�豸��������


/* �ⲿ����API */
extern void  SmltIIC_Paramter(void);
extern uint8 SmltIIC_CheckDevice(uint8 _Address);
extern uint8 SmltIIC_send(uint8 ucDevAddr, uint8 ucRegAddr, uint8 *pucBuf, uint8 uclength);
extern uint8 SmltIIC_recv(uint8 ucDevAddr, uint8 ucRegAddr, uint8 *pucBuf, uint8 uclength);

#endif

