/******************************************************
文件名：	crc32.c

描述：		
Data			Vasion			author
2018/7/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "crc32.h"


/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/
static uint32 crc_table[256];
static uint8  have_table = 0U;


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/

static void init_crc_table(void);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：MemIf_Init

*形  参：void

*返回值：void

*描  述： * 初始化crc表,生成32位大小的crc表 
 * 也可以直接定义出crc表,直接查表, 
 * 但总共有256个,看着眼花,用生成的比较方便. 

*备  注： x32  则对应32bit = 1, x26 则对应26bit=1，得出一个值：
(1<<32)|(1<<26)|(1<<23)|(1<<22)|...|(1<<1)|(1)=0x104C11DB7，对于CRC32取低32位，则＝0x4C11DB7
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
*函数名：crc32

*形  参：void

*返回值：void

*描  述：

*备  注：
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

