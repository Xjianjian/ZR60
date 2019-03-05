#ifndef _HYM8563_H
#define _HYM8563_H

#include "LocalRTC/LocalRTC_Cfg.h"


/* iic外设hym8563地址 */
#define HYM8563_ADDRESS 	0XA2


#define   RTC_CTL1		0x00
#define   RTC_CTL2		0x01
#define   RTC_SEC		0x02//秒寄存器地址
#define   RTC_MIN		0x03
#define   RTC_HOUR		0x04
#define   RTC_DAY		0x05
#define   RTC_WEEK		0x06
#define   RTC_MON		0x07
#define   RTC_YEAR		0x08
#define   RTC_A_MIN 	0x09
#define   RTC_A_HOUR	0x0A
#define   RTC_A_DAY 	0x0B
#define   RTC_A_WEEK	0x0C
#define   RTC_CLKOUT	0x0D
#define   RTC_T_CTL 	0x0E
#define   RTC_T_COUNT	0x0F
#define   CENTURY	0x80
#define   STOP		0x20
#define   TI		0x10
#define   AF		0x08
#define   TF		0x04
#define   AIE		0x02
#define   TIE		0x01
#define   FE		0x80
#define   TE		0x80
#define   FD1		0x02
#define   FD0		0x01
#define   TD1		0x02
#define   TD0		0x01
#define   VL		0x80

#define HYM8563_REG_LEN 	0x10
#define HYM8563_RTC_SECTION_LEN	0x07

extern void Inithym8563_device(void);
extern uint8_t bin2bcd(uint8_t val);
extern uint8_t hym8563_set_time(struct rtc_time *tm);
extern void hym8563_Currenttime(uint64_t* time);
extern uint8_t hym8563_read_datetime(struct rtc_time *tm);
extern uint8_t hym8563_i2c_set_regs(u8 reg, u8 *buf, u8 len);
extern uint8_t hym8563_i2c_read_regs(u8 regAddr, u8 *buf, u8 len);

#endif

