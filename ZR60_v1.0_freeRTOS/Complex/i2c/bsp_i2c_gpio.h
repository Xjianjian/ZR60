#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include "stm32f4xx.h"
#include <inttypes.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"


#define I2C_WR	0x00			/* д����bit */
#define I2C_RD	0x01			/* ������bit */

#define I2C_TYPE_SIMULATE		1		/* IICͨ�ŷ�ʽ��ģ�� */
#define I2C_TYPE_HARDWARE		2		/* IICͨ�ŷ�ʽ��Ӳ�� */

#define I2C_HARDWARE
#ifdef 	I2C_HARDWARE   //����Ӳ��iic
/* STM32 ����� I2C ��ַ�������ַֻҪ�� STM32 ��ҵ� I2C ������ַ��һ������ */
#define I2C_OWN_ADDRESS7 		0X0A

/* iic����hym8563��ַ */
#define I2C_HYM8563_ADDRESS 	0XA2
/* STM32<->hym8563 I2C ���� */
#define I2C_HYM8563_Speed 		200000 //200kbit/s


/*I2C �ӿ�*/
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


/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#define EEPROM_I2C_SCL_1()  GPIO_SetBits(GPIOB, I2C_SCL_PIN)		/* SCL = 1 */
#define EEPROM_I2C_SCL_0()  GPIO_ResetBits(GPIOB, I2C_SCL_PIN)		/* SCL = 0 */
	
#define EEPROM_I2C_SDA_1()  GPIO_SetBits(GPIOB, I2C_SDA_PIN)		/* SDA = 1 */
#define EEPROM_I2C_SDA_0()  GPIO_ResetBits(GPIOB, I2C_SDA_PIN)		/* SDA = 0 */
	
#define EEPROM_I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIOB,I2C_SDA_PIN)	/* ��SDA����״̬ */


/*ͨѶ�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


typedef enum
{
	IIC_HYM8563 = 0U,/*hym8563ʱ��оƬ*/
	IIC_WM8978,/*wm8978��ƵоƬ*/
	IIC_LCD,/*lcd��ʾ��*/
	IIC_TOUCHKEY,/*��������*/
	I2C_DEVICE_NUM//iic�豸����
}bsp_iic_DevIndex;//�豸��������


typedef struct
{
	uint8_t DevAddr;//�豸��ַ����������Ӧ���豸
	I2C_TypeDef* I2Cx;//I2Cͨ��
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




#ifdef 	I2C_HARDWARE   //����Ӳ��iic	
extern void i2c_CfgGpio(uint8_t Dev,uint8_t Le_u_workType);
extern void I2C_Mode_Config(uint8_t Dev);
extern uint8_t I2C_BytesWrite(uint8_t Dev,uint8_t RegWrAddr,uint8_t* pBuffer,uint8_t NumByteToWrite);
extern uint8_t I2C_BytesRead(uint8_t Dev,uint8_t RegRdAddr,uint8_t* pBuffer,uint8_t NumByteToRead);
#endif
/* �ⲿ����API */
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

