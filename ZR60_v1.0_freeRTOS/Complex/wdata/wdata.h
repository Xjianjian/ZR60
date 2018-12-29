#ifndef __WDATA_H
#define	__WDATA_H

#include "stm32f4xx.h"

struct comm_info //楼栋信息
{
	unsigned char community_id[16];//小区id
	unsigned char build_numOne[16];//楼栋编号列表1(0x15)
	unsigned char build_numTwo[16];//楼栋编号列表2(0x16)
	unsigned char build_id[16];//楼栋id
	unsigned char door_id[16];//门id
	unsigned short suffix;//下标
	//unsigned char BListFlag;//黑白名单标志:0--黑名单；1--白名单
};

struct sm3_info //楼栋信息
{
	unsigned char community_id[8];//小区id
	unsigned char build_num[8];//楼栋号
	unsigned char cell_num[8];//单元号
	unsigned char null;//空字符'\0'
};



int add_data(const char *filename,struct comm_info *sm3_buf);
int read_card_data(const char *filename,struct comm_info *sm3_buf);
void read_init_sminfo(struct comm_info *sm3_buf,struct sm3_info *sm);
#endif
