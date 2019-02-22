

#include "./wdata/wdata.h"

#include "usart/bsp_debug_usart.h"
#include "./FATFS/ff.h" 

#include "string.h"
#include "Include.h"



int black_n = 0;				//��������������
FIL file_name;							/* �ļ����� */
UINT name_num;            					  /* �ļ��ɹ���д�� */
FRESULT res = FR_OK;


static uint8_t wdata_HexToChar(uint8_t temp);

/***
���ܣ������ļ������濨��Ϣ
������
filename���ļ�������·�� �� "1:/д��.txt
sm3_buf: ic����Ϣ��
				struct comm_info //¥����Ϣ
				{
						unsigned char community_id[16];//С��id
						unsigned char build_num[16];//¥����
						unsigned char cell_num[16];//��Ԫ��,д��
				}


***/

int add_data(const char *filename,struct comm_info *sm3_buf)
{
	//int i = 0;

	UINT name_n = 0;
	res = f_open(&file_name,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		//USART_PRINTF_S("��д���ļ�ʧ��\n");
		return -1;		
	}

	USART_PRINTF_D("file_name.fsize: %d\n",file_name.fsize);

	res = f_write(&file_name, sm3_buf,sizeof(struct comm_info), &name_n);

	if(res != FR_OK)
	{
		USART_PRINTF_S("д��ĸ������ʧ��  X\n");
		f_close(&file_name);
		return -1;
	}

	f_close(&file_name);
	
	return 0;
	

}
/**
 ���ܣ���ȡ�ļ�����Ŀ���Ϣ
������
filename���ļ�������·�� �� "1:/д��.txt
sm3_buf: ���ļ�����Ϣ�浽�ṹ��sm3_buf
				struct comm_info //¥����Ϣ
				{
						unsigned char community_id[16];//С��id
						unsigned char build_num[16];//¥����
						unsigned char cell_num[16];//��Ԫ��,д��
				}


**/


int read_card_data(const char *filename,struct comm_info *sm3_buf)
{
	//int i = 0;

	UINT name_n = 0;	
	
	
		/*��д���ļ�����Ϣ���¶�����*/
	res = f_open(&file_name,filename,FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		USART_PRINTF_S("��д���ļ�ʧ��  X\n");
		return -1;
	}
	
	name_n = 0;
	res = f_read(&file_name,sm3_buf,sizeof(struct comm_info), &name_n);
	if(res != FR_OK)
	{
		USART_PRINTF_S("��ȡд���ļ�ʧ��  X\n");
		return -1;	
	}


	f_close(&file_name);
	return 0;
	
}
/**
���ܣ������ڳ�ʼ����ʱ���ȡic���ļ������ݣ�����ȡic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ����sm3������֤
������
filename���ļ�������·��
sm3_buf�����ļ�����Ϣ�浽�ṹ��sm3_buf
sm��ic��С���ţ�¥���ţ���Ԫ�ŵ�ǰ8λ���ڽṹ��sm

**/

void read_init_sminfo(struct comm_info *sm3_buf,struct sm3_info *sm)
{
	u8 i;

	memset(sm->community_id,'F',8U);
	memset(sm->build_num,'F',8U);
	memset(sm->cell_num,'F',8U);
	for(i = 0;i < 4U;i++)
	{
		sm->community_id[2*i] = wdata_HexToChar(sm3_buf->community_id[i]>>4);
		sm->community_id[2*i+1] = wdata_HexToChar(sm3_buf->community_id[i]&0xf);
	}

	if((sm3_buf->build_numOne[0] == 0xAA) && (sm3_buf->build_numOne[12] == 0xAA))
	{//Χǽ��0x15ΪȫA
		machine_type = 1;
		printf("\n��4����ǰ�豸ΪΧǽ��  ��\n");
	}
	else
	{//���ſڻ�������Ҫ��֤ С������ �� ¥�����룩
		machine_type = 2;
		printf("\n��4����ǰ�豸Ϊ�ſڻ�  ��\n");	
		for(i = 0;i < 2U;i++)
		{
			sm->build_num[2*i] = wdata_HexToChar(sm3_buf->build_numOne[i]>>4);
			sm->build_num[2*i+1] = wdata_HexToChar(sm3_buf->build_numOne[i]&0xf);
		}
	}
}


static uint8_t wdata_HexToChar(uint8_t temp)
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
