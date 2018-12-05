
#include "./display/display.h"
#include "./timeStamp/time_stamp.h"
#include "./rtc/rtc.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsP_i2c_ee.h"
#include "./led/bsp_led.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "./systick/bsp_SysTick.h"


uint8_t LCD1602_Table[]="0123456789";

	uint8_t DIS_DAT[]  ={"2017:11:21(1)   "};
	uint8_t DIS_TIME[] ={"HH:MM:SS        "};

unsigned long now_time;
	//		struct rtc_time tm;

void Write_Command(unsigned char com)
{
        unsigned char com1,com2;
        com1 = com|0x0F;
        delay_us(1000);
        I2C_Write_Byte(0x27,com1&0xFC);
        delay_us(500);
        I2C_Write_Byte(0x27,com1&0xF8);
 
        com2 = com<<4;
        com2 |= 0x0F;
        delay_us(1000);
        I2C_Write_Byte(0x27,com2&0xFC);
        delay_us(500);
        I2C_Write_Byte(0x27,com2&0xF8);
}

//д����
uint8_t Write_Data(uint8_t data)
{
  uint8_t com1,com2;
  com1 = data|0x0F;
  delay_us(1000);
  I2C_Write_Byte(0x27,com1&0xFD);
  delay_us(500);
  I2C_Write_Byte(0x27,com1&0xF9);
 
  com2 = data<<4;
  com2 |= 0x0F;
  delay_us(1000);
  I2C_Write_Byte(0x27,com2&0xFD);
  delay_us(500);
  I2C_Write_Byte(0x27,com2&0xF9);
	
}
	
void LCD_Init()
{
		uint8_t i;


	I2C_Write_Byte(0x27,0x08);
  Write_Command(0x0F);
	  Write_Command(0x28);
  //Write_Command(0x32);//����4λ
  I2C_Write_Byte(0x27,0x0C);
  I2C_Write_Byte(0x27,0x08);
	  Write_Command(0x28);
	Write_Command(0x01);//�����0
  Write_Command(0x0C);//����ʾ���޹�ꡢ����˸
  Write_Command(0x06);//����һ���������ƶ�
	
	  /*    
	Write_Command(0x80);//��һ������
	

	for(i = 0;i < 16;i++)
        Write_Data(DIS_TAB1[i]);
		Write_Command(0x80+0x40);
	for(i=0;i<16;i++)
	Write_Data(DIS_TAB2[i]);
	*/
		



	
}


void display(uint8_t *buf,int n)
{
	  u8 i,m;
		
			if(n < 0)
				 n = 0;
	//		Write_Command(0x01);
			Write_Command(0x80);//��һ������
			for(m = 0;m < n;m++)
						Write_Data(buf[m]);
			for(i = n ;i < 16;i++)
						Write_Data(' ');
			Write_Command(0x80+0x40);//�ڶ�������
			for(i=0;i<16;i++)
				Write_Data(' ');
	
}

void display_time(void)
{
		u8 i;

	struct rtc_time tm;
	if(hym8563_read_datetime(0xA2, &tm) == 0)
	{
		//printf("��ȡhym8563����ʱ��ɹ�\n");
	}

	/*
		printf("ʱ�� %4d-%02d-%02d(%d) %02d:%02d:%02d\n",
		tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);	
*/
			


	 now_time = GetTick(&tm);
	//	printf("display now_time is %ld \n",now_time);
	
	//	sm3_time_test(now_time,5);
		Write_Command(0x80);//��һ��

		Write_Data(LCD1602_Table[tm.tm_year/1000]);
		Write_Data( LCD1602_Table[tm.tm_year/100%10]);
		Write_Data( LCD1602_Table[tm.tm_year/10%10]);
		Write_Data( LCD1602_Table[tm.tm_year%10]);
		Write_Data(':');
		Write_Data( LCD1602_Table[tm.tm_mon/10]);
		Write_Data( LCD1602_Table[tm.tm_mon%10]);
		Write_Data(':');
		Write_Data( LCD1602_Table[tm.tm_mday/10]);
		Write_Data( LCD1602_Table[tm.tm_mday%10]);	
		Write_Data('(');
		Write_Data( LCD1602_Table[tm.tm_wday%10]);
		Write_Data(')');
		for(i = 13;i < 16;i++)
			Write_Data(DIS_DAT[i]);
	
		Write_Command(0x80 + 0x40);//�ڶ���
		
		Write_Data(LCD1602_Table[tm.tm_hour/10]);
		Write_Data( LCD1602_Table[tm.tm_hour%10]);
		Write_Data(':');
		Write_Data( LCD1602_Table[tm.tm_min/10]);
		Write_Data( LCD1602_Table[tm.tm_min%10]);
		Write_Data(':');
		Write_Data( LCD1602_Table[tm.tm_sec/10]);
		Write_Data( LCD1602_Table[tm.tm_sec%10]);
		for(i = 8;i < 16;i++)
			Write_Data(DIS_TIME[i]);
	
	
}
