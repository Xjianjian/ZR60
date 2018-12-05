#ifndef __BEEP_H_
#define	__BEEP_H_

#include "stm32f4xx.h"



/* ������������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶķ��������� */
#define BEEP_GPIO_PORT    	GPIOG			              /* GPIO�˿� */
#define BEEP_GPIO_CLK 	    RCC_AHB1Periph_GPIOG		/* GPIO�˿�ʱ�� */
#define BEEP_GPIO_PIN		  	GPIO_Pin_7			        /* ���ӵ���������GPIO */

#define LOCK_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define LOCK_GPIO_CLK 	    RCC_AHB1Periph_GPIOG		/* GPIO�˿�ʱ�� */
#define LOCK_GPIO_PIN		  	GPIO_Pin_0			        /* ���ӵ���������GPIO */

					
/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRRL=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BSRRH=i;}				//����͵�ƽ
#define digitalToggle(p,i)	{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
#define BEEP_TOGGLE		digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF				digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON				digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

/* �������IO�ĺ� */
#define LOCK_TOGGLE		digitalToggle(LOCK_GPIO_PORT,LOCK_GPIO_PIN)
#define LOCK_OFF				digitalLo(LOCK_GPIO_PORT,LOCK_GPIO_PIN)
#define LOCK_ON				digitalHi(LOCK_GPIO_PORT,LOCK_GPIO_PIN)

void BEEP_GPIO_Config(void);
#endif /* __BEEP_H_ */
