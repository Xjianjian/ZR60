

#ifndef __TIME_STAMP_H__
#define __TIME_STAMP_H__

#include "stm32f4xx.h"
#include "./rtc/rtc.h"

#include "./wdata/wdata.h"



unsigned long GetTick(struct rtc_time *dtm);
//uint8_t sm3_time_test(unsigned long now_time,struct comm_info *sm,u8 n);
uint32_t  check_passwd(u32 type_value,u8 mach_type);
//uint8_t sm3_time_test(unsigned long now_time,struct comm_info *sm,u8 n,u32 passwd);
uint8_t sm3_time_test(unsigned long now_time,struct sm3_info *sm,u8 n,u8 mach_type);

void check_time(uint8_t *tm);  
#endif