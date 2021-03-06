/************************************************
  * @file    main.c
  * @author  liujian
  * @version V1.0
  * @date    2018-3-28
  * @brief
  ***********************************************
  * @attention
  *
  * 平台:秉火  STM32 F407 开发板  
  *
  *
  *
 ************************************************/
#include "Include.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "event_groups.h"
#include "./led/bsp_led.h"

/*************************************************
  *        				宏定义
**************************************************/
//#define RunSt_bits_Task  		(1 << 0)
#define LwipApp5ms_bits_Task  	(1 << 1)
#define LwipApp100ms_bits_Task  (1 << 2)
#define iap_bits_Task  			(1 << 3)
#define Fliter_bits_Task  		(1 << 4)
#define ZR60Ctrl5ms_bits_Task  	(1 << 5)
#define ZR60Ctrl100ms_bits_Task (1 << 6)
#define SleepMng_bits_Task 		(1 << 7)
#define https_bits_Task 		(1 << 8)
#define All_bits_Task  			(LwipApp5ms_bits_Task | LwipApp100ms_bits_Task | iap_bits_Task | SleepMng_bits_Task\
								 | Fliter_bits_Task | ZR60Ctrl5ms_bits_Task | ZR60Ctrl100ms_bits_Task)
static EventGroupHandle_t  		xCreatedEventGroup;

/*************************************************
  *        				定义变量
**************************************************/

/*************************************************
  *        				定义静态函数
**************************************************/
static void AppTaskCreate(void);
static void Lwip_app_5msTask(void * pvParameters);
static void Lwip_app_100msTask(void * pvParameters);
static void iap_Task(void * pvParameters);
static void Fliter_Task(void * pvParameters);
static void ZR60Ctrl_5msTask(void * pvParameters);
static void ZR60Ctrl_10msTask(void * pvParameters);
#ifdef freeRTOS_RUN_STATUS_DEBUG
static void vTaskRunningStatus(void *pvParameters);
#endif
static void AppObjCreate (void);
static void vTaskFeedDog(void *pvParameters);
static void LowPower_Task(void * pvParameters);
static void https_Task(void * pvParameters);
/*************************************************
  *        				main函数
**************************************************/
int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x40000;//中断向量表偏移
	Mcu_Init();/* 初始化 */
	
	/* Enable RNG clock source *///polarSSL库要使用到
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	/* Enable RNG IP  */
	RNG_Cmd(ENABLE);
	/* Enable CRYP clock */
	//RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
	/* Enable HASH clock */
	//RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);
	
	AppObjCreate(); /* 创建事件标志组 */
	AppTaskCreate();/* 创建任务 */
	
	while(1)
	{}
}


/* 创建任务 */
static void AppTaskCreate(void)
{
#ifdef freeRTOS_RUN_STATUS_DEBUG
	xTaskCreate(vTaskRunningStatus, (const char *)"RunningStatus", 	1024 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 1, STD_NULL_PTR);
#endif
	xTaskCreate(LowPower_Task, (const char *)"LowPower", 	256 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 2, STD_NULL_PTR);
	xTaskCreate(iap_Task, 			(const char *)"iap", 			512 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 3, STD_NULL_PTR);
	xTaskCreate(Lwip_app_100msTask, (const char *)"Lwip_app_100ms", 1024 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 3, STD_NULL_PTR);
	xTaskCreate(https_Task, (const char *)"https", 1024 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 1, STD_NULL_PTR);
	xTaskCreate(ZR60Ctrl_5msTask, 	(const char *)"ZR60Ctrl_5ms", 	512 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 5,STD_NULL_PTR);
	xTaskCreate(ZR60Ctrl_10msTask, 	(const char *)"ZR60Ctrl_10ms", 	512 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 3,STD_NULL_PTR);
	xTaskCreate(Lwip_app_5msTask, 	(const char *)"Lwip_app_5ms", 	512 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 4, STD_NULL_PTR);
	xTaskCreate(Fliter_Task,		(const char *)"Fliter", 		128 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 5,STD_NULL_PTR);
	xTaskCreate(vTaskFeedDog,		(const char *)"FeedDog", 		128 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 5,STD_NULL_PTR);
	
	vTaskStartScheduler();//启动调动器
}

