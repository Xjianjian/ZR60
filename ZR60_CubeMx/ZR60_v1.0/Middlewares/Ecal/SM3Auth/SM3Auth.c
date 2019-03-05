/******************************************************
�ļ�����	SM3Auth.c

����:	
Data			Vasion			author
2018/1/4		V1.0			liujian
*******************************************************/

/*******************************************************
description�include the header file
*******************************************************/
#include "SM3Auth/SM3Auth.h"


/*******************************************************
description�global variable definitions
*******************************************************/


/*******************************************************
description�static variable definitions
*******************************************************/
/*
sm3��������ʹ�õ�������Ϣ
Χǽ����community_idΪС��idǰ4�ֽڣ�build_num��cell_numȫF
�ſڻ���community_idΪС��idǰ4�ֽڣ�build_numǰ4�ֽ�Ϊ¥����ţ�����ȫF;cell_numȫF
*/
static struct sm3AuthInfo sm3Auth = {{'F','F','F','F','F','F','F','F'},{'F','F','F','F','F','F','F','F'}, \
															{'F','F','F','F','F','F','F','F'},'\0'};


/*******************************************************
description�function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static unsigned int SM3Auth_localgeneratePassword(unsigned long timestamp,unsigned  char *community_id, \
									unsigned char *build_num,unsigned char *cell_num);
static uint8 SM3Auth_HexToChar(uint8 temp);
/******************************************************
description�function code
******************************************************/
/******************************************************
*������InitSM3Auth_Paramter

*形参�?

*返回值：

*����:��ʼ��

*备注�?
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
	{//���ſڻ�������Ҫ��֤ С������ �� ¥�����룩
		for(Le_u_i = 0;Le_u_i < 2U;Le_u_i++)
		{
			sm3Auth.build_num[2*Le_u_i] = SM3Auth_HexToChar(ptr[16+Le_u_i]>>4);
			sm3Auth.build_num[2*Le_u_i+1] = SM3Auth_HexToChar(ptr[16+Le_u_i]&0xf);
		}
	}
	else//Χǽ��
	{}
}

/******************************************************
*������SM3Auth_PasswordAuth

*形参�?

*返回值：

*����:������֤

*备注�?
******************************************************/
uint8_t SM3Auth_PasswordAuth(uint32_t timestamp,uint8 mach_type,uint32 Password,uint8 pwType)
{
	uint8 AythCnt = 0U;//��֤����
	unsigned char build_num[8U] ={'F','F','F','F','F','F','F','F'};
	unsigned long nowtimestamp ;
	uint32 authpasswd ;//��֤����
	uint32 localpasswd ;//������������
	uint32 a_passwd,b_passwd;
	uint8 Le_u_i;
	
	if(mach_type == 1)//Χǽ��
	{
		authpasswd =  Password/100000*100 + (Password/1000%10)*10 + Password/10%10;
	}
	else//�ſڻ�
	{
		authpasswd = Password;
	}
	SM3AUTH_PRINTF_D("��֤������ %ld\n",authpasswd);
	
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
			SM3AUTH_PRINTF_D("������������: %ld\n",localpasswd);
			if(localpasswd == authpasswd)
			{
				SM3AUTH_PRINTF_S("�ſ�������֤ͨ��\n");
				return 1;	
			}
		}
		else
		{
			localpasswd = SM3Auth_localgeneratePassword(nowtimestamp, sm3Auth.community_id, sm3Auth.build_num, sm3Auth.cell_num);
			SM3AUTH_PRINTF_D("������������: %ld\n",localpasswd);
			if(localpasswd == authpasswd)
			{
				SM3AUTH_PRINTF_S("Χǽ��������֤ͨ��\n");
				return 1;	
			}
		}
		nowtimestamp++;
	}
	SM3AUTH_PRINTF_D("��֤ʱ��ʱ���%ld\n",timestamp);			
	SM3AUTH_PRINTF_D("��֤ʱ����֤������Ϣ%s\n",(char*)sm3Auth.community_id);
	return 0;
}

/******************************************************
*函数名：SM3Auth_localgeneratePassword

*� 参：

*返回值：

*���������ɱ�������

*� 注：
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
            (unsigned char)(timestamp & 0xff), 			(unsigned char)((timestamp >> 8) & 0xff), //ʱ���
			(unsigned char)((timestamp >> 16) & 0xff), 	(unsigned char)((timestamp >> 24) & 0xff), 
			0, 											0, 										 
			0, 											0,
            community_id[0], 							community_id[1],//С��ID 						 
			community_id[2], 							community_id[3], 
			community_id[4], 							community_id[5],						 
			community_id[6], 							community_id[7],
            build_num[0], 								build_num[1],//¥�����							 
			build_num[2], 								build_num[3], 
			build_num[4], 								build_num[5], 							 
			build_num[6],								build_num[7],
            cell_num[0], 								cell_num[1], 							 
			cell_num[2], 								cell_num[3],//
			cell_num[4], 								cell_num[5], 							 
			cell_num[6], 								cell_num[7]
    }; //单元编号
	
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
