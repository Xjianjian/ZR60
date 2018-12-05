#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

// ADC GPIO �궨��
#define RHEOSTAT_ADC_GPIO_PORT    GPIOB
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_1
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOB

// ADC ��ź궨��
#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_9

// ADC DR�Ĵ����궨�壬ADCת���������ֵ����������
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC1+0x4c)

// ADC DMA ͨ���궨�壬��������ʹ��DMA����
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

extern __IO uint16_t ADC_ConvertedValue;//ADC1

void bsp_adc_Init(void);

#endif /* __BSP_ADC_H */



