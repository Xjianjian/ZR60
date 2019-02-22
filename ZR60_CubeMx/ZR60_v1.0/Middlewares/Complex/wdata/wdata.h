#ifndef __WDATA_H
#define	__WDATA_H

#include "stm32f4xx.h"

struct comm_info //¥����Ϣ
{
	unsigned char community_id[16];//С��id
	unsigned char build_numOne[16];//¥������б�1(0x15)
	unsigned char build_numTwo[16];//¥������б�2(0x16)
	unsigned char build_id[16];//¥��id
	unsigned char door_id[16];//��id
	unsigned short suffix;//�±�
	//unsigned char BListFlag;//�ڰ�������־:0--��������1--������
};

struct sm3_info //¥����Ϣ
{
	unsigned char community_id[8];//С��id
	unsigned char build_num[8];//¥����
	unsigned char cell_num[8];//��Ԫ��
	unsigned char null;//���ַ�'\0'
};



int add_data(const char *filename,struct comm_info *sm3_buf);
int read_card_data(const char *filename,struct comm_info *sm3_buf);
void read_init_sminfo(struct comm_info *sm3_buf,struct sm3_info *sm);
#endif
