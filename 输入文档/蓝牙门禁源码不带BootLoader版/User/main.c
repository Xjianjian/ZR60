/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��дEEPROM
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./usart3/usart3.h"
#include "./usart4/usart4.h"
#include "./usart5/usart5.h"
#include "./i2c/bsP_i2c_ee.h"
#include "./led/bsp_led.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "./timeStamp/time_stamp.h"
//#include "./sm3/sm3.h"
#include "time.h"
#include "./systick/bsp_SysTick.h"
#include "./key/bsp_exti.h"
#include "./rtc/rtc.h"
#include "./tim/bsp_basic_tim.h"
#include "./display/display.h"
#include "./sm3/SM3.h"
#include "./beep/bsp_beep.h" 
#include "string.h"
#include "./wm8978/bsp_wm8978.h"
#include "./FATFS/ff.h" 
#include "./recorder/Recorder.h"
#include "./sram/bsp_sram.h"	
#include "./malloc/malloc.h"
#include "./timeStamp/time_stamp.h"
#include "./FATFS/tm_stm32f4_fatfs.h"
#include "./FATFS/diskio.h"	
#include "./flash/fatfs_flash_spi.h"
#include "./auxdata/aux_data.h"
#include "./soundwave/soundwave.h"
#include "./adc/bsp_adc.h"
#include "./wdata/wdata.h"
#include "./keyinput/bsp_input.h" 

#include "./readcard/readcard.h"
#include "./iwdg/bsp_iwdg.h"   
#include "./rcc/bsp_clkconfig.h"

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */



#define DEBUG 1


extern uint8_t key_int;
extern uint8_t update_sec;
uint8_t 			sm3_flag = 0;
uint8_t 			enter_flag = 0;
uint8_t 			sec_count = 0;
u8  time_buf[8];
 u8  test_buf[16];
extern u8 key_buf[6];
uint8_t 			delay_flag = 0;

extern unsigned long now_time;
extern u8 receive_flag;
extern uint8_t ble_flag;
extern uint8_t set_time_flag;
extern uint8_t timelist[14];
extern FATFS flash_fs;//FLASH�ļ�ϵͳ
extern union SoundWaveBuf   Buffer_Sound;
extern uint32_t ulong;
extern uint8_t b_500ms_flag; 
extern 	u8 open_door;
 uint8_t b_1s_count = 0;
extern u8 build_type;  //��������
u8 ALARM = 0;
extern u8 b_10ms_flag;
extern u8 b_50ms_flag ;
extern u8 b_500ms_flag ;
extern u8 b_1s_flag ;
extern 	u8 open_flag;
u8 b_5s_flag = 0;
u8 b_5s_count = 0;
u8 b_err_5s_flag = 0;
u8 b_err_5s_count = 0;
u8 b_3s_count = 0;
extern u8 inderopen_flag;
extern u8 b_3s_flag;
extern uint8_t read_card_flag;

 extern uint8_t setcard ;
 extern uint8_t isSectedCard_flag;
 extern struct comm_info sminfo;
extern struct comm_info sminfo1;

 extern RebackInfo  rebackinfo;
 
extern u8 card_set;
struct sm3_info sm = {{'D', 'F', '0', '0', '7', '7', 'B', '1'},{'0','0','0','1','F','F','F','F'},{'F','F','F','F','F','F','F','F'}};

//struct sm3_info sm = {{ 'F','F','F','F','F','F','F','F'},{'0','0','0','1','F','F','F','F'},{'F','F','F','F','F','F','F','F'}};

	
//	struct sm3_info sm;




/****                      ****/
	
	
//	float ADC_Vol;//���ڱ���ת�������ĵ�ѹֵ
	extern __IO uint16_t ADC_ConvertedValue;//ADC1
	
float adc_value;
u8 delay_num;

