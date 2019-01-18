/******************************************************
�ļ�����	FormatConv.c

������		
Data			Vasion			author
2019/1/18		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "FormatConv.h"


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
*��������FormatConv_StrToHex

*��  �Σ�void

*����ֵ��void

*��  �����ַ���תHex��ʽ:AABBCCDDתΪ0xAA,0xBB,0xCC,0xDD

*��  ע��
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
*��������FormatConv_MacAddFlags

*��  �Σ�void

*����ֵ��void

*��  ����mac��ַ����':'

*��  ע��
   ��ַ���飺�ַ���112233445566����Ϊ�ַ���11:22:33:44:55:66
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
*��������FormatConv_HexToChar

*��  �Σ�void

*����ֵ��void

*��  ����ʮ������0~FתΪ�ַ�'0'~'F'

*��  ע��
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
*��������FormatConv_IpAddComma

*��  �Σ�Le_dw_in:ʮ����ip��4�ֽ�
				 ip��ַ��Ź���192.168.1.1
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
		
	   Le_u_out: ת�����ip����ʽ:192.168.1.1
*����ֵ��void

*��  ����ʮ���� ipת����x.x.x.x 

*��  ע��
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
