/******************************************************
�ļ�����	crc32.c

������		
Data			Vasion			author
2018/7/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "crc32.h"


/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/
static uint32 crc_table[256];
static uint8  have_table = 0U;


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/

static void init_crc_table(void);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������MemIf_Init

*��  �Σ�void

*����ֵ��void

*��  ���� * ��ʼ��crc��,����32λ��С��crc�� 
 * Ҳ����ֱ�Ӷ����crc��,ֱ�Ӳ��, 
 * ���ܹ���256��,�����ۻ�,�����ɵıȽϷ���. 

*��  ע�� x32  ���Ӧ32bit = 1, x26 ���Ӧ26bit=1���ó�һ��ֵ��
(1<<32)|(1<<26)|(1<<23)|(1<<22)|...|(1<<1)|(1)=0x104C11DB7������CRC32ȡ��32λ����0x4C11DB7
******************************************************/
static void init_crc_table(void)  
{  
	uint32 c;  
	uint32 i, j;  
 
	have_table = 1U;
	for (i = 0; i < 256; i++) 
	{  
		c = i;  
 
		for (j = 0; j < 8; j++) 
		{  
			if (c & 1)  
				c = 0xEDB88320 ^ (c >> 1);  
			else  
				c = c >> 1;  
		}  
 
		crc_table[i] = c;  
	}  
}  


/******************************************************
*��������crc32

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��
******************************************************/
uint32 crc32(uint32 Le_dw_crc,uint8* Le_u_Dt, uint32 Le_dw_Lng)
{
	uint32 Le_dw_i;  
    unsigned char Le_u_index;
	if(!have_table)
	{
		init_crc_table();
	}
	
    for(Le_dw_i = 0U;Le_dw_i < Le_dw_Lng;Le_dw_i++)
    {
        Le_u_index = (unsigned char)(Le_dw_crc^Le_u_Dt[Le_dw_i]); 
        Le_dw_crc = ((Le_dw_crc>>8)^crc_table[Le_u_index]);
    }
    return (Le_dw_crc ^= 0xFFFFFFFF);
}

