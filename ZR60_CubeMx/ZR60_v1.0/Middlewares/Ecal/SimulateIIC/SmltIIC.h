#ifndef _SMLT_IIC_H
#define _SMLT_IIC_H

#include "Include.h"



#define I2C_WR	0x00			/* 写控制bit */
#define I2C_RD	0x01			/* 读控制bit */

/* STM32 自身的 I2C 地址，这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 		0X0A

/* iic外设hym8563地址 */
#define I2C_TOUCH_KEY_ADDRESS 	0x37
/* STM32<->hym8563 I2C 速率 */
#define I2C_TOUCH_KEY_SPEED		100000 //100kbit/s


/*I2C 接口*/
#define I2C 					I2C1
#define I2C_SCL_PIN 			GPIO_PIN_8
#define I2C_SCL_GPIO_PORT 		GPIOB
#define I2C_SDA_PIN 			GPIO_PIN_9
#define I2C_SDA_GPIO_PORT 		GPIOB


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#define I2C_SCL_1()  HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_PIN,GPIO_PIN_SET)/* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_PIN,GPIO_PIN_RESET)/* SCL = 0 */
	
#define I2C_SDA_1()  HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_PIN,GPIO_PIN_SET)/* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_PIN,GPIO_PIN_RESET)/* SDA = 0 */
	
#define I2C_SDA_READ()  HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT,I2C_SDA_PIN)	/* 读SDA口线状态 */



typedef enum
{
	IIC_TOUCHKEY,/*触摸按键*/
	I2C_DEVICE_NUM//iic设备数量
}bsp_iic_DevIndex;//设备类型索引


/* 外部调用API */
extern void  SmltIIC_Paramter(void);
extern uint8 SmltIIC_CheckDevice(uint8 _Address);
extern uint8 SmltIIC_send(uint8 ucDevAddr, uint8 ucRegAddr, uint8 *pucBuf, uint8 uclength);
extern uint8 SmltIIC_recv(uint8 ucDevAddr, uint8 ucRegAddr, uint8 *pucBuf, uint8 uclength);

#endif

