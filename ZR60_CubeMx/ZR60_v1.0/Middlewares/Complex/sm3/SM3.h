#ifndef SM3_H
#define SM3_H


#include "stm32f4xx.h"


extern	void sm3(unsigned char * input_m, unsigned int input_m_len, unsigned char * output_m);
extern	int get_sm3_pass(unsigned long now_time,unsigned  char *community_id,unsigned char *build_num,unsigned char *cell_num); 

#endif
