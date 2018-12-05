

#include "./wdata/wdata.h"

#include "usart/bsp_debug_usart.h"
#include "./FATFS/ff.h" 
#include "./malloc/malloc.h"

#include "string.h"


	
uint8_t LCD_CODE[]="0123456789ABCDEF";


	extern  u8 build_type;
int black_n = 0;				//黑名单具体数量
FIL file_name;							/* 文件对象 */
UINT name_num;            					  /* 文件成功读写量 */
FRESULT res = FR_OK;
/***
功能：创建文件，储存卡信息
参数：
filename：文件名及其路径 如 "1:/写卡.txt
sm3_buf: ic卡信息：
				struct comm_info //楼栋信息
				{
						unsigned char community_id[16];//小区id
						unsigned char build_num[16];//楼栋号
						unsigned char cell_num[16];//单元号,写死
				}


***/

int add_data(const char *filename,struct comm_info *sm3_buf)
{
	int i = 0;

	int name_n = 0;
	res = f_open(&file_name,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res == FR_OK)
	{
		printf("打开写卡文件成功\n");
	}
	else
	{
		printf("打开写卡文件失败\n");
		return -1;
	}
	
	//printf("添加函数第一次查看：file_name.fsize = %d\n",file_name.fsize);

	res = f_write(&file_name, sm3_buf,sizeof(struct comm_info), &name_n);

	if(res == FR_OK)
	{
		printf("写入母卡数据成功\n");
	}
	else
	{
		printf("写入母卡数据失败\n");
			f_close(&file_name);
		return -1;
	}	

	f_close(&file_name);
	
	return 0;
	

}
/**
 功能：读取文件里面的卡信息
参数：
filename：文件名及其路径 如 "1:/写卡.txt
sm3_buf: 将文件的信息存到结构体sm3_buf
				struct comm_info //楼栋信息
				{
						unsigned char community_id[16];//小区id
						unsigned char build_num[16];//楼栋号
						unsigned char cell_num[16];//单元号,写死
				}


**/


int read_card_data(const char *filename,struct comm_info *sm3_buf)
{
	int i = 0;

	int name_n = 0;	
	
	
		/*将写入文件的信息重新读出来*/
	res = f_open(&file_name,filename,FA_OPEN_EXISTING | FA_READ);
	if(res == FR_OK)
	{
		printf("打开写卡文件成功\n");
	}
	else
	{
		printf("打开写卡文件失败\n");
		return -1;
	}	
	name_n = 0;
	res = f_read(&file_name,sm3_buf,sizeof(struct comm_info), &name_n);
	if(res == FR_OK)
	{
		printf("读取写卡文件成功\n");
		printf("添加黑名单信息后，读取的有效字节数为：name_n= %d\n",name_n);
			
	}
	else
	{
		printf("读取写卡文件失败\n");
		return -1;
	}

	f_close(&file_name);
	return 0;
	
}
/**
功能：用于在初始化的时候读取ic卡文件的数据，并截取ic卡小区号，楼栋号，单元号的前8位用于sm3密码验证
参数：
filename：文件名及其路径
sm3_buf：将文件的信息存到结构体sm3_buf
sm：ic卡小区号，楼栋号，单元号的前8位存于结构体sm

**/

int read_init_sminfo(const char *filename,struct comm_info *sm3_buf,struct sm3_info *sm)
{
	u8 i;
		if(read_card_data(filename,sm3_buf) != 0)
		{
				
			return -1;
		}
		printf("读取初始化门口信息成功\n");
		
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

