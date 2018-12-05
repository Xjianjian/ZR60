/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* File Name          : ARMJISHU_TouchScreen_ADS7843.h
* Author             : www.armjishu.com Team
* Version            : V3.0.1
* Date               : 03/20/2010
* Description        : 
*******************************************************************************/
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"


/* SPI_1 pins definition for test */
#define RCC_SPI2_CLK                RCC_APB1Periph_SPI2
#define SPI2_GPIO_PORT              GPIOB
#define RCC_SPI2_PORT               RCC_AHB1Periph_GPIOB
#define SPI2_SCK                    GPIO_Pin_13
#define SPI2_MISO                   GPIO_Pin_14
#define SPI2_MOSI                   GPIO_Pin_15
#define SPI2_SCK_SOURCE             GPIO_PinSource13
#define SPI2_MISO_SOURCE            GPIO_PinSource14
#define SPI2_MOSI_SOURCE            GPIO_PinSource15

/* ARMJISHU_TouchScreen_ADS7843 */

#define RCC_ADS7843_CS                         RCC_AHB1Periph_GPIOD
#define GPIO_ADS7843_CS_PORT                   GPIOD 
#define GPIO_ADS7843_CS                        GPIO_Pin_13 

//NRF24L01 SPI½Ó¿ÚCSÐÅºÅ
#define NRF24L01_CSN      		GPIO_Pin_1
#define GPIO_NRF24L01_CSN  		GPIOA
#define RCC_NRF24L01_CSN  		RCC_AHB1Periph_GPIOD 



#define RCC_ADS7843_INT                        RCC_AHB1Periph_GPIOG
#define GPIO_ADS7843_INT_PORT                  GPIOG  
#define GPIO_ADS7843_INT                       GPIO_Pin_7  
#define GPIO_ADS7843_EXTI_LINE                 EXTI_Line7
#define GPIO_ADS7843_EXTI_PORT_SOURCE          EXTI_PortSourceGPIOG
#define GPIO_ADS7843_EXTI_PIN_SOURCE           EXTI_PinSource7
#define GPIO_ADS7843_EXTI_IRQn                 EXTI9_5_IRQn 

#define TOUCHLEFT				3650
#define	TOUCHRIGHT			30000
#define TOUCHTOP				3100
#define	TOUCHBUTTOM			27900

#define GPIO_ADS7843_INT_VALID   (!GPIO_ReadInputDataBit(GPIO_ADS7843_INT_PORT, GPIO_ADS7843_INT))

/* Select ADS7843: ChipSelect pin low  */
#define ADS7843_CS_LOW()     GPIO_ResetBits(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS)
/* Deselect ADS7843: ChipSelect pin high */
#define ADS7843_CS_HIGH()    GPIO_SetBits(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS)

/* pin low  */
#define SPI1_SCK_LOW()     GPIO_ResetBits(SPI1_PORT, SPI1_SCK)
/* pin high */
#define SPI1_SCK_HIGH()    GPIO_SetBits(SPI1_PORT, SPI1_SCK)

/* pin read  */
#define SPI1_MISO_READ()     GPIO_ReadInputDataBit(SPI1_PORT, SPI1_MISO)

/* pin low  */
#define SPI1_MOSI_LOW()     GPIO_ResetBits(SPI1_PORT, SPI1_MOSI)
/* pin high */
#define SPI1_MOSI_HIGH()    GPIO_SetBits(SPI1_PORT, SPI1_MOSI)

void ADS7843_Init(void);
u16 TPReadX(void);
u16 TPReadY(void);
void ADS7843_Rd_Addata(u16 *X_Addata,u16 *Y_Addata);
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/

