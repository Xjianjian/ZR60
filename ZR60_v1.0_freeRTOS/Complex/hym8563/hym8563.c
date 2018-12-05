/******************************************************
�ļ�����	hym8563.c

������		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "./hym8563/hym8563.h"
//#include "./i2c/bsp_i2c_ee.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "./usart/bsp_debug_usart.h"


/*�����·�����*/
static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const unsigned short rtc_ydays[2][13] = {
	/* Normal years */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* Leap years */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)

/*the init of the hym8563 at first time */
void Inithym8563_device(void)   
{
    i2c_CfgGpio(IIC_HYM8563,I2C_TYPE_HARDWARE);//iic gpio�����ã�hym8563����Ӳ��iic
	I2C_Mode_Config(IIC_HYM8563);//����iic����ģʽ
}



static uint8_t is_leap_year(unsigned int year)
{
	return ((!(year % 4) && (year % 100)) || !(year % 400));
}


/*
 * The number of days in the month.
 */
int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month-1] + (is_leap_year(year) && month == 2);
}

int rtc_year_days(unsigned int day, unsigned int month, unsigned int year)
{
	return rtc_ydays[is_leap_year(year)][month] + day-1;
}

/*
ʮ����תֵͬ��16����**
��12--0x12
*/
uint8_t bin2bcd(uint8_t val)
{
    return ((val / 10) << 4) + val % 10;
}

/*
ʮ������תֵͬ��10����**
��0x12--12
*/
uint8_t bcd2bin(uint8_t val)
{
    return (val & 0x0f) + (val >> 4) * 10;
}

/**
���������ռ��������ڼ�
**/
uint8_t CaculateWeekDay(int y,int m, int d)
{
	uint8_t iWeek;
    if(m==1||m==2) 
	{
        m+=12;
        y--;
    }
	iWeek=((d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)+1)%7;
		
	return iWeek;
} 

/**
���ܣ�����hym8563�ļĴ���ֵ
������
ucDevAddr���豸��ַ
reg����ʼ�Ĵ�����ַ
buf�����������
len��д�볤��
**/
uint8_t hym8563_i2c_set_regs(u8 reg, u8 *buf, u8 len)
{
	u8 ret;
	
	Inithym8563_device();
	ret = I2C_BytesWrite(IIC_HYM8563,reg,buf,len);
	return ret;
}

/**
���ܣ���ȡhym8563�ļĴ���ֵ
������
ucDevAddr���豸��ַ
reg����ʼ�Ĵ�����ַ
buf�����������
len��д�볤��
**/
uint8_t hym8563_i2c_read_regs(u8 regAddr, u8 *buf, u8 len)
{
	u8 ret;
	
	Inithym8563_device();
	ret = I2C_BytesRead(IIC_HYM8563,regAddr,buf,len);
	return ret;
}


/**
���ܣ���ȡhym8563������ʱ�䣬��ʽ��201806215959
������
**/
void hym8563_Currenttime(uint64_t* time)
{
	struct rtc_time Le_h_tm;
	(void)hym8563_read_datetime(&Le_h_tm);//��ȡ��ǰʱ��
	//tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	*time = (uint64_t)(Le_h_tm.tm_year)*10000000000 + (uint64_t)(Le_h_tm.tm_mon)*100000000 + \
			(uint64_t)(Le_h_tm.tm_mday)*1000000 + (uint64_t)(Le_h_tm.tm_hour)*10000 + \
			(uint64_t)(Le_h_tm.tm_min)*100 + (uint64_t)Le_h_tm.tm_sec;
}

