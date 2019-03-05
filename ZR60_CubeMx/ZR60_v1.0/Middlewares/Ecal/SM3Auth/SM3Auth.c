/******************************************************
文件名：	SM3Auth.c

描述:	
Data			Vasion			author
2018/1/4		V1.0			liujian
*******************************************************/

/*******************************************************
description锛include the header file
*******************************************************/
#include "SM3Auth/SM3Auth.h"


/*******************************************************
description锛global variable definitions
*******************************************************/


/*******************************************************
description锛static variable definitions
*******************************************************/
/*
sm3密码生成使用的数据信息
围墙机：community_id为小区id前4字节；build_num和cell_num全F
门口机：community_id为小区id前4字节；build_num前4字节为楼栋编号，其余全F;cell_num全F
*/
static struct sm3AuthInfo sm3Auth = {{'F','F','F','F','F','F','F','F'},{'F','F','F','F','F','F','F','F'}, \
															{'F','F','F','F','F','F','F','F'},'\0'};


/*******************************************************
description锛function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static unsigned int SM3Auth_localgeneratePassword(unsigned long timestamp,unsigned  char *community_id, \
									unsigned char *build_num,unsigned char *cell_num);
static uint8 SM3Auth_HexToChar(uint8 temp);
/******************************************************
description锛function code
******************************************************/
/******************************************************
*函数：InitSM3Auth_Paramter

*褰㈠弬锛?

*杩斿洖鍊硷細

*描述:初始化

*澶囨敞锛?
******************************************************/
void InitSM3Auth_Paramter(void* SM3Info,uint8 machineType)
{
	uint8 Le_u_i;
	char* ptr;

	ptr = (char*)SM3Info;
	memset(sm3Auth.community_id,'F',8U);
	memset(sm3Auth.build_num,'F',8U);
	memset(sm3Auth.cell_num,'F',8U);
	for(Le_u_i = 0;Le_u_i < 4U;Le_u_i++)
	{
		sm3Auth.community_id[2*Le_u_i] = SM3Auth_HexToChar(ptr[Le_u_i]>>4);
		sm3Auth.community_id[2*Le_u_i+1] = SM3Auth_HexToChar(ptr[Le_u_i]&0xf);
	}

	if(2 == machineType)
	{//当门口机处理（需要验证 小区密码 和 楼栋密码）
		for(Le_u_i = 0;Le_u_i < 2U;Le_u_i++)
		{
			sm3Auth.build_num[2*Le_u_i] = SM3Auth_HexToChar(ptr[16+Le_u_i]>>4);
			sm3Auth.build_num[2*Le_u_i+1] = SM3Auth_HexToChar(ptr[16+Le_u_i]&0xf);
		}
	}
	else//围墙机
	{}
}