uint8_t  return_data(uint16_t temp)
{
    switch(temp)
    {
        case 0x0000:
          return 0xff;
        case 0x0001:
					StartPlay("1:/ȡ��.wav",0);
					delay_ms(50);
          return  'C';
        case 0x0002:
					StartPlay("1:/7.wav",0);
					delay_ms(50);
          return  '7' ;
        case 0x0004:
					StartPlay("1:/4.wav",0);
					delay_ms(50);
          return  '4';
        case 0x0008:
					StartPlay("1:/8.wav",0);
					delay_ms(50);
          return  '8';
        case 0x0010:
					StartPlay("1:/0.wav",0);
					delay_ms(50);
          return  '0';
        case 0x0020:
					//StartPlay("1:/ȷ��.wav",0);
					delay_ms(50);
          return  'A';
        case 0x0040:
         // return  'E';
        case 0x0080:
         // return  'F';
        case 0x0100:
          //return  'D';
        case 0x0200:
         // return  'B';
						break;
        case 0x0400:
					StartPlay("1:/6.wav",0);
					delay_ms(50);
          return  '6';
        case 0x0800:
					StartPlay("1:/9.wav",0);
					delay_ms(50);
          return  '9';
        case 0x1000:
					StartPlay("1:/3.wav",0);
					delay_ms(50);
          return  '3';
        case 0x2000:
					StartPlay("1:/2.wav",0);
					delay_ms(50);
          return  '2';
        case 0x4000:
					StartPlay("1:/5.wav",0);
					delay_ms(50);
          return  '5';
        case 0x8000:
					StartPlay("1:/1.wav",0);
					delay_ms(50);
          return  '1';
      default:
          return  0xff;
        
    }
  
}
u8 RfidGetFinishFlag = 0;


 void del_rep(u8 *flag,u8 *count,u8 type)
 {
	 
	 		if( *flag == 1)
			{
						(*count)++;
						if((*count) == 1)
						{
								if(type == 1) // ble error
								{
										StartPlay("1:/errorKey.wav",1);
										delay_ms(30);

								}
								else 
								{
									LOCK_ON;
									b_1s_count = 0;
									StartPlay("1:/��ӭ�ؼ�.wav",0);
									delay_ms(30);
								}
						}
						else
						{
									if((*count) >= 8)
									{
												(*count) = 0;
												(*flag) = 0;
									}
												
						}
										
			}
	 
 }

 void del_card_config()
 {
	 									if(b_3s_flag == 1)
									{
										b_3s_count++;
											printf(" b_3s_count = %d V \r\n",b_3s_count ); 
											if(b_3s_count >=8)
											{
												printf(" b_3s_count = %d V \r\n",b_3s_count );
												b_3s_count = 0;
												 if(card_set == 1)
												 {
													 card_set = 0;
												 }
												b_3s_flag = 0;
											}	
									}
	 
 }

extern FIL file;

//uint8_t buffer0[RECBUFFER_SIZE];  /* ���ݻ�����1 ��ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */

