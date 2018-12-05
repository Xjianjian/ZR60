#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include "stm32f4xx.h"
#include <inttypes.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"


#define I2C_WR	0x00			/* 写控制bit */
#define I2C_RD	0x01			/* 读控制bit */

#define I2C_TYPE_SIMULATE		1		/* IIC通信方式：模拟 */
#define I2C_TYPE_HARDWARE		2		/* IIC通信方式：硬件 */

#define I2C_HARDWARE
#ifdef 	I2C_HARDWARE   //定义硬件iic
/* STM32 自身的 I2C 地址，这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 		0X0A

/* iic外设hym8563地址 */
#define I2C_HYM8563_ADDRESS 	0XA2
/* STM32<->hym8563 I2C 速率 */
#define I2C_HYM8563_Speed 		200000 //200kbit/s


/*I2C 接口*/
#define I2C 					I2C1
#define I2C_CLK 				RCC_APB1Periph_I2C1
#define I2C_SCL_PIN 			GPIO_Pin_8
#define I2C_SCL_GPIO_PORT 		GPIOB
#define I2C_SCL_GPIO_CLK 		RCC_AHB1Periph_GPIOB
#define I2C_SCL_SOURCE 			GPIO_PinSource8
#define I2C_SCL_AF 				GPIO_AF_I2C1
#define I2C_SDA_PIN 			GPIO_Pin_9
#define I2C_SDA_GPIO_PORT 		GPIOB
#define I2C_SDA_GPIO_CLK 		RCC_AHB1Periph_GPIOB
#define I2C_SDA_SOURCE 			GPIO_PinSource9
#define I2C_SDA_AF 				GPIO_AF_I2C1
#endif


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#define EEPROM_I2C_SCL_1()  GPIO_SetBits(GPIOB, I2C_SCL_PIN)		/* SCL = 1 */
#define EEPROM_I2C_SCL_0()  GPIO_ResetBits(GPIOB, I2C_SCL_PIN)		/* SCL = 0 */
	
#define EEPROM_I2C_SDA_1()  GPIO_SetBits(GPIOB, I2C_SDA_PIN)		/* SDA = 1 */
#define EEPROM_I2C_SDA_0()  GPIO_ResetBits(GPIOB, I2C_SDA_PIN)		/* SDA = 0 */
	
#define EEPROM_I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIOB,I2C_SDA_PIN)	/* 读SDA口线状态 */


/*通讯等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


typedef enum
{
	IIC_HYM8563 = 0U,/*hym8563时钟芯片*/
	IIC_WM8978,/*wm8978音频芯片*/
	IIC_LCD,/*lcd显示屏*/
	IIC_TOUCHKEY,/*触摸按键*/
	I2C_DEVICE_NUM//iic设备数量
}bsp_iic_DevIndex;//设备类型索引


typedef struct
{
	uint8_t DevAddr;//设备地址，代表了相应的设备
	I2C_TypeDef* I2Cx;//I2C通道
	uint32_t I2C_ClkSpeed;
	uint32_t RCC_APB1Periph;
	uint32_t RCC_AHB1Periph;
	GPIO_TypeDef* GPIOx;
	uint16_t SCL_PinSource;
	uint16_t SDA_PinSource;
	uint8_t GPIO_AF;
	uint32_t SCL_pin;
	uint32_t SDA_pin;
}bsp_iic_confStruct;




#ifdef 	I2C_HARDWARE   //定义硬件iic	
extern void i2c_CfgGpio(uint8_t Dev,uint8_t Le_u_workType);
extern void I2C_Mode_Config(uint8_t Dev);
extern uint8_t I2C_BytesWrite(uint8_t Dev,uint8_t RegWrAddr,uint8_t* pBuffer,uint8_t NumByteToWrite);
extern uint8_t I2C_BytesRead(uint8_t Dev,uint8_t RegRdAddr,uint8_t* pBuffer,uint8_t NumByteToRead);
#endif
/* 外部调用API */
extern void i2c_Start(void);
extern void i2c_Stop(void);
extern void i2c_SendByte(uint8_t _ucByte);
extern uint8_t i2c_ReadByte(void);
extern uint8_t i2c_WaitAck(void);
extern void i2c_Ack(void);
extern void i2c_NAck(void);
extern uint8_t i2c_CheckDevice(uint8_t _Address);

extern uint8_t I2C_Write_Byte(uint8_t ucDevAddr,uint8_t data);
extern uint8_t send_byte(uint8_t _ucByte);
extern uint8_t i2c_master_reg8_send(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength);
extern uint8_t i2c_master_reg8_recv(uint8_t ucDevAddr, uint8_t ucRegAddr, uint8_t *pucBuf, uint8_t uclength);

#endif

