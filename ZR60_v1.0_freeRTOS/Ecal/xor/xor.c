/******************************************************
�ļ�����	xor.c

������		
Data			Vasion			author
2018/7/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "xor.h"


/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/



/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������xor_checkSum

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
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

