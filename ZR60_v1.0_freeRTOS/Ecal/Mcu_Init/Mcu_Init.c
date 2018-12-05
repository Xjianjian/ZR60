/******************************************************
�ļ�����	Mcu_Init.c

������		
Data			Vasion			author
2018/1/4		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "Mcu_Init.h"


/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/
//SD����flash�ĸ�Ŀ¼
#define SD_ROOT       "0:"
#define FLASH_ROOT    "1:"

FATFS flash_fs;/*flash��sd�����ļ�ϵͳ���*/

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
u8 volatile machine_type = 0U;//�豸���ͣ�1--Χǽ����2--�ſڻ�

static void McuInit_RSTtype(void);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������Mcu_Init

*��  �Σ�void

*����ֵ��void

*��  ����Mcu�ײ��ʼ��

*��  ע��
******************************************************/
void Mcu_Init(void)
{
	uint32 Le_dw_InitDlyTime = 0xffffff;//��ʱʱ��
	/*******************
	 *	��ʼ��...	
	********************/
	while(Le_dw_InitDlyTime--);
	McuInit_RSTtype();
	/*ϵͳʱ������(��ʼ�������ļ����ѵ���SystemInit����ϵͳʱ��)*/
	/*����NVICΪ���ȼ���4 *//*һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ���*/
	/*���ȼ�����Ϊ 4 ��ʾ֧�� 0-15 ����ռ���ȼ���ע�⣬0-15 ���� 16 �����𣬰��� 0 ������ÿ
	����ռ�����ȣ�����ֻ��1�������ȼ���0����ֵԽС����ռ���ȼ��ļ���Խ��*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/*  user init code begin */

	/*  user init code end */
}

/******************************************************
*��������McuInit_RSTtype

*��  �Σ�void

*����ֵ��void

*��  �����鿴��λ����

*��  ע��
******************************************************/
static void McuInit_RSTtype(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)
	{
		printf("\r\n�ϵ�/���縴λ\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		printf("\r\n�������Ź���λ\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET)
	{
		printf("\r\n�͵�ѹ��λ\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)
	{
		printf("\r\n�����λ\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)
	{
		printf("\r\n���Ÿ�λ\r\n");
	}
	
	RCC_ClearFlag();//�����λ��־
}
