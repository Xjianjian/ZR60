#ifndef __WDATA_H
#define	__WDATA_H

#include "stm32f4xx.h"

struct comm_info //楼栋信息
{
		unsigned char community_id[16];//小区id
		unsigned char build_num[16];//楼栋号
		unsigned char cell_num[16];//单元号,写死
};

struct sm3_info //楼栋信息
{
		unsigned char community_id[8];//小区id
		unsigned char build_num[8];//楼栋号
		unsigned char cell_num[8];//单元号,写死
};



int add_data(const char *filename,struct comm_info *sm3_buf);
int read_card_data(const char *filename,struct comm_info *sm3_buf);
int read_init_sminfo(const char *filename,struct comm_info *sm3_buf,struct sm3_info *sm);
#endif
