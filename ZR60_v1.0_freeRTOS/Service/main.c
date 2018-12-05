/************************************************
  * @file    main.c
  * @author  liujian
  * @version V1.0
  * @date    2018-3-28
  * @brief
  ***********************************************
  * @attention
  *
  * ƽ̨:����  STM32 F407 ������  
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
  *        				�궨��
**************************************************/
//#define RunSt_bits_Task  		(1 << 0)
#define LwipApp5ms_bits_Task  	(1 << 1)
#define SleepMng_bits_Task 		(1 << 7)
#define All_bits_Task  			(LwipApp5ms_bits_Task | SleepMng_bits_Task)
static EventGroupHandle_t  		xCreatedEventGroup;

/*************************************************
  *        				�������
**************************************************/

/*************************************************
  *        				���徲̬����
**************************************************/
static void AppTaskCreate(void);
static void Lwip_app_5msTask(void * pvParameters);
#ifdef freeRTOS_RUN_STATUS_DEBUG
static void vTaskRunningStatus(void *pvParameters);
#endif
static void AppObjCreate (void);
static void vTaskFeedDog(void *pvParameters);
static void LowPower_Task(void * pvParameters);
/*************************************************
  *        				main����
**************************************************/
int main(void)
{
	//SCB->VTOR = FLASH_BASE | 0x40000;//�ж�������ƫ��
	Mcu_Init();/* ��ʼ�� */
	
	/* Enable RNG clock source *///polarSSL��Ҫʹ�õ���δʹ�ÿ�ע�͵�
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	/* Enable RNG IP  */
	RNG_Cmd(ENABLE);
	
	AppObjCreate(); /* �����¼���־�� */
	AppTaskCreate();/* �������� */
	
	while(1)
	{}
}


/* �������� */
static void AppTaskCreate(void)
{
#ifdef freeRTOS_RUN_STATUS_DEBUG
	xTaskCreate(vTaskRunningStatus, (const char *)"RunningStatus", 	1024 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 1, STD_NULL_PTR);
#endif
	xTaskCreate(LowPower_Task, (const char *)"LowPower", 	256 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 2, STD_NULL_PTR);
	xTaskCreate(Lwip_app_5msTask, 	(const char *)"Lwip_app_5ms", 	512 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 4, STD_NULL_PTR);
	xTaskCreate(vTaskFeedDog,		(const char *)"FeedDog", 		128 , \
				STD_NULL_PTR, tskIDLE_PRIORITY + 5,STD_NULL_PTR);
	
	vTaskStartScheduler();//����������
}

static void Lwip_app_5msTask(void * pvParameters)
{
	static uint64_t Se_dw_LocalTime = 0U;
	while(1)
	{	
		TskdhcpClient_MainFunction();
	
		/*********************************
		���½ӿ���Ҫ���ڵ���
		*********************************/
		/* handle periodic timers for LwIP */
		Se_dw_LocalTime = Se_dw_LocalTime + 5U;
		LwIP_Periodic_Handle(Se_dw_LocalTime);
		
		xEventGroupSetBits(xCreatedEventGroup,LwipApp5ms_bits_Task);
		vTaskDelay(5 / portTICK_RATE_MS);
	}
}


static void LowPower_Task(void * pvParameters)
{
	while(1)
	{
		TskSleepMng_MainFunction();	
		
		xEventGroupSetBits(xCreatedEventGroup, SleepMng_bits_Task);
		vTaskDelay(20 / portTICK_RATE_MS);
	}
}

#ifdef freeRTOS_RUN_STATUS_DEBUG
/* ����״̬ͳ�� */
static void vTaskRunningStatus(void *pvParameters)
{
    uint8_t pcWriteBuffer[700];

    while(1)
    {
		printf("\r\n������         ����״̬ ���ȼ� ʣ��ջ   �������\r\n");
		vTaskList((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
		
		printf("\r\n������          ���м���        ʹ����\r\n");
		vTaskGetRunTimeStats((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
		
		//xEventGroupSetBits(xCreatedEventGroup, RunSt_bits_Task);
        IWDG_Feed();//ι��
        vTaskDelay(30000);
    }
}
#endif

/* �����¼���־�� */
static void AppObjCreate (void)
{
    /* �����¼���־�� */
    xCreatedEventGroup = xEventGroupCreate();
    
    if(xCreatedEventGroup == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		USART_PRINTF_S("\r\ncreate event failure!\n");
    }
}

/* ι������ */
static void vTaskFeedDog(void *pvParameters)
{
	static uint16_t Se_w_Timer = 0U;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 150 / portTICK_PERIOD_MS; /* ����ӳ�150ms */
	
    IWDG_Config(IWDG_Prescaler_256 ,100);//�������Ź���ʼ����50-->320ms
    while(1)
    {
        /* �ȴ������������¼���־ */
        uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
                                     All_bits_Task,       /* �ȴ�TASK_BIT_ALL������ */
                                     pdTRUE,             /* �˳�ǰTASK_BIT_ALL�������������TASK_BIT_ALL�������òű�ʾ���˳���*/
                                     pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�TASK_BIT_ALL��������*/
                                     xTicksToWait);      /* �ȴ��ӳ�ʱ�� */
        
        if((uxBits & All_bits_Task) == All_bits_Task)
        {
            IWDG_Feed();
			Se_w_Timer++;
			if(Se_w_Timer >= 200U)
			{
				Se_w_Timer = 0U;
				USART_PRINTF_S("\r\n�û�������������\r\n");
			}
        }
        else
        {
            /* ������ÿxTicksToWait����һ�� */
            /* ͨ������uxBits�򵥵Ŀ����ڴ˴�����Ǹ�������û�з������б�־ */			
			if((uxBits & LwipApp5ms_bits_Task) != LwipApp5ms_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging��Lwip app 5ms����δ��������\r\n");
			}
			
			if((uxBits & SleepMng_bits_Task) != SleepMng_bits_Task)
			{
				USART_PRINTF_S("\r\nErrorLogging���͹�������δ��������\r\n");
			}
        }
    }
}


/*******************END OF FILE******************/

