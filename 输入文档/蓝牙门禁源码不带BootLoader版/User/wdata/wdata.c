

#include "./wdata/wdata.h"

#include "usart/bsp_debug_usart.h"
#include "./FATFS/ff.h" 
#include "./malloc/malloc.h"

#include "string.h"


	
uint8_t LCD_CODE[]="0123456789ABCDEF";


	extern  u8 build_type;
int black_n = 0;				//��������������
FIL file_name;							/* �ļ����� */
UINT name_num;            					  /* �ļ��ɹ���д�� */
FRESULT res = FR_OK;
/***
���ܣ������ļ������濨��Ϣ
������
filename���ļ�������·�� �� "1:/д��.txt
sm3_buf: ic����Ϣ��
				struct comm_info //¥����Ϣ
				{
						unsigned char community_id[16];//С��id
						unsigned char build_num[16];//¥����
						unsigned char cell_num[16];//��Ԫ��,д��
				}


***/

int add_data(const char *filename,struct comm_info *sm3_buf)
{
	int i = 0;

	int name_n = 0;
	res = f_open(&file_name,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res == FR_OK)
	{
		printf("��д���ļ��ɹ�\n");
	}
	else
	{
		printf("��д���ļ�ʧ��\n");
		return -1;
	}
	
	//printf("��Ӻ�����һ�β鿴��file_name.fsize = %d\n",file_name.fsize);

	res = f_write(&file_name, sm3_buf,sizeof(struct comm_info), &name_n);

	if(res == FR_OK)
	{
		printf("д��ĸ�����ݳɹ�\n");
	}
	else
	{
		printf("д��ĸ������ʧ��\n");
			f_close(&file_name);
		return -1;
	}	

	f_close(&file_name);
	
	return 0;
	

}
/**
 ���ܣ���ȡ�ļ�����Ŀ���Ϣ
������
filename���ļ�������·�� �� "1:/д��.txt
sm3_buf: ���ļ�����Ϣ�浽�ṹ��sm3_buf
				struct comm_info //¥����Ϣ
				{
						unsigned char community_id[16];//С��id
						unsigned char build_num[16];//¥����
						unsigned char cell_num[16];//��Ԫ��,д��
				}


**/


int read_card_data(const char *filename,struct comm_info *sm3_buf)
{
	int i = 0;

	int name_n = 0;	
	
	
		/*��д���ļ�����Ϣ���¶�����*/
	res = f_open(&file_name,filename,FA_OPEN_EXISTING | FA_READ);
	if(res == FR_OK)
	{
		printf("��д���ļ��ɹ�\n");
	}
	else
	{
		printf("��д���ļ�ʧ��\n");
		return -1;
	}	
	name_n = 0;
	res = f_read(&file_name,sm3_buf,sizeof(struct comm_info), &name_n);
	if(res == FR_OK)
	{
		printf("��ȡд���ļ��ɹ�\n");
		printf("��Ӻ�������Ϣ�󣬶�ȡ����Ч�ֽ���Ϊ��name_n= %d\n",name_n);
			
	}
	else
	{
		printf("��ȡд���ļ�ʧ��\n");
		return -1;
	}

	f_close(&file_name);
	return 0;
	
}
/**
���ܣ������ڳ�ʼ����ʱ���ȡic���ļ������ݣ�����ȡic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
������
filename���ļ�������·��
sm3_buf�����ļ�����Ϣ�浽�ṹ��sm3_buf
sm��ic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ���ڽṹ��sm

**/

int read_init_sminfo(const char *filename,struct comm_info *sm3_buf,struct sm3_info *sm)
{
	u8 i;
		if(read_card_data(filename,sm3_buf) != 0)
		{
				
			return -1;
		}
		printf("��ȡ��ʼ���ſ���Ϣ�ɹ�\n");
		
		printf("community_id:");
		for(i = 0;i < 16;i++)
		{
					printf("%#x ",sm3_buf->community_id[i]);
					if(i < 8)
					{
							
							if(i % 2 ==0)
							{
							sm->community_id[i] = LCD_CODE[sm3_buf->community_id[i/2]/16];
							}
							else
							{
							sm->community_id[i] = LCD_CODE[sm3_buf->community_id[i/2]%16];
							}
						//printf("%#x ",sm->community_id[i]);
					}
		}
		printf("\n");
		printf("build_num:");
		for(i = 0;i < 16;i++)
		{
				printf("%#x ",sm3_buf->build_num[i]);
				if(i < 4)
				{
						//	sm->build_num[i] = sm3_buf->build_num[i];
					
						if(sm3_buf->build_num[i] == 0xaa)
						{
							sm->build_num[i] = 'F';
						}
						else 
						{
									if(i % 2 ==0)
									{
										sm->build_num[i] = LCD_CODE[sm3_buf->build_num[i/2]/16];
									}
									else
									{
										sm->build_num[i] = LCD_CODE[sm3_buf->build_num[i/2]%16];
									}
						}

				}
				else if(i < 8)
				{
						//	sm->build_num[i] = sm3_buf->build_num[i];
						sm->build_num[i] = 'F';
						//printf("%#x ",sm->build_num[i]);

				}
				
		
				
					
				
				
		}
		printf("\n");
		printf("cell_num:");
		for(i = 0;i < 16;i++)
		{
				printf("%#x ",sm3_buf->cell_num[i]);
				if( i < 8)
				{

					//sm->cell_num[i] = sm3_buf->cell_num[i];
					sm->cell_num[i] = 'F';
					//printf("%#x ",sm->cell_num[i]);
							
				}
		}
		printf("\n");


		printf("sm3 community_id:");
		for(i = 0;i < 8;i++)
		{

				printf("%c",sm->community_id[i]);

		}
		printf("\n");
		printf("sm3  build_num:");
		for(i = 0;i < 8;i++)
		{

					printf("%c",sm->build_num[i]);


				
		}
		printf("\n");
		printf("sm3  cell_num:");
		for(i = 0;i < 8;i++)
		{

					printf("%c",sm->cell_num[i]);
							
		}
		printf("\n");
		
		if(strncmp(sm->build_num,"FF",2) == 0)
		{
						build_type = 1;
		}
		else
		{
						build_type = 2;
		}
		
		return 0;
	
	
}

