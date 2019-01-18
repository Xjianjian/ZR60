/******************************************************
文件名：	FormatConv.c

描述：		
Data			Vasion			author
2019/1/18		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "FormatConv.h"


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
*函数名：FormatConv_StrToHex

*形  参：void

*返回值：void

*描  述：字符串转Hex格式:AABBCCDD转为0xAA,0xBB,0xCC,0xDD

*备  注：
******************************************************/
void FormatConv_StrToHex(char* Le_in,unsigned char* Le_out,unsigned int Le_w_lng)
{
	unsigned int Le_w_i;
	for(Le_w_i = 0U;Le_w_i < Le_w_lng;Le_w_i++)
	{
		if((Le_in[2*Le_w_i] >= '0') && (Le_in[2*Le_w_i] <= '9'))
		{
			Le_out[Le_w_i] = ((Le_in[2*Le_w_i]-0x30) << 4U);
		}
		else
		{
			Le_out[Le_w_i] = ((Le_in[2*Le_w_i]-0x37) << 4U);
		}
		
		if((Le_in[2*Le_w_i+1] >= '0') && (Le_in[2*Le_w_i+1] <= '9'))
		{
			Le_out[Le_w_i] |= ((Le_in[2*Le_w_i + 1] -0x30) & 0x0f);
		}
		else
		{
			Le_out[Le_w_i] |= ((Le_in[2*Le_w_i + 1] -0x37) & 0x0f);
		}
	}
}

/******************************************************
*函数名：FormatConv_MacAddFlags

*形  参：void

*返回值：void

*描  述：mac地址插入':'

*备  注：
   地址重组：字符串112233445566重组为字符串11:22:33:44:55:66
******************************************************/
void FormatConv_MacAddFlags(char* Le_u_mac)
{
	char Le_u_i;
	char La_u_TempMac[13U] = {0};
	for(Le_u_i=0U;Le_u_i < 13U;Le_u_i++)
	{
		La_u_TempMac[Le_u_i] = Le_u_mac[Le_u_i];
	}
	for(Le_u_i=0U;Le_u_i < 6U;Le_u_i++)
	{
		Le_u_mac[(3*(Le_u_i+1)-3)] = La_u_TempMac[(2*Le_u_i)];
		Le_u_mac[(3*(Le_u_i+1)-2)] = La_u_TempMac[(2*Le_u_i+1)];
		if(Le_u_i == 5U)
		{
			Le_u_mac[(3*(Le_u_i+1)-1)] = '\0';
		}
		else
		{
			Le_u_mac[(3*(Le_u_i+1)-1)] = ':';
		}
	}
}

/******************************************************
*函数名：FormatConv_HexToChar

*形  参：void

*返回值：void

*描  述：十六进制0~F转为字符'0'~'F'

*备  注：
******************************************************/
char FormatConv_HexToChar(unsigned char Le_u_dt)
{
    char dst;
    if (Le_u_dt < 10)
	{
        dst = Le_u_dt + '0';
    }
	else
	{
        dst = Le_u_dt - 10 + 'A';
    }
    return dst;
}


/******************************************************
*函数名：FormatConv_IpAddComma

*形  参：Le_dw_in:十进制ip，4字节
				 ip地址存放规则：192.168.1.1
			bit31 _______________________  bit24
				 | 	|  |  |  |  |  |  |  |
				 |__|__|__|__|__|__|__|__|      1
				 
			bit23 _______________________  bit16
				 | 	|  |  |  |  |  |  |  |
				 |__|__|__|__|__|__|__|__|      1
				 
			bit15 _______________________  bit8
				 | 	|  |  |  |  |  |  |  |
				 |__|__|__|__|__|__|__|__|      168
				 
			bit7 _______________________  bit0
				 | 	|  |  |  |  |  |  |  |
				 |__|__|__|__|__|__|__|__|      192
		
	   Le_u_out: 转换后的ip，格式:192.168.1.1
*返回值：void

*描  述：十进制 ip转换成x.x.x.x 

*备  注：
******************************************************/
void FormatConv_IpAddComma(unsigned long Le_dw_in,char* Le_u_out)
{
	uint8_t ip_int_index[4U];
	uint8_t ip_temp_numbr = 24U;
	uint8_t Le_u_j;
	for(Le_u_j=0;Le_u_j<4;Le_u_j++)  
	{  
		ip_int_index[Le_u_j]=((Le_dw_in >> ip_temp_numbr)&0xFF);  
		ip_temp_numbr-=8;  
	}  

	sprintf(Le_u_out,"%d.%d.%d.%d",ip_int_index[3],ip_int_index[2],ip_int_index[1],ip_int_index[0]); 
}