uint8_t read[10];
UINT bbw; 
UINT br;
//u32 timeold = 0;
extern u8 timenew;
int main(void)
{
	
		u8 ret;
  FRESULT res = FR_OK;
	 u8 flag = 1;
	 int i,state;
	int key_num = 0;


u8 buf[2]={0x20,0x00};
u16 key,value,bnum;

u8 key_flag = 0;
 //u8 read_card_flag;

	
	//struct rtc_time tm = {30,33,9,23,11,2017,4};

	HSE_SetSysClock(25, 336, 2, 7);
  LED_GPIO_Config();
  Debug_USART_Config(); 
	USART3_Config();
	USART4_Config();
	USART5_Config();

	SysTick_Init();
	//LED_BLUE;
  /*��ʼ��USART1*/
	Rheostat_Init();
	//EXTI_Key_Config();
EXTI_Key_Config(ENABLE);
	  /* ��ʼ��������ʱ����ʱ��0.5s����һ���ж� */
	TIMx_Configuration();
	BEEP_GPIO_Config();
//	Key_GPIO_Config();
	LCD_Init();
		
	printf("\r\n ��ӭʹ�ñ���  STM32 F407 �����塣\r\n");		 

	printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");

 	FSMC_SRAM_Init();			  //��ʼ���ⲿSRAM  
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��

	TM_FATFS_FLASH_SPI_disk_initialize();
  
  res = FR_DISK_ERR;
	res = f_mount(&flash_fs,FLASH_ROOT,1);  //�����ļ�ϵͳ
  if(res == FR_OK)
	{
		printf("�ļ�ϵͳ���سɹ�\n");
	}
	else
	{
		printf("�ļ�ϵͳ����ʧ��\n");
	}


	
	
	ret = wm8978_Init();//��Ƶ��ʼ��

	
	if(ret == 0)
	{
		printf("δ�ҵ���Ƶ����оƬ\n");
	}
	else
	{
		printf("�ɹ��ҵ���Ƶ����оƬ\n");
	}
	
	zzw_playconfig();
IWDG_Config(IWDG_Prescaler_256 ,800);//5.12s
//	StartPlay("1:/��Ƭ��Ч.wav",0);
//	StartPlay("1:test.wav",0);
	
	


	read_init_sminfo("1:/д��.txt",&sminfo1,&sm);
	
					if(strncmp(sm.build_num,"FF",2) == 0)
					{
								build_type = 1;
					}
					else
					{
								build_type = 2;
					}
	
					printf("build_type is %d\n",build_type);
	
			if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
			{
				/* �������Ź���λ */
				/*  ����� */
			//  LED_RED;
					printf("���Ź���λ\n");
				/* �����־ */
				RCC_ClearFlag();
				
				/*���һֱ��ι������һֱ��λ������ǰ�����ʱ���ῴ�������˸
				��1s ʱ����ι���Ļ������������̵�*/
			}
			else
			{
				
			}
		
			init_serial();	//�������������Զ�Ѱ������										
														

  while (1)
  { 

							if(card_set == 1 && open_door == 1 )
							{
								setcard = 0;
								card_set = 0;	
								open_door = 0;
									printf("����д��ģʽ\n")	;
									delay_ms(10);

										if(add_data("1:/д��.txt",&sminfo) == 0)
										{
												printf("д���ɹ�,�����ȡ��Ϣģʽ\n")	;	
												if(read_init_sminfo("1:/д��.txt",&sminfo1,&sm) == 0)
												{
													printf("��ȡ��Ϣ�ɹ�\n")	;	
												};
												StartPlay("1:/���óɹ�.wav",0);
												memset(test_buf,0,sizeof(test_buf));
										}
										else{
												StartPlay("1:/��Ƭ��Ч.wav",0);
										}
								delay_ms(1000);				
							}							
						if(open_door == 1)
						{
								
									if(card_data_handle(&sminfo,&sminfo1,build_type) == 0)
									{
											StartPlay("1:/��Ƭ��Ч.wav",0);
											delay_ms(10);
									}
									open_door = 0;
						}
						if(key_int == 1)
						{
								delay_ms(50);
								if(i2c_master_reg8_recv(0x6E, 0xAA, buf, 2) == 0 && flag == 1)
								{
										sec_count = 0;
										key = buf[1];
										key = (key << 8) | buf[0];
										if(key !=0)
										{
												printf("key is %#x\n",key);
												value = return_data(key);
												if( value != 0xff)
												{
															if( value == 'C')
															{
																/*	key_num--;
																	if(key_num < 1)
																	{
																			key_num = 0;
																	}*/
																	key_num = 0;
																	printf("ȡ��������\n");
																	sm3_flag = 1;		
															}
															else if( value == 'A')
															{ 												
																 if(key_num == 6)
																 {
																		printf("ȷ�ϼ����� ���룺");
																		for(i = 0;i<8;i++)
																		{
																			printf("%c",test_buf[i]);
																		}
																		printf("\n");
																		strncpy(key_buf,test_buf,6);
																		key_flag = 1;
																		state = 0;	
																 }
																 key_num = 0;
																 sm3_flag = 0;												 
															}
															else
															{													
																	test_buf[key_num] = value;
																	key_num ++;
																	sm3_flag = 1;	
															}													
															state = 1;
											 }
									}	
							}
							key_int = 0;
					}
					if(state == 1 && sm3_flag == 1)
					{
							flag = 0;
							printf("i is %d\n",key_num);
							display(test_buf,key_num);
							if(key_num >  6)
							{				
									key_num =0;
									sm3_flag = 0;
							}
							delay_ms(50);
							flag = 1;
							state = 0;
					}
					if(b_50ms_flag == 1)
					{
									b_50ms_flag = 0;
//						   		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_OFF  )
//								{
//										LOCK_ON;
//										delay_flag = 1;
//										b_1s_count = 0;
//										StartPlay("1:/һ·˳��.wav",0);
//										delay_ms(20);

//								}
								if(isSectedCard_flag == 1)
								{
										init_serial();
										isSectedCard_flag = 0;
								}
						
								if( inderopen_flag == 1)//���ڿ��ز���
								{
										delay_ms(20);
										inderopen_flag = 0;
										LOCK_ON;
										delay_flag = 1;
										b_1s_count = 0;
										StartPlay("1:/һ·˳��.wav",0);
								}
								if( Key_Scan(ALARM_GPIO_PORT,ALARM_PIN) == KEY_OFF  )//��������
								{
										LOCK_ON;//����
										TIM_Cmd(BASIC_TIM, DISABLE);//�رն�ʱ���ж�
										USART_Cmd(UART5, DISABLE);//�رմ���5�ж�
										USART_Cmd(UART3_USART,  DISABLE);//�رմ���3�ж�
										USART_Cmd(UART4,  DISABLE);//�رմ���4�ж�
									  ALARM = 1;
										EXTI_Key_Config(DISABLE);//�رհ����ж�
										flag = 0;//�رհ������ȡ
										delay_ms(20);
								}
				
					}
					if(read_card_flag == 1)//�յ�������������һ֡����
					{
								handleMsg(rebackinfo); //������������
								read_card_flag = 0;									
					}	
					if(b_500ms_flag == 1)
					{
							b_500ms_flag = 0;
							update_sec ++;
							if(update_sec == 2 )
							{
									update_sec = 0;
									if(sm3_flag == 0)
									{
										display_time();
									}						
									sec_count ++;	

					  //	printf("sec_count is %d\n",sec_count);
									if(sec_count >= 20)
									{
												if(sm3_flag == 1)
												{
														sm3_flag = 0;	
														key_num = 0;
												}		
										sec_count = 0;	
									}
									if(delay_flag == 1)
									{
										
											b_1s_count ++;
											printf(" b_1s_count = %d  \r\n",b_1s_count ); 
											printf(" b_1s_count = %d  \r\n",delay_num );  
											if(b_1s_count >= delay_num)
											{
													b_1s_count = 0;
													delay_flag  = 0;
													LOCK_OFF;
											}									
									}
						} 
				//		Usart_SendString(UART5,"AT+DISA?");
					}	
						if( open_flag == 1 )//������֤�ɹ����ű�־
						{
								LOCK_ON;
								delay_flag = 1;
								b_1s_count = 0;
								StartPlay("1:/��ӭ�ؼ�.wav",0);
								delay_ms(20);
								open_flag = 0;
						} 
						if (key_flag == 1)//����6λ�����־
						{
								printf("anjian\n");
								delay_flag = sm3_time_test(now_time,&sm,0,build_type);//��������ȶ�
								b_1s_count = 0;//��ʱ��������
								if(delay_flag!= 1)
								{
									StartPlay("1:/errorKey.wav",1);
									delay_ms(30);
								}
								else
								{
										memset(key_buf,0,6);
										StartPlay("1:/��ӭ�ؼ�.wav",0);
										delay_ms(30);
								}
								printf("delay_flag is %d\n",delay_flag);		
								key_flag = 0;
						}	
						if(receive_flag == 1)//���������յ���־
						{
								printf("shengbo\n");
								delay_flag = sm3_time_test(now_time,&sm,1,build_type);//��������ȶ�
								if(delay_flag!= 1)
								{
										StartPlay("1:/errorKey.wav",1);
										delay_ms(30);
								}
								else
								{
										StartPlay("1:/��ӭ�ؼ�.wav",0);
										delay_ms(30);
									
								}
								receive_flag = 0;
						}
					/** ������������  **/
						if(ble_flag == 1)
						{

							printf("bluebooth\n");
							delay_flag = sm3_time_test(now_time,&sm,2,build_type);
						  if(b_5s_flag == 1) //��ʾ��6s���Ѿ��յ�����������������
							{
								;
							}
							else
							{
									if(delay_flag!= 1)//��ʾ�յ�����ȶ�û�гɹ�
									{
										b_err_5s_flag = 1;
									}
									else
									{
										b_5s_flag = 1;//����6s���յ���������������
									}
							}

							ble_flag = 0;
						}
						/**��������������ʱ��У׼��־*/
						if(set_time_flag == 1)
						{
								check_time(timelist);//У׼ʱ��
								set_time_flag = 0;
						}
						if(b_1s_flag == 1)
						{
										Usart_SendString(UART5,"AT+DISA?");//ÿ��1s������ģ�鷢��
										adc_value = ((float)ADC_ConvertedValue/4096)* 3.30;//ÿ��1s��ȡ��λ���ĵ�ѹ
										printf("adc_value is %f\n",adc_value);
								
										if(adc_value < 0.5)
										{
												delay_num = 1;
										}
										else if(adc_value < 3.0)
										{
												delay_num = (u8)( adc_value * 10.0 / 3.0) + 1;
										}
										else
										{
											delay_num = 10;
										}
										
									printf(" delay time  = %d V \r\n",delay_num ); 

									del_card_config();//������ĸ�����ü�������ˢ���Ĳ�����
									del_rep(&b_5s_flag,&b_5s_count,2);//��������������֤��ȷ�Ĳ���
									del_rep(&b_err_5s_flag,&b_err_5s_count,1);//���������������Ĳ���
			
								b_1s_flag = 0;                                                                                    											
							}	
							if(ALARM == 1)
							{
								
										printf("ALARM");
										StartPlay("1:/�о���.wav",1);
										delay_ms(3000);
							}
					
					IWDG_Feed();//4.5s����˾�������Ź���λ
						
  }  

}



/*********************************************END OF FILE**********************/

