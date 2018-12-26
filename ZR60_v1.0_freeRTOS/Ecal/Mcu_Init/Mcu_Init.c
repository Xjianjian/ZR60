/******************************************************
文件名：	Mcu_Init.c

描述：		
Data			Vasion			author
2018/1/4		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "Mcu_Init.h"


/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/
//SD卡及flash的根目录
#define SD_ROOT       "0:"
#define FLASH_ROOT    "1:"

FATFS flash_fs;/*flash及sd卡的文件系统句柄*/

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
u8 volatile machine_type = 0U;//设备类型：1--围墙机；2--门口机

static void McuInit_RSTtype(void);
static void McuInit_UnusedGPIOConfig(void);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：Mcu_Init

*形  参：void

*返回值：void

*描  述：Mcu底层初始化

*备  注：
******************************************************/
void Mcu_Init(void)
{
	u8 ret;
	FRESULT res = FR_OK;
	uint32 Le_dw_InitDlyTime = 0xffffff;//延时时间
	/*******************
	 *	初始化...	
	********************/
	while(Le_dw_InitDlyTime--);
	McuInit_RSTtype();
	//系统时钟配置(初始化启动文件中已调用SystemInit配置系统时钟)
	/* 配置NVIC为优先级组4 *//*一旦初始化好 NVIC 的优先级分组后，切不可以在应用中再次更改*/
	/*优先级分组为 4 表示支持 0-15 级抢占优先级（注意，0-15 级是 16 个级别，包含 0 级），每个抢占（优先）级下只有1个副优先级：0。数值越小，抢占优先级的级别越高*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#ifdef freeRTOS_RUN_STATUS_DEBUG
	TIMx_Configuration();
#endif
	LED_GPIO_Config();//led引脚配置
	Debug_USART_Config();//debug 串口配置
	USART3_Config();//FM17550使用
	USART4_Config();//声波使用
	USART5_Config();//蓝牙使用
	Rheostat_Init();/*初始化ADC*/
	Key_Config();//按键口配置
	printf("\n软件版本：%s\n",CeZR60Ctrl_u_SoftVersion);
#ifndef HYM8563
	/* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
	RTC_CLK_Config_Backup();
#endif
	//LCD_Init();//lcd1062初始化,目前未使用
	TM_FATFS_FLASH_SPI_disk_initialize();//外部flash初始化

	ret = wm8978_Init();//wm8978复位
	if(ret == 1)
	{
		printf("\n【2】wm8978初始化完成 √\n");
	}
	InitAudioIO_playconfig();//音频播放初始化配置（DMA的双缓冲，wm8978工作方式）
	MemIf_Init();
	InitZR60Ctrl_parameter();
	InitBtnFltr_Parameter();//按键滤波初始化	
	InitPSFltr_Parameter();
	BListMng_Init();
	InitBListCache_Parameter();//黑名单缓存队列初始化
	InitUnlockLogCache_Parameter();//开门日志缓存队列初始化
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
	tcp_ShortConnect_parameter();//短连接
	tcp_LngConnect_Parameter();//长连接
	Initntpclient_Pramater();
#endif	
	LED_RGBOFF;
	McuInit_UnusedGPIOConfig();
	printf("\nall初始化完成！\n");
}

/*
	查看复位类型
*/
static void McuInit_RSTtype(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)
	{
		printf("\r\n上电/掉电复位\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		printf("\r\n独立看门狗复位\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET)
	{
		printf("\r\n低电压复位\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)
	{
		printf("\r\n软件复位\r\n");
	}
	
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)
	{
		printf("\r\n引脚复位\r\n");
	}
	
	RCC_ClearFlag();//清除复位标志
}

/*
	未使用的引脚处理
*/
static void McuInit_UnusedGPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	/*开启按键GPIO口的时钟*/
	//RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK|DOORLOCK_GPIO_CLK|KEY2_INT_GPIO_CLK| \
						   KEY3_INT_GPIO_CLK|KEY4_INT_GPIO_CLK ,ENABLE);//已开启过，此处不再开启

	/* 设置引脚为输入模式 */ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
	/* 设置引脚下拉 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	/* port A */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/* 初始化引脚 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);//PA3
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	/* 初始化引脚 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);//PA4
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	/* 初始化引脚 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);//PA6
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	/* 初始化引脚 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);//PA8
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	/* 初始化引脚 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);//PA15

	/* port B */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	/* 初始化引脚 */
	GPIO_Init(GPIOB, &GPIO_InitStructure);//PB6
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	/* 初始化引脚 */
	GPIO_Init(GPIOB, &GPIO_InitStructure);//PB7

	/* port C */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC0
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC3
	/* 选择引脚 */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	/* 初始化引脚 */
	//GPIO_Init(GPIOC, &GPIO_InitStructure);//PC6
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC7
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC8
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC9
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC10
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC14
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	/* 初始化引脚 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);//PC15


	/* port D */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD0
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD1
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD3
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD4
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD5
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD6
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD7
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD9
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD10
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD11
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD12
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD13
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD14
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	/* 初始化引脚 */
	GPIO_Init(GPIOD, &GPIO_InitStructure);//PD15

	/* port E */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE0
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE1
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE2
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE4
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE5
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE6
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE7
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE8
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE9
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE10
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE11
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE12
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE13
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE14
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	/* 初始化引脚 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);//PE15

	/* port F */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF0
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF1
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF2
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF3
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF4
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF5
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF9
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF11
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF12
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF13
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF14
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	/* 初始化引脚 */
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF15

	/* port G */
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG0
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG1
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG2
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG3
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG4
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG5
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG8
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG9
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG10
	/* 选择引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	/* 初始化引脚 */
	GPIO_Init(GPIOG, &GPIO_InitStructure);//PG12
}