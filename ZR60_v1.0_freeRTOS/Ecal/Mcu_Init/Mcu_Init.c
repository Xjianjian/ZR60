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
	u8 ret;
	FRESULT res = FR_OK;
	uint32 Le_dw_InitDlyTime = 0xffffff;//��ʱʱ��
	/*******************
	 *	��ʼ��...	
	********************/
	while(Le_dw_InitDlyTime--);
	McuInit_RSTtype();
	//ϵͳʱ������(��ʼ�������ļ����ѵ���SystemInit����ϵͳʱ��)
	/* ����NVICΪ���ȼ���4 *//*һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ���*/
	/*���ȼ�����Ϊ 4 ��ʾ֧�� 0-15 ����ռ���ȼ���ע�⣬0-15 ���� 16 �����𣬰��� 0 ������ÿ����ռ�����ȣ�����ֻ��1�������ȼ���0����ֵԽС����ռ���ȼ��ļ���Խ��*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#ifdef freeRTOS_RUN_STATUS_DEBUG
	TIMx_Configuration();
#endif
	LED_GPIO_Config();//led��������
	Debug_USART_Config();//debug ��������
	USART3_Config();//FM17550ʹ��
	USART4_Config();//����ʹ��
	USART5_Config();//����ʹ��
	Rheostat_Init();/*��ʼ��ADC*/
	Key_Config();//����������
	printf("\n����汾��%s\n",CeZR60Ctrl_u_SoftVersion);
#ifndef HYM8563
	/* RTC���ã�ѡ��ʱ��Դ������RTC_CLK�ķ�Ƶϵ�� */
	RTC_CLK_Config_Backup();
#endif
	//LCD_Init();//lcd1062��ʼ��,Ŀǰδʹ��
	TM_FATFS_FLASH_SPI_disk_initialize();//�ⲿflash��ʼ��

	ret = wm8978_Init();//wm8978��λ
	if(ret == 1)
	{
		printf("\n��2��wm8978��ʼ����� ��\n");
	}
	InitAudioIO_playconfig();//��Ƶ���ų�ʼ�����ã�DMA��˫���壬wm8978������ʽ��
	MemIf_Init();
	InitZR60Ctrl_parameter();
	InitBtnFltr_Parameter();//�����˲���ʼ��	
	InitPSFltr_Parameter();
	BListMng_Init();
	InitBListCache_Parameter();//������������г�ʼ��
	InitUnlockLogCache_Parameter();//������־������г�ʼ��
	InitADFliter_parameter();
	InitSleepMng_parameter();

#ifdef NETWORK_ON
	NetConnIf_Parameter();
	ETH_BSP_Config();/* Configure ethernet (GPIOs, clocks, MAC, DMA) */	
	LwIP_Init();/* Initilaize the LwIP stack */
	stm8_fm17550_iap_parameter();
	http_client_iap_parameter();	
	InitdhcpClient_parameter();
	InitdnsAnalysis_parameter();//dns
	tcp_ShortConnect_parameter();//������
	tcp_LngConnect_Parameter();//������
	Initntpclient_Pramater();
#endif	
	LED_RGBOFF;
	printf("\nall��ʼ����ɣ�\n");
}

/*
	�鿴��λ����
*/
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