static void Lwip_app_5msTask(void * pvParameters)
{
	static uint64_t Se_dw_LocalTime = 0U;
	while(1)
	{	
		TskdhcpClient_MainFunction();
		TskdnsAnalysis_MainFunction();
	
		/*********************************
		以下接口需要周期调用
		*********************************/
		/* handle periodic timers for LwIP */
		Se_dw_LocalTime = Se_dw_LocalTime + 5U;
		LwIP_Periodic_Handle(Se_dw_LocalTime);
		
		xEventGroupSetBits(xCreatedEventGroup,LwipApp5ms_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(5 / portTICK_RATE_MS);
	}
}


static void https_Task(void * pvParameters)
{
	while(1)
	{
		tcp_ShortConnect_MainFunction();
		
		//xEventGroupSetBits(xCreatedEventGroup, https_bits_Task);
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}


static void Lwip_app_100msTask(void * pvParameters)
{
	while(1)
	{
		tcp_LngConnect_MainFunction();
		Tskntpclient_MainFunction();
		
		xEventGroupSetBits(xCreatedEventGroup, LwipApp100ms_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(35 / portTICK_RATE_MS);
	}
}

static void iap_Task(void * pvParameters)
{
	while(1)
	{
		//stm8_fm17550_iap_MainFunction();
		http_client_iap_MainFunction();
		
		xEventGroupSetBits(xCreatedEventGroup, iap_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}


/* 滤波任务 */
static void Fliter_Task(void * pvParameters)
{
	while(1)
	{
		MngADFliter_mainFunction();
		TskBtnFltr_MainFunction();//按键扫描
		TskPSFltr_MainFunction();//外设输入硬线信号扫描
		
		xEventGroupSetBits(xCreatedEventGroup, Fliter_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(5 / portTICK_RATE_MS);
	}
}

static void ZR60Ctrl_5msTask(void * pvParameters)
{
	while(1)
	{
		/*******************
		*	ZR60控制逻辑
		********************/	
		TskZR60Ctrl_MainFunction();
		TskAudioIO_MainFunction();//语音播放
		
		xEventGroupSetBits(xCreatedEventGroup, ZR60Ctrl5ms_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(20 / portTICK_RATE_MS);
	}
}

static void ZR60Ctrl_10msTask(void * pvParameters)
{
	while(1)
	{
		TskBListMng_MainFunction();
		TskMemIf_MainFunction();//写flash或eeprom
		
		xEventGroupSetBits(xCreatedEventGroup, ZR60Ctrl100ms_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}

static void LowPower_Task(void * pvParameters)
{
	while(1)
	{
		TskSleepMng_MainFunction();	
		
		xEventGroupSetBits(xCreatedEventGroup, SleepMng_bits_Task);
		//IWDG_Feed();//喂狗
		vTaskDelay(20 / portTICK_RATE_MS);
	}
}

#ifdef freeRTOS_RUN_STATUS_DEBUG
/* 运行状态统计 */
static void vTaskRunningStatus(void *pvParameters)
{
    uint8_t pcWriteBuffer[700];

    while(1)
    {
		printf("\r\n任务名         任务状态 优先级 剩余栈   任务序号\r\n");
		vTaskList((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
		
		printf("\r\n任务名          运行计数        使用率\r\n");
		vTaskGetRunTimeStats((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
		
		//xEventGroupSetBits(xCreatedEventGroup, RunSt_bits_Task);
        IWDG_Feed();//喂狗
        vTaskDelay(30000);
    }
}
#endif

/* 创建事件标志组 */
static void AppObjCreate (void)
{
    /* 创建事件标志组 */
    xCreatedEventGroup = xEventGroupCreate();
    
    if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
		USART_PRINTF_S("\r\ncreate event failure!\n");
    }
}

/* 喂狗任务 */
static void vTaskFeedDog(void *pvParameters)
{
	static uint16_t Se_w_Timer = 0U;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 150 / portTICK_PERIOD_MS; /* 最大延迟150ms */
	
    IWDG_Config(IWDG_Prescaler_256 ,100);//独立看门狗初始化，50-->320ms
    while(1)
    {
        /* 等待所有任务发来事件标志 */
        uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
                                     All_bits_Task,       /* 等待TASK_BIT_ALL被设置 */
                                     pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
                                     pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
                                     xTicksToWait);      /* 等待延迟时间 */
        
        if((uxBits & All_bits_Task) == All_bits_Task)
        {
            IWDG_Feed();
			Se_w_Timer++;
			if(Se_w_Timer >= 200U)
			{
				Se_w_Timer = 0U;
				USART_PRINTF_S("\r\n用户任务都正常运行\r\n");
			}
        }
        else
        {
            /* 基本是每xTicksToWait进来一次 */
            /* 通过变量uxBits简单的可以在此处检测那个任务长期没有发来运行标志 */			
			if((uxBits & LwipApp5ms_bits_Task) != LwipApp5ms_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：Lwip app 5ms任务未正常运行\r\n");
			}
			
			if((uxBits & LwipApp100ms_bits_Task) != LwipApp100ms_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：Lwip app 100ms任务未正常运行\r\n");
			}
			
			if((uxBits & iap_bits_Task) != iap_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：在线更新任务未正常运行\r\n");
			}	
			
			if((uxBits & Fliter_bits_Task) != Fliter_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：滤波任务未正常运行\r\n");
			}
			
			if((uxBits & ZR60Ctrl5ms_bits_Task) != ZR60Ctrl5ms_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：ZR60 5ms任务未正常运行\r\n");
			}
			
			if((uxBits & ZR60Ctrl100ms_bits_Task) != ZR60Ctrl100ms_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：ZR60 100ms任务未正常运行\r\n");
			}
			
			if((uxBits & SleepMng_bits_Task) != SleepMng_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging：低功耗任务未正常运行\r\n");
			}
        }
    }
}


/*******************END OF FILE******************/

