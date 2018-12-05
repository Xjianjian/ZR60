

#include "./wdata/wdata.h"

#include "usart/bsp_debug_usart.h"
#include "./FATFS/ff.h" 

#include "string.h"
#include "Include.h"



int black_n = 0;				//黑名单具体数量
FIL file_name;							/* 文件对象 */
UINT name_num;            					  /* 文件成功读写量 */
FRESULT res = FR_OK;


static uint8_t wdata_HexToChar(uint8_t temp);

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
	//int i = 0;

	UINT name_n = 0;
	res = f_open(&file_name,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		//USART_PRINTF_S("打开写卡文件失败\n");
		return -1;		
	}

	USART_PRINTF_D("file_name.fsize: %d\n",file_name.fsize);

	res = f_write(&file_name, sm3_buf,sizeof(struct comm_info), &name_n);

	if(res != FR_OK)
	{
		USART_PRINTF_S("写入母卡数据失败  X\n");
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
	//int i = 0;

	UINT name_n = 0;	
	
	
		/*将写入文件的信息重新读出来*/
	res = f_open(&file_name,filename,FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		USART_PRINTF_S("打开写卡文件失败  X\n");
		return -1;
	}
	
	name_n = 0;
	res = f_read(&file_name,sm3_buf,sizeof(struct comm_info), &name_n);
	if(res != FR_OK)
	{
		USART_PRINTF_S("读取写卡文件失败  X\n");
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

void read_init_sminfo(struct comm_info *sm3_buf,struct sm3_info *sm)
{
	u8 i;

	memset(sm->community_id,'F',8U);
	memset(sm->build_num,'F',8U);
	memset(sm->cell_num,'F',8U);
	for(i = 0;i < 4U;i++)
	{
		sm->community_id[2*i] = wdata_HexToChar(sm3_buf->community_id[i]>>4);
		sm->community_id[2*i+1] = wdata_HexToChar(sm3_buf->community_id[i]&0xf);
	}

	if((sm3_buf->build_numOne[0] == 0xAA) && (sm3_buf->build_numOne[12] == 0xAA))
	{//围墙机0x15为全A
		machine_type = 1;
		printf("\n【4】当前设备为围墙机  √\n");
	}
	else
	{//当门口机处理（需要验证 小区密码 和 楼栋密码）
		machine_type = 2;
		printf("\n【4】当前设备为门口机  √\n");	
		for(i = 0;i < 2U;i++)
		{
			sm->build_num[2*i] = wdata_HexToChar(sm3_buf->build_numOne[i]>>4);
			sm->build_num[2*i+1] = wdata_HexToChar(sm3_buf->build_numOne[i]&0xf);
		}
	}
}


static uint8_t wdata_HexToChar(uint8_t temp)
{
    uint8_t dst;
    if (temp < 10)
	{
        dst = temp + '0';
    }
	else
	{
        dst = temp -10 +'A';
    }
    return dst;
}
