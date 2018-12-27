/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author 
  * @version V1.0
  * @date   
  * @brief   RTC����
  ******************************************************************************
  * @attention
  *
  * 
  * 
  * 
  *
  ******************************************************************************
  */
#include "./rtc/bsp_rtc.h" 

/**
  * @brief  ����ʱ�������
  * @param  ��
  * @retval ��
  */
void Set_RTC_TimeAndDate(struct rtc_time* tm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// ��ʼ��ʱ��
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;//AM--����24Сʱ��;PM--����12Сʱ��
	RTC_TimeStructure.RTC_Hours = (uint8_t)(tm->tm_hour);//����24Сʱ��ʱ��0-11;����12Сʱ��ʱ��0-23     
	RTC_TimeStructure.RTC_Minutes = (uint8_t)(tm->tm_min);      
	RTC_TimeStructure.RTC_Seconds = (uint8_t)(tm->tm_sec);      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	//RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
    // ��ʼ������	
	RTC_DateStructure.RTC_WeekDay = 0;       
	RTC_DateStructure.RTC_Date = (uint8_t)(tm->tm_mday);         
	RTC_DateStructure.RTC_Month = (uint8_t)(tm->tm_mon);         
	RTC_DateStructure.RTC_Year = (uint8_t)(tm->tm_year-2000);      
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
#ifdef UART_DEBUG	
	// ��ӡ����
	printf("Set The Date :  20%0.2d.%0.2d.%0.2d ����%0.2d\r\n", 
	RTC_DateStructure.RTC_Year,
	RTC_DateStructure.RTC_Month, 
	RTC_DateStructure.RTC_Date,
	RTC_DateStructure.RTC_WeekDay);
	// ��ӡʱ��
	printf("Set The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", 
	RTC_TimeStructure.RTC_Hours, 
	RTC_TimeStructure.RTC_Minutes, 
	RTC_TimeStructure.RTC_Seconds);
#endif
}

/**
  * @brief  ��ʾʱ�������
  * @param  ��
  * @retval ��
  */
void Read_RTC_TimeAndDate(struct rtc_time* tm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	// ��ȡ���ڡ�ʱ��
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
				
	// ��ӡ����
	//printf("The Date :  20%0.2d.%0.2d.%0.2d ����%0.2d\r\n", \
	RTC_DateStructure.RTC_Year,	\
	RTC_DateStructure.RTC_Month, \
	RTC_DateStructure.RTC_Date,	\
	RTC_DateStructure.RTC_WeekDay);
	// ��ӡʱ��
	//printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", \
	RTC_TimeStructure.RTC_Hours, \
	RTC_TimeStructure.RTC_Minutes, \
	RTC_TimeStructure.RTC_Seconds);
	
	tm->tm_sec = (int)RTC_TimeStructure.RTC_Seconds;
	tm->tm_min = (int)RTC_TimeStructure.RTC_Minutes;
	tm->tm_hour = (int)RTC_TimeStructure.RTC_Hours;
	tm->tm_mday = (int)RTC_DateStructure.RTC_Date;
	tm->tm_wday = (int)RTC_DateStructure.RTC_WeekDay;	
	tm->tm_mon = (int)RTC_DateStructure.RTC_Month;
	tm->tm_year = (int)(RTC_DateStructure.RTC_Year + 2000);

	(void)RTC->DR;
}


/**
���ܣ���ȡ����ʱ�䣬��ʽ��201806215959
������
**/
void RTC_Currenttime(uint64_t* time)
{
	struct rtc_time Le_h_tm;
	Read_RTC_TimeAndDate(&Le_h_tm);//��ȡ��ǰʱ��
	//tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	*time = (uint64_t)(Le_h_tm.tm_year)*10000000000 + (uint64_t)(Le_h_tm.tm_mon)*100000000 + \
			(uint64_t)(Le_h_tm.tm_mday)*1000000 + (uint64_t)(Le_h_tm.tm_hour)*10000 + \
			(uint64_t)(Le_h_tm.tm_min)*100 + (uint64_t)Le_h_tm.tm_sec;
}


/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*ʹ�� PWR ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* ʹ��LSI��ΪRTCʱ��Դ������� 
	 * Ĭ��ѡ��LSE��ΪRTC��ʱ��Դ
	 */
	/* ʹ��LSI */ 
	RCC_LSICmd(ENABLE);
	/* �ȴ�LSI�ȶ� */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
		
	}
	/* ѡ��LSI��ΪRTC��ʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* ʹ��LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* �ȴ�LSE�ȶ� */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* ѡ��LSE��ΪRTC��ʱ��Դ */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

	/* ʹ��RTCʱ�� */
	RCC_RTCCLKCmd(ENABLE);

	/* �ȴ� RTC APB �Ĵ���ͬ�� */
	RTC_WaitForSynchro();
   
/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	/* ����������ʱ��ck_spare = LSI/[(255+1)*(116+1)] = 1HZ */
	
	/* �����첽Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* ����ͬ��Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
	}
	else
	{
		printf("\n\r RTC ʱ�ӳ�ʼ���ɹ� \r\n");
	}
	
}

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
void RTC_CLK_Config_Backup(void)
{  
	__IO uint16_t StartUpCounter = 0;
	FlagStatus LSEStatus = RESET;	
	RTC_InitTypeDef RTC_InitStructure;
	
	/* ʹ�� PWR ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	PWR_BackupAccessCmd(ENABLE);
	
/*=========================ѡ��RTCʱ��Դ==============================*/
	/* Ĭ��ʹ��LSE�����LSE��������ʹ��LSI */
	/* ʹ��LSE */
	RCC_LSEConfig(RCC_LSE_ON);	
	
	/* �ȴ�LSE�����ȶ��������ʱ���˳� */
	do
	{
		LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
		StartUpCounter++;
	}while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));
	
	if(LSEStatus == SET )
	{
		printf("\n\r LSE �����ɹ� \r\n");
		/* ѡ��LSE��ΪRTC��ʱ��Դ */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		/* �����첽Ԥ��Ƶ����ֵΪ127 */
		//RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	}
	else
	{
		printf("\n\r LSE ���ϣ�תΪʹ��LSI \r\n");	
		/* ʹ��LSI */	
		RCC_LSICmd(ENABLE);
		/* �ȴ�LSI�ȶ� */ 
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{}	
		printf("\n\r LSI �����ɹ� \r\n");
		/* ѡ��LSI��ΪRTC��ʱ��Դ */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		/* �����첽Ԥ��Ƶ����ֵΪ116 */
		//RTC_InitStructure.RTC_AsynchPrediv = 0x75;
	}
	
	/* ʹ�� RTC ʱ�� */
	RCC_RTCCLKCmd(ENABLE);
	/* �ȴ� RTC APB �Ĵ���ͬ�� */
	RTC_WaitForSynchro();

/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* �����첽Ԥ��Ƶ����ֵΪ127 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	/* ����ͬ��Ԥ��Ƶ����ֵΪ255 */
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	StartUpCounter = 3U;
	while(StartUpCounter--)
	{
		if(RTC_Init(&RTC_InitStructure) == ERROR)
		{
			printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
		}
		else
		{
			printf("\n\r RTC ʱ�ӳ�ʼ����� \r\n");
			break;
		}
	}
}


/**********************************END OF FILE*************************************/
