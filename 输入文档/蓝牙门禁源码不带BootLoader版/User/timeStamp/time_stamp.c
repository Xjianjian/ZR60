

#include "./timeStamp/time_stamp.h"
#include "./sm3/SM3.h"
#include "time.h"
#include "./led/bsp_led.h"  
#include "./systick/bsp_SysTick.h"
#include "wm8978/bsp_wm8978.h"
#include "./malloc/malloc.h"

#include "./wdata/wdata.h"
#include "./beep/bsp_beep.h" 
#include  "math.h"

u8 key_buf[6];
extern char BleValue[6];
extern unsigned long now_time;
//extern uint8_t timelist[14];
unsigned long GetTick(struct rtc_time *tm)  
{  
    struct tm stm;  

  
	

    stm.tm_year = tm->tm_year - 1900 ;  
    stm.tm_mon = tm->tm_mon - 1;  
    stm.tm_mday = tm->tm_mday;  
    stm.tm_hour = tm->tm_hour - 8;  
    stm.tm_min = tm->tm_min ;  
    stm.tm_sec = tm->tm_sec;  

  
    return mktime(&stm);  
}

void check_time(uint8_t *tm)  
{  
    struct rtc_time stm;
		char year[4];
		char mon[2];
	char mday[2];
	char hour[2];
	char min[2];
	char sec[2];
	u8 buf[2]={0x20,0x00};
	unsigned long recv_time;
	 u8 sub_num;
	  strncpy(year,tm,4);
	  
		strncpy(mon,tm+4,2);
	strncpy(mday,tm+6,2);
	strncpy(hour,tm+8,2);
	strncpy(min,tm+10,2);
	strncpy(sec,tm+12,2);
		printf("time is %s\n",tm);
    stm.tm_year = atoi(year) ;  
    stm.tm_mon = atoi(mon)-1;  
    stm.tm_mday = atoi(mday);  
    stm.tm_hour = atoi(hour);  
    stm.tm_min = atoi(min) ;  
    stm.tm_sec =atoi(sec);  
		printf("%4d-%02d-%02d %02d:%02d:%02d\n",
		 stm.tm_year, stm.tm_mon , stm.tm_mday,
		stm.tm_hour, stm.tm_min, stm.tm_sec);
	
	
		recv_time = GetTick(&stm);
		
		sub_num = abs(GetTick(&stm) - now_time);
			printf("sub is %d\n",sub_num);
			
			
		 if( sub_num >= 0 && sub_num <= 200)
		 {
			  printf("时间在误差范围内\n");
			// return ;
		 
		 }

		

			if (hym8563_i2c_set_regs(0xA2, 0x00, buf, 1) == 0)
		{
				printf("关闭hym8563成功\n");
			
		}


	/*
	***
	设置日期、时间
	***	
	*/
		
			if(hym8563_set_time(0xA2, &stm) == 0)
		{
			printf("set hym8563 time成功\n");
		}
		



	/*
	***
	读取日期、时间
	***	
	*/
		
		if (hym8563_i2c_set_regs(0xA2, 0x00, buf+1, 1) == 0)
		{
				printf("开启HYM863时钟\n");
			
		}
  
//	}
 
}



extern uint32_t ulong;

/****根据机器类型生成不同的密码***
***
参数1：buf  所储存密码的6位数组
参数2：mach_type 机器类型
   1：表示围墙机
   2：表示门口机
****/





uint32_t  check_passwd(u32 type_value,u8 mach_type)
{
	   u32 now_value;
     u32 value1,value2,value3;
      printf("type_value is %d\n",type_value);
	   switch(mach_type)
		 {
			 case 1:
				      
						value1 = type_value /100000;
			      value2 = type_value / 1000 %10;
			      value3 = type_value /10 % 10;
			      now_value = value1 * 100 + value2 * 10 + value3;
			      break;
			      
			 case 2:
				   // value1 = type_value /10000 %10;
			     // value2 = type_value/ 100 %10;
			     // value3 = type_value  % 10;
			     // now_value = value1 * 100 + value2 * 10 + value3;
							now_value = type_value;
			      break;
			 default:
				    now_value = 0;
			      break;
				 		        
		 }
		 return now_value;
	
}

uint32_t  generate_password(u32 *passwd2,u32 *passwd1)
{
	   u32 now_passwd;
     u32 value1,value2,value3,value4,value5,value6;
//     printf("passwd1 is %d\n",*passwd1);
//	
//		 printf("passwd2 is %d\n",*passwd2);
//		value1 = passwd1 /100 ;
//		value2 = passwd1 / 10 %10;
//		value3 = passwd1 % 10;
//		value4 = passwd1 /100;
//		value5 = passwd1 / 10 %10;
//		value6 = passwd1 % 10;
		
		//now_passwd = ((value1 * 100000) + (passwd2/100)*10000 + (value2 * 1000) + (passwd2/10%10)*100 + value3 * 10 + (passwd2%10));
	
		value1 = (*passwd1 /100) *100000;
		value2 = (*passwd2 / 100) *10000;
		value3 = (*passwd1 / 10 %10) * 1000;
		value4 = (*passwd2 / 10 %10) * 100;
		value5 = (*passwd1  %10) *10;
		value6 = *passwd2 % 10;
			now_passwd = value1 + value2 + value3 + value4 + value5 + value6 ;
	  printf("now_passwd is %d\n",now_passwd);

		 return now_passwd;
	
}


