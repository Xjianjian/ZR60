/******************************************************
文件名：	xor.c
描述：		
Data			Vasion			author
2018/7/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "xor.h"


/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/



/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：xor_checkSum
*形  参：
*返回值：
*描  述：异或取反校验
*备  注：
******************************************************/
unsigned long xor_checkSum(unsigned char* Le_u_Dt,unsigned int Le_w_Lng)
{
	unsigned int Le_w_i;
	unsigned char Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return (unsigned long)Le_u_Xor;
}
