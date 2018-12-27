/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author 
  * @version V1.0
  * @date   
  * @brief   RTC驱动
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
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void Set_RTC_TimeAndDate(struct rtc_time* tm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;//AM--采用24小时制;PM--采用12小时制
	RTC_TimeStructure.RTC_Hours = (uint8_t)(tm->tm_hour);//采用24小时制时，0-11;采用12小时制时，0-23     
	RTC_TimeStructure.RTC_Minutes = (uint8_t)(tm->tm_min);      
	RTC_TimeStructure.RTC_Seconds = (uint8_t)(tm->tm_sec);      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	//RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
    // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = 0;       
	RTC_DateStructure.RTC_Date = (uint8_t)(tm->tm_mday);         
	RTC_DateStructure.RTC_Month = (uint8_t)(tm->tm_mon);         
	RTC_DateStructure.RTC_Year = (uint8_t)(tm->tm_year-2000);      
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
#ifdef UART_DEBUG	
	// 打印日期
	printf("Set The Date :  20%0.2d.%0.2d.%0.2d 星期%0.2d\r\n", 
	RTC_DateStructure.RTC_Year,
	RTC_DateStructure.RTC_Month, 
	RTC_DateStructure.RTC_Date,
	RTC_DateStructure.RTC_WeekDay);
	// 打印时间
	printf("Set The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", 
	RTC_TimeStructure.RTC_Hours, 
	RTC_TimeStructure.RTC_Minutes, 
	RTC_TimeStructure.RTC_Seconds);
#endif
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void Read_RTC_TimeAndDate(struct rtc_time* tm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	// 获取日期、时间
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
				
	// 打印日期
	//printf("The Date :  20%0.2d.%0.2d.%0.2d 星期%0.2d\r\n", \
	RTC_DateStructure.RTC_Year,	\
	RTC_DateStructure.RTC_Month, \
	RTC_DateStructure.RTC_Date,	\
	RTC_DateStructure.RTC_WeekDay);
	// 打印时间
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
功能：读取日期时间，格式：201806215959
参数：
**/
void RTC_Currenttime(uint64_t* time)
{
	struct rtc_time Le_h_tm;
	Read_RTC_TimeAndDate(&Le_h_tm);//获取当前时间
	//tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	*time = (uint64_t)(Le_h_tm.tm_year)*10000000000 + (uint64_t)(Le_h_tm.tm_mon)*100000000 + \
			(uint64_t)(Le_h_tm.tm_mday)*1000000 + (uint64_t)(Le_h_tm.tm_hour)*10000 + \
			(uint64_t)(Le_h_tm.tm_min)*100 + (uint64_t)Le_h_tm.tm_sec;
}


/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*使能 PWR 时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* 使用LSI作为RTC时钟源会有误差 
	 * 默认选择LSE作为RTC的时钟源
	 */
	/* 使能LSI */ 
	RCC_LSICmd(ENABLE);
	/* 等待LSI稳定 */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
		
	}
	/* 选择LSI做为RTC的时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* 使能LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* 等待LSE稳定 */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* 选择LSE做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

	/* 使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);

	/* 等待 RTC APB 寄存器同步 */
	RTC_WaitForSynchro();
   
/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	/* 驱动日历的时钟ck_spare = LSI/[(255+1)*(116+1)] = 1HZ */
	
	/* 设置异步预分频器的值 */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}
	else
	{
		printf("\n\r RTC 时钟初始化成功 \r\n");
	}
	
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
void RTC_CLK_Config_Backup(void)
{  
	__IO uint16_t StartUpCounter = 0;
	FlagStatus LSEStatus = RESET;	
	RTC_InitTypeDef RTC_InitStructure;
	
	/* 使能 PWR 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	PWR_BackupAccessCmd(ENABLE);
	
/*=========================选择RTC时钟源==============================*/
	/* 默认使用LSE，如果LSE出故障则使用LSI */
	/* 使能LSE */
	RCC_LSEConfig(RCC_LSE_ON);	
	
	/* 等待LSE启动稳定，如果超时则退出 */
	do
	{
		LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
		StartUpCounter++;
	}while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));
	
	if(LSEStatus == SET )
	{
		printf("\n\r LSE 启动成功 \r\n");
		/* 选择LSE作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		/* 设置异步预分频器的值为127 */
		//RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	}
	else
	{
		printf("\n\r LSE 故障，转为使用LSI \r\n");	
		/* 使能LSI */	
		RCC_LSICmd(ENABLE);
		/* 等待LSI稳定 */ 
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{}	
		printf("\n\r LSI 启动成功 \r\n");
		/* 选择LSI作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		/* 设置异步预分频器的值为116 */
		//RTC_InitStructure.RTC_AsynchPrediv = 0x75;
	}
	
	/* 使能 RTC 时钟 */
	RCC_RTCCLKCmd(ENABLE);
	/* 等待 RTC APB 寄存器同步 */
	RTC_WaitForSynchro();

/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值为127 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	/* 设置同步预分频器的值为255 */
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	StartUpCounter = 3U;
	while(StartUpCounter--)
	{
		if(RTC_Init(&RTC_InitStructure) == ERROR)
		{
			printf("\n\r RTC 时钟初始化失败 \r\n");
		}
		else
		{
			printf("\n\r RTC 时钟初始化完成 \r\n");
			break;
		}
	}
}


/**********************************END OF FILE*************************************/
