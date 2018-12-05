#ifndef __WDATA_H
#define	__WDATA_H

#include "stm32f4xx.h"

struct comm_info //¥����Ϣ
{
		unsigned char community_id[16];//С��id
		unsigned char build_num[16];//¥����
		unsigned char cell_num[16];//��Ԫ��,д��
};

struct sm3_info //¥����Ϣ
{
		unsigned char community_id[8];//С��id
		unsigned char build_num[8];//¥����
		unsigned char cell_num[8];//��Ԫ��,д��
};



int add_data(const char *filename,struct comm_info *sm3_buf);
int read_card_data(const char *filename,struct comm_info *sm3_buf);
int read_init_sminfo(const char *filename,struct comm_info *sm3_buf,struct sm3_info *sm);
#endif