uint8_t sm3_time_test(unsigned long now_time,struct sm3_info *sm,u8 n,u8 mach_type)
{
	
				u8 pass_success = 0;
				u8 type[3] = {64,64,8};
				unsigned char build_num[] ="FFFFFFFF";
				unsigned long i_num ;
				u32 passwd ;
				u32 b_passwd;
			//	u32 *test1 =NULL;
				u32 test = 0;
				printf("mach_type is %d\n",mach_type);
				printf("now_time is %d\n",now_time);
				printf("key_buf is %s\n",key_buf);
				 switch(n)
				 {
					 case 0:
						      passwd = 	check_passwd(atoi(key_buf),mach_type);
					        i_num  = now_time/60 - 60;
						 break;
					 case 1:
						      passwd = 			check_passwd(ulong,mach_type);
									i_num  = now_time/60 - 60;
						 break;
					 case 2:
						      passwd = 			check_passwd(atoi(BleValue),mach_type);
									i_num  = now_time/60 - 5;
						 break;
					 
					 
				 }
				printf("passwd is %d\n",passwd);
			//	u32 test[70];
				
			//	u32 *test = (u32 *)malloc(SRAMEX,sizeof(u32)*type[n]);
				 //mymemset((u32 *)test,0,sizeof(u32)*type[n]);

				
				

				int i;
				

			for(i = 0;i < 8;i++)
			{
					printf("%c ",sm->community_id[i]);
				
			}
			printf("\n");
			printf("build_num:");
			for(i = 0;i < 8;i++)
			{
					printf("%c",sm->build_num[i]);
				
			}
			printf("\n");
			printf("cell_num:");
			for(i = 0;i < 8;i++)
			{
					printf("%c",sm->cell_num[i]);
				
			}
			printf("\n");
				
				
				for(i = 0 ; i < type[n] ;i++)
				{
							if(mach_type == 2)
							{
								
							//	test[i] = get_sm3_pass(i_num , sm->community_id, sm->build_num, sm->cell_num);
								test = get_sm3_pass(i_num , sm->community_id, sm->build_num, sm->cell_num);
								printf("test[%d] id is %d \n",i,test);
								b_passwd = get_sm3_pass(i_num , sm->community_id,build_num, sm->cell_num);
								
									printf("b_passwd id is %u \n",b_passwd);
									if(generate_password(&test,&b_passwd) == passwd )
									{
												//	LOCK_ON;
													printf("密码正确\n");
													pass_success = 1;
												//	StartPlay("1:/欢迎回家.wav",0);
													//myfree(SRAMEX,test);
													//test = NULL;
													return 1;	
									}
								
							}
							else
							{
										printf("key is %s\n",key_buf);	
									test = get_sm3_pass(i_num , sm->community_id, sm->build_num, sm->cell_num);
									printf("test[%d] id is %d \n",i,test);
									if(test == passwd )
									{
												//	LOCK_ON;
													printf("密码正确\n");
													pass_success = 1;
												//	StartPlay("1:/欢迎回家.wav",0);
												//	myfree(SRAMEX,test);
													//test = NULL;
													return 1;	
									}
							}
							i_num ++;
//						
//						if(mach_type == 2)
//						{
//									if(generate_password(test[i],b_passwd) == passwd )
//									{
//												//	LOCK_ON;
//													printf("密码正确\n");
//													pass_success = 1;
//												//	StartPlay("1:/欢迎回家.wav",0);
//													myfree(SRAMEX,test);
//													test = NULL;
//													return 1;	
//									}
//						}
//						else 
//						{
//										if(test[i] == passwd )
//									{
//												//	LOCK_ON;
//													printf("密码正确\n");
//													pass_success = 1;
//												//	StartPlay("1:/欢迎回家.wav",0);
//													myfree(SRAMEX,test);
//													test = NULL;
//													return 1;	
//									}
//						}
				
			


	
			}
				
			if(i == type[n] && pass_success == 0 )
			{
					delay_ms(5);
				//	LOCK_OFF;
						//memset(setip,0,6);
				//StartPlay("1:/密码错误.wav",0);
					ulong = 0;
				//printf("密码错误\n");
			}
		//	myfree(SRAMEX,test);
			//test = NULL;
			
	   return -1;
}





 
