#include  "led.h"
#include  "dma.h"
#include  "iic.h"
#include  "iis.h"   
#include  "text.h"
#include  "sram.h"
#include  "delay.h"
#include  "button.h"
#include  "malloc.h"
#include  "usart.h"
#include  "znfat.h"
#include  "timer3.h"
#include  "wm8978.h"
#include  "at24c02.h"
#include  "fontupd.h"
#include  "exfuns.h"
#include  "mp3play.h"
#include  "wavcodec.h"
#include  "lcd12864.h"  
#include  "keyboard.h"
#include  "audioplayer.h"
#include  "sdio_sdcard.h"
struct znFAT_Init_Args Init_Args; //��ʼ����������
struct FileInfo fileinfo_gbkuni,fileinfo;
struct FileInfo fileinfo_f12,fileinfo_f16,fileinfo_f24;
uint8_t KeyVal=0;
int main(void)
{
   uint8_t i=0;
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
   delay_init(168);
   LED_Init();
   uart_init(115200);
   LCD_GPIO_Init();
   LCD12864_Init();       
   LCD_BL_ON;
   FSMC_SRAM_Init();
   WM8978_Init();				//��ʼ��WM8978
   WM8978_HPvol_Set(20,20);	//������������
   WM8978_SPKvol_Set(40);		//������������
   my_mem_init(SRAMIN);
   my_mem_init(SRAMEX);
   my_mem_init(SRAMCCM);
   Button_Init();
   AT24CXX_Init();
   while(AT24CXX_Check())
   {
      LED1=!LED1;
	  delay_ms(300);
   }
   while(SD_Init())//��ⲻ��SD��
   {
      LED0=!LED0;
	  delay_ms(300);
   }
   znFAT_Device_Init();
   znFAT_Select_Device(0,&Init_Args);
   while(znFAT_Init());
   while(znFAT_Open_File(&fileinfo_f12,(char*)GBK12_PATH,0,1));
   while(znFAT_Open_File(&fileinfo_f16,(char*)GBK16_PATH,0,1));
   while(znFAT_Open_File(&fileinfo_f24,(char*)GBK24_PATH,0,1));
   while(znFAT_Open_File(&fileinfo_gbkuni,(char*)UNIGBK_PATH,0,1));
/*
 * arr:�Զ���װ��ֵ psc:Ԥ��Ƶ��
 * ��ʱ�����ʱ��Tout:((arr+1)*(psc+1))/Ft us
 * Ft:��ʱ������Ƶ�� 84MHz
 *���ʱʱ��65536*65536/84/1000/1000=51.13s
 */
   Timer3_Init(199,8399);//��ʱ20ms
   i=audio_get_tnum("/MUSIC/*.MP3");
   printf("/MUSIC/·������%d��MP3�ļ�\r\n",i);
   audio_play(T_MP3);
   znFAT_Close_File(&fileinfo_f12);
   znFAT_Close_File(&fileinfo_f16);
   znFAT_Close_File(&fileinfo_f24);
   znFAT_Close_File(&fileinfo_gbkuni);
   znFAT_Flush_FS(); //ˢ���ļ�ϵͳ
   while(1)
   {  
//		printf("Voltage_PA4=%dVoltage_PA5=%d\r\r",ADCConvertedValue[0],ADCConvertedValue[1]);
//		LED0=0;
//        delay_ms(500);		  
//        LED0=!LED0;
//		delay_ms(500);
     


   }
}


