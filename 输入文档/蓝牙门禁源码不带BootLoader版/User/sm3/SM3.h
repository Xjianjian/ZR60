/*
 ============================================================================
 Name        : SM3.h
 Author      : 邓燎燕
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 算法参考 http://www.oscca.gov.cn/UpFile/20101222141857786.pdf
 */

#ifndef SM3_H
#define SM3_H


#include "stm32f4xx.h"

/**
 * 获取SM3
 * @param input_m 输入信息
 * @param input_m_len 输入信息长度
 * @param output_m 输出信息
 */
void sm3(unsigned char * input_m, unsigned int input_m_len, unsigned char * output_m);
/*
int get_sm3_pass(unsigned long now_time,
	 unsigned	 char * community_id,
	unsigned char * build_num,
	unsigned	 char * cell_num);
*/	
	int get_sm3_pass(unsigned long now_time,unsigned  char *community_id,unsigned char *build_num,unsigned char *cell_num); 

#endif
