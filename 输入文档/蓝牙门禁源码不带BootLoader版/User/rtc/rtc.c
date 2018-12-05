

#include "./rtc/rtc.h"

#include "./i2c/bsp_i2c_ee.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "./usart/bsp_debug_usart.h"


/*正常月份天数*/
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

static uint8_t is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}


/*
 * The number of days in the month.
 */
int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

int rtc_year_days(unsigned int day, unsigned int month, unsigned int year)
{
	return rtc_ydays[is_leap_year(year)][month] + day-1;
}
/*
十进制转同值的16进制**
如12--0x12
*/
uint8_t bin2bcd(uint8_t val)
{
    return ((val / 10) << 4) + val % 10;
}
/*
十六进制转同值的10进制**
如0x12--12
*/
uint8_t bcd2bin(uint8_t val)
{
    return (val & 0x0f) + (val >> 4) * 10;
}

/**
根据年月日计算是星期几
**/
uint8_t CaculateWeekDay(int y,int m, int d)
{
	uint8_t iWeek;
    if(m==1||m==2) {
        m+=12;
        y--;
    }
   iWeek=((d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)+1)%7;
		
		return iWeek;

} 
/**
功能：设置hym8563的寄存器值
参数：
ucDevAddr：设备地址
reg：起始寄存器值
buf：储存的数据
len：写入长度
**/
uint8_t hym8563_i2c_set_regs(u8 ucDevAddr, u8 reg, u8 *buf, u8 len)
{
	  u8 ret;
		ret = i2c_master_reg8_send(ucDevAddr,reg,buf,len);
		return ret;
}
/**
功能：读取hym8563的寄存器值
参数：
ucDevAddr：设备地址
reg：起始寄存器值
buf：储存的数组
len：写入长度
**/
uint8_t hym8563_i2c_read_regs(u8 ucDevAddr, u8 reg, u8 *buf, u8 len)
{
	  u8 ret;
		ret = i2c_master_reg8_recv(ucDevAddr,reg,buf,len);
		return ret;
}
/**
功能：读取hym8563的日期时间
参数：
ucDevAddr：设备地址
tm：读取的时间存储在结构体
**/


uint8_t hym8563_read_datetime(u8 ucDevAddr, struct rtc_time *tm)
{

	u8 i,ret,regs[HYM8563_RTC_SECTION_LEN] = { 0};


	 ret = hym8563_i2c_read_regs(ucDevAddr, RTC_SEC, regs, HYM8563_RTC_SECTION_LEN);
#ifdef TIME_DEBUG
	/*
	printf("regs is ");
	for(i = 0;i < HYM8563_RTC_SECTION_LEN;i++)
	{
	printf(" %#x",regs[i]);
	}
		printf("\n");
	*/
#endif	
	tm->tm_sec = bcd2bin(regs[0x00] & 0x7F);
	tm->tm_min = bcd2bin(regs[0x01] & 0x7F);
	tm->tm_hour = bcd2bin(regs[0x02] & 0x3F);
	tm->tm_mday = bcd2bin(regs[0x03] & 0x3F);
	tm->tm_wday = bcd2bin(regs[0x04] & 0x07);	
	
	tm->tm_mon = bcd2bin(regs[0x05] & 0x1F) +1 ; 
	//tm->tm_mon -= 1;			//inorder to cooperate the systerm time
	
	tm->tm_year = bcd2bin(regs[0x06] & 0xFF);
	if(regs[5] & 0x80)
		tm->tm_year += 1900;
	else
		tm->tm_year += 2000;
		
	tm->tm_yday = rtc_year_days(tm->tm_mday, tm->tm_mon, tm->tm_year);	
//	tm->tm_year -= 1900;			//inorder to cooperate the systerm time	
	if( (tm->tm_year - 1900) < 0)
		tm->tm_year = 0;	
	tm->tm_isdst = 0;	

	
	
#ifdef  TIME_DEBUG
	/*
	printf("%4d-%02d-%02d(%d) %02d:%02d:%02d\n",
		tm->tm_year, tm->tm_mon + 1, tm->tm_mday, tm->tm_wday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);*/
#endif
	return ret;
}

/**
功能：向hym8563写入日期时间
参数：
ucDevAddr：设备地址
tm：存储的时间日期
**/


 uint8_t hym8563_set_time(u8 ucDevAddr, struct rtc_time *tm)	
{
	uint8_t regs[HYM8563_RTC_SECTION_LEN] = {0};
	
	
	u8 mon_day,i;
	u8 ret = 0;

#ifdef  TIME_DEBUG
/*
	printf("set tie%4d-%02d-%02d(%d) %02d:%02d:%02d\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);*/
#endif
	mon_day = rtc_month_days((tm->tm_mon), tm->tm_year);
	tm->tm_wday = CaculateWeekDay(tm->tm_year,tm->tm_mon+1,tm->tm_mday);
#ifdef  TIME_DEBUG	
	//	printf("week is %d\n",
	//	 tm->tm_wday);
#endif	
	if(tm->tm_sec >= 60 || tm->tm_sec < 0 )		//set  sec
		regs[0x00] = bin2bcd(0x00);
	else
		regs[0x00] = bin2bcd(tm->tm_sec);
	
	if(tm->tm_min >= 60 || tm->tm_min < 0 )		//set  min	
		regs[0x01] = bin2bcd(0x00);
	else
		regs[0x01] = bin2bcd(tm->tm_min);

	if(tm->tm_hour >= 24 || tm->tm_hour < 0 )		//set  hour
		regs[0x02] = bin2bcd(0x00);
	else
		regs[0x02] = bin2bcd(tm->tm_hour);
	
	if((tm->tm_mday) > mon_day)				//if the input month day is bigger than the biggest day of this month, set the biggest day 
		regs[0x03] = bin2bcd(mon_day);
	else if((tm->tm_mday) > 0)
		regs[0x03] = bin2bcd(tm->tm_mday);
	else if((tm->tm_mday) <= 0)
		regs[0x03] = bin2bcd(0x01);

	if( tm->tm_year >= 2100)		// year >= 2100
		regs[0x06] = bin2bcd(99);	//year = 2099
	else if(tm->tm_year >= 2000)			// 2000 <= year < 2100
		regs[0x06] = bin2bcd(tm->tm_year - 2000);
	else if(tm->tm_year >= 0){				// 1900 <= year < 2000
		regs[0x06] = bin2bcd(tm->tm_year - 1900 );	
		regs[0x05] |= 0x80;	
	}else{									// year < 1900
		regs[0x06] = bin2bcd(0);	//year = 1900	
		regs[0x05] |= 0x80;	
	}	
	
	
	regs[0x04] = bin2bcd(tm->tm_wday);		//set  the  weekday
	regs[0x05] = (regs[0x05] & 0x80)| (bin2bcd(tm->tm_mon) & 0x7F);		//set  the  month
#if  TIME_DEBUG
/*	printf("regs is ");
	for(i = 0;i < HYM8563_RTC_SECTION_LEN;i++)
	{
	printf(" %#x",regs[i]);
	}
	printf("\n");*/
#endif
	
	ret = hym8563_i2c_set_regs(ucDevAddr,RTC_SEC,regs,HYM8563_RTC_SECTION_LEN);
	
	
	
	return ret;
}