/******************************************************
*函数：SM3Auth_PasswordAuth

*褰㈠弬锛?

*杩斿洖鍊硷細

*描述:密码验证

*澶囨敞锛?
******************************************************/
uint8_t SM3Auth_PasswordAuth(uint32_t timestamp,uint8 mach_type,uint32 Password,uint8 pwType)
{
	uint8 AythCnt = 0U;//验证次数
	unsigned char build_num[8U] ={'F','F','F','F','F','F','F','F'};
	unsigned long nowtimestamp ;
	uint32 authpasswd ;//验证密码
	uint32 localpasswd ;//本地生成密码
	uint32 a_passwd,b_passwd;
	uint8 Le_u_i;
	
	if(mach_type == 1)//围墙机
	{
		authpasswd =  Password/100000*100 + (Password/1000%10)*10 + Password/10%10;
	}
	else//门口机
	{
		authpasswd = Password;
	}
	SM3AUTH_PRINTF_D("验证密码是 %ld\n",authpasswd);
	
	switch(pwType)
	{
		case PASSWORD_TEMPORARY:
		{
			nowtimestamp  = timestamp/60 - 60;
			AythCnt = 64U;
		}
		break;
		case PASSWORD_BLE:
		{
			nowtimestamp  = timestamp/60 - 5;
			AythCnt = 8U;
		}
		default:
		break;				 	 
	}
			
	for(Le_u_i = 0; Le_u_i < AythCnt;Le_u_i++)
	{
		if(mach_type == 2)
		{
			a_passwd = SM3Auth_localgeneratePassword(nowtimestamp, sm3Auth.community_id, sm3Auth.build_num, sm3Auth.cell_num);
			b_passwd = SM3Auth_localgeneratePassword(nowtimestamp, sm3Auth.community_id,build_num, sm3Auth.cell_num);
			localpasswd = (b_passwd /100) *100000 + (a_passwd / 100) *10000 + (b_passwd / 10 %10) * 1000 \
						+ (a_passwd / 10 %10) * 100 + (b_passwd  %10) *10 + a_passwd % 10 ;
			SM3AUTH_PRINTF_D("本地生成密码: %ld\n",localpasswd);
			if(localpasswd == authpasswd)
			{
				SM3AUTH_PRINTF_S("门口密码验证通过\n");
				return 1;	
			}
		}
		else
		{
			localpasswd = SM3Auth_localgeneratePassword(nowtimestamp, sm3Auth.community_id, sm3Auth.build_num, sm3Auth.cell_num);
			SM3AUTH_PRINTF_D("本地生成密码: %ld\n",localpasswd);
			if(localpasswd == authpasswd)
			{
				SM3AUTH_PRINTF_S("围墙机密码验证通过\n");
				return 1;	
			}
		}
		nowtimestamp++;
	}
	SM3AUTH_PRINTF_D("验证时，时间戳%ld\n",timestamp);			
	SM3AUTH_PRINTF_D("验证时，验证数据信息%s\n",(char*)sm3Auth.community_id);
	return 0;
}

/******************************************************
*鍑芥暟鍚嶏細SM3Auth_localgeneratePassword

*褰 鍙傦細

*杩斿洖鍊硷細

*描述：生成本地密码

*澶 娉細
******************************************************/
static unsigned int SM3Auth_localgeneratePassword(unsigned long timestamp,unsigned  char *community_id, \
													unsigned char *build_num,unsigned char *cell_num) 
{
    unsigned char index; 
	uint64_t sum_i = 0;
	unsigned char p0[32] = {0};
	unsigned long is[8] = {0};
	unsigned long _d = 10;
	unsigned char j;
    unsigned char s[32] = 
	{
            (unsigned char)(timestamp & 0xff), 			(unsigned char)((timestamp >> 8) & 0xff), //时间戳
			(unsigned char)((timestamp >> 16) & 0xff), 	(unsigned char)((timestamp >> 24) & 0xff), 
			0, 											0, 										 
			0, 											0,
            community_id[0], 							community_id[1],//小区ID 						 
			community_id[2], 							community_id[3], 
			community_id[4], 							community_id[5],						 
			community_id[6], 							community_id[7],
            build_num[0], 								build_num[1],//楼栋编号							 
			build_num[2], 								build_num[3], 
			build_num[4], 								build_num[5], 							 
			build_num[6],								build_num[7],
            cell_num[0], 								cell_num[1], 							 
			cell_num[2], 								cell_num[3],//
			cell_num[4], 								cell_num[5], 							 
			cell_num[6], 								cell_num[7]
    }; //鍗曞厓缂栧彿
	
    sm3(s, 32, p0);
	
    for(index = 0; index < 8; index++)
	{
        is[index] = p0[index * 4 + 0] << 24 | p0[index * 4 + 1] << 16 | \
									p0[index * 4 + 2] << 8 | p0[index * 4 + 3];
    }
    
    for(index = 0; index < 8; index++)
	{
        sum_i = sum_i + is[index];
    }
		
    sum_i = (sum_i % 0xffffffff);

    for (j = 1; j < 3; j++) 
	{
        _d = _d * 10;
    }

    return (unsigned int)(sum_i % _d);
}

static uint8 SM3Auth_HexToChar(uint8 temp)
{
    uint8_t dst;
    if (temp < 10)
	{
        dst = temp + '0';
    }
	else
	{
        dst = temp -10 +'A';
    }
    return dst;
}
