#ifndef __RTC_H__
#define __RTC_H__
#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_rcc.h"
#include "./usart/bsp_debug_usart.h"
#include "./hym8563/hym8563.h"

// ʱ��Դ�궨��
//#define RTC_CLOCK_SOURCE_LSE      
#define RTC_CLOCK_SOURCE_LSI

// �첽��Ƶ����
#define ASYNCHPREDIV         0X74
// ͬ����Ƶ����
#define SYNCHPREDIV          0XFF

// ʱ��궨��
#define RTC_H12_AMorPM		 RTC_H12_AM

// ʱ���ʽ�궨��
#define RTC_Format_BINorBCD  RTC_Format_BIN

// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR0
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0X32F2

                              
extern void RTC_CLK_Config(void);
extern void Set_RTC_TimeAndDate(struct rtc_time* tm);
extern void Read_RTC_TimeAndDate(struct rtc_time* tm);
extern void RTC_CLK_Config_Backup(void);
extern void RTC_Currenttime(uint64_t* time);
#endif // __RTC_H__