/**
���ܣ���ȡhym8563������ʱ��
������
tm����ȡ��ʱ��洢�ڽṹ��
**/
uint8_t hym8563_read_datetime(struct rtc_time *tm)
{
	u8 ret;
	u8 regsData[HYM8563_RTC_SECTION_LEN] = {0};//����������ʱ�������
	u8 cnt=3U;
	do
	{
		ret = hym8563_i2c_read_regs(RTC_SEC, regsData, HYM8563_RTC_SECTION_LEN);//��ȡ����ʱ�䣬���ݼĴ�����ʼ��ַRTC_SEC

		tm->tm_sec = bcd2bin(regsData[0x00] & 0x7F);
		tm->tm_min = bcd2bin(regsData[0x01] & 0x7F);
		tm->tm_hour = bcd2bin(regsData[0x02] & 0x3F);
		tm->tm_mday = bcd2bin(regsData[0x03] & 0x3F);
		tm->tm_wday = bcd2bin(regsData[0x04] & 0x07);	
		
		tm->tm_mon = bcd2bin(regsData[0x05] & 0x1F); 
		
		tm->tm_year = bcd2bin(regsData[0x06] & 0xFF);
		if(regsData[5] & 0x80)
			tm->tm_year += 1900;
		else
			tm->tm_year += 2000;
			
		tm->tm_yday = rtc_year_days(tm->tm_mday, tm->tm_mon, tm->tm_year);	
		
		if( (tm->tm_year - 1900) < 0)
			tm->tm_year = 0;	
		tm->tm_isdst = 0;	
	}while((tm->tm_year < 2018) && (cnt--));
	return ret;
}

/**
���ܣ���hym8563д������ʱ��
������
tm���洢��ʱ������
**/
uint8_t hym8563_set_time(struct rtc_time *tm)	
{
	uint8_t regs[HYM8563_RTC_SECTION_LEN] = {0};
	u8 mon_day;
	u8 ret = 0;

	USART_PRINTF_D("������ %d��\n",tm->tm_mday);
	mon_day = rtc_month_days((tm->tm_mon), tm->tm_year);
	USART_PRINTF_D("����������� %d��\n",mon_day);
	tm->tm_wday = CaculateWeekDay(tm->tm_year,tm->tm_mon,tm->tm_mday);
	USART_PRINTF_D("�������� %d\n",tm->tm_wday);
	if(tm->tm_sec >= 60 || tm->tm_sec < 0 )		//set  sec
	{
		regs[0x00] = bin2bcd(0x00);
	}
	else
	{
		regs[0x00] = bin2bcd(tm->tm_sec);
	}
	
	if(tm->tm_min >= 60 || tm->tm_min < 0 )		//set  min	
	{
		regs[0x01] = bin2bcd(0x00);
	}
	else
	{
		regs[0x01] = bin2bcd(tm->tm_min);
	}

	if(tm->tm_hour >= 24 || tm->tm_hour < 0 )		//set  hour
	{
		regs[0x02] = bin2bcd(0x00);
	}
	else
	{
		regs[0x02] = bin2bcd(tm->tm_hour);
	}
	
	if((tm->tm_mday) > mon_day)				//if the input month day is bigger than the biggest day of this month, set the biggest day 
	{
		regs[0x03] = bin2bcd(mon_day);
	}
	else if((tm->tm_mday) > 0)
	{
		regs[0x03] = bin2bcd(tm->tm_mday);
	}
	else
	{
		regs[0x03] = bin2bcd(0x01);
	}

	if( tm->tm_year >= 2100)		// year >= 2100
	{
		regs[0x06] = bin2bcd(99);	//year = 2099
	}
	else if(tm->tm_year >= 2000)			// 2000 <= year < 2100
	{
		regs[0x06] = bin2bcd(tm->tm_year - 2000);
	}
	else if(tm->tm_year >= 1900)
	{				// 1900 <= year < 2000
		regs[0x06] = bin2bcd(tm->tm_year - 1900 );	
		regs[0x05] |= 0x80;	
	}
	else
	{									// year < 1900
		regs[0x06] = bin2bcd(0);	//year = 1900	
		regs[0x05] |= 0x80;	
	}	
	
	regs[0x04] = bin2bcd(tm->tm_wday);		//set  the  weekday
	regs[0x05] = (regs[0x05] & 0x80)| (bin2bcd(tm->tm_mon) & 0x7F);		//set  the  month
	
	ret = hym8563_i2c_set_regs(RTC_SEC,regs,HYM8563_RTC_SECTION_LEN);//д����ʱ�䣬���ݼĴ�����ʼ��ַRTC_SEC
	
	return ret;
}

