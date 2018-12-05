/******************************************************
�ļ�����	readcard.c

������		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./usart3/usart3.h"
#include "./readcard/readcard.h"
#include "./wdata/wdata.h"
#include "string.h"
 
 
uint8_t volatile Sereadcard_SetKeyflag = 0U; //���ö��������־
uint8_t volatile setcard = 0;//ĸ��������ɱ�־
uint8_t volatile isSectedCard_flag = 1U;//�Զ�Ѱ�����ñ�־��1--�����Զ�Ѱ��
struct comm_info sminfo;//����ĸ��������Ϣ
struct comm_info sminfo1;//����ĸ��������Ϣ
RebackInfo  rebackinfo;//����ic������������
uint8_t volatile open_door = 0;//���ű�־��1--����

CardNumInfo SeReadcard_h_CardID;//��ǰ����ʹ�õ�ic������

/*�������߷�Χ�ڵ����п�*/
static uint8_t Set_Card_ALL[7U] = {0x20,SetDetectCard_ALL, 0x25, 0x01, 0x01, 0x00,0x03};
static uint8_t Sereadcard_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xF5 ,0x79 ,0xED, 0xD8, 0x9F, 0x77 ,0x89, 0x03};		
//static uint8_t Sereadcard_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xFF ,0xFF ,0xFF, 0xFF, 0xFF, 0xFF ,0xD8, 0x03};	
/**
         * ��ȡģ�������:
         * 0x20��ʾ��ʼ��
         * 0x00��ʾ����(��ʵ��������д��ģ��, ģ��ԭ������)
         * 0x22��ʾ�Ƕ�����������
         * 0x01��ʾ����Я��������ֻ��һλ
         * 0x02��ʾ���ζ�ȡ��������02�е�����,
         * 0x00��ʾ������֤��, ��Ҫ����
         * 0x03��ʾ��������Ľ�����
*/
uint8_t Read_Block[7U] = {0x20,0x00, 0x22, 0x01, 0x02, 0x00,0x03};

/*���徲̬����*/
static void calcCommandsBBC(uint8_t *command,uint8_t length);
static void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length);	 
static void decodeLoadKeyResult(RebackInfo rebackInfo);
static void decodeSetDetectCard(RebackInfo rebackInfo);
static void decodeReadBlock(RebackInfo rebackInfo);



/**
     * 
     * �����ڽ��յ�������
     *
     * @param rebackInfo
*/
void handleMsg(RebackInfo rebackInfo) 
{
	//uint8 Le_u_i;
	switch (rebackInfo.Dt.DtSrt.Seqnr) 
	{
		case On_Card :  
		{
			//��ǩλΪ0��ʾ��������
			if(OK == rebackInfo.Dt.DtSrt.St)//��ȡ������Ϣ
			{
				if(0U == GetZR60Ctrl_u_CardSetSt())//�ǿ�Ƭ����״̬
				{
					SeReadcard_h_CardID.lng = rebackInfo.Dt.DtSrt.ValidDt[3];//��ȡ���ų�����Ϣ
					if((SeReadcard_h_CardID.lng == 4U) && (0U == ReadCard_VrfyCardNumber(*((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4]))))))//������֤ͨ��,���Ų��ں�������
					{
						*((uint32*)SeReadcard_h_CardID.CardID) = *((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4])));
						readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));//��ȡ����0x14��С��id
						USART_PRINTF_CARD_NUM("���� %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5],rebackInfo.Dt.DtSrt.ValidDt[6], \
												  rebackInfo.Dt.DtSrt.ValidDt[7]);
						USART_PRINTF_S("��֤ͨ��  ��");
					}
					else
					{
						USART_PRINTF_CARD_NUM("���� %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5],rebackInfo.Dt.DtSrt.ValidDt[6], \
											  rebackInfo.Dt.DtSrt.ValidDt[7]);
						USART_PRINTF_S("��֤��ͨ��  X");
						isSectedCard_flag = 1U;
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
					}
				}
				else
				{
					readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));//��ȡ����0x14
				}
			}
		}
		break;
		case Load_Key://��ǩλΪLoad_Key��ʾ���Ǵӻ���������װ����Կ�ķ���
		{
			USART_PRINTF_S("key�������óɹ�");
			//decodeLoadKeyResult(rebackInfo);
		}
		break;
		case SetDetectCard_ALL://��ʾ���Ǵӻ��������������Զ������ķ���
		{
			USART_PRINTF_S("�Զ�Ѱ�����óɹ�");
			//isSectedCard_flag = 0U;
			//decodeSetDetectCard(rebackInfo);
		}
		break;
		case Block_0x14://��ʾ���Ǵӻ���ȡ����5 0x14�ķ���
		case Block_0x15://��ʾ���Ǵӻ���ȡ����5 0x15�ķ���
		case Block_0x16://��ʾ���Ǵӻ���ȡ����5 0x16�ķ���
		case Block_0x18://��ʾ���Ǵӻ���ȡ����6 0x18�ķ���
		case Block_0x19://��ʾ���Ǵӻ���ȡ����6 0x19�ķ���
		case Block_0x1A://��ʾ���Ǵӻ���ȡ����6 0x1A�ķ���
		{
			decodeReadBlock(rebackInfo);
		}
		break;
		default:
		break;
	}
}

/*
   �����Զ�Ѱ��
*/
void Setreadcard_AutoSearchCard(void)
{
	// �ȼ�����ȷ��У��ֵ
	u8 Le_u_lng;
	Le_u_lng = sizeof(Set_Card_ALL);
	calcCommandsBBC(Set_Card_ALL,Le_u_lng);
	SetUartCmn_TxMsg(Set_Card_ALL,Le_u_lng);
}
/*
	���ö�������
*/
void Setreadcard_Key(void)
{
	SetUartCmn_TxMsg(Sereadcard_u_KeyA,12);
	Sereadcard_SetKeyflag = 1U;
}



/*
   ����У���
*/
static void calcCommandsBBC(uint8_t *command,uint8_t length) 
{
	u8 i;
	uint8_t c = command[1];
	for (i = 2; i < length - 2; i++) 
	{
		c ^= command[i];
	}

	command[length - 2] = (uint8_t) ~c;
}



static void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length) 
{
	blockCommand[4] = block;    // Ҫ��������������
	blockCommand[1] = block;    // ���ζ�ȡ���ݵ�����ͷ
	calcCommandsBBC(blockCommand,length);// �ȼ�����ȷ��У��ֵ
	SetUartCmn_TxMsg(blockCommand,length);
}


static void decodeLoadKeyResult(RebackInfo rebackInfo) 
{
 //   char status = rebackInfo.status;
	switch (rebackInfo.Dt.DtSrt.St) 
	{
		case OK:
			// װ����Կ�ɹ�, ��������������ģ���Զ�����
			//sendCommand(Set_Card_ALL,sizeof(Set_Card_ALL));
		break;
		default:
			// װ����Կ���ɹ�,���·���
							
			//restartMifareRead();
		break;
	}
}
 
/**
     * DB
     * ���������Զ�����
     *
     * @param rebackInfo
*/
static void decodeSetDetectCard(RebackInfo rebackInfo) 
{
	//char status = rebackInfo.status;
	switch (rebackInfo.Dt.DtSrt.St) 
	{
		case OK:
			//�����Զ������ɹ�
			//isSectedCard = true;
		break;
		default:
			// �����Զ�����ʧ��
			// restartMifareRead();
		break;
	}
}

	
/**
     * 
     * �����ڽ��յ�������
     *
     * @param rebackInfo
*/
static void decodeReadBlock(RebackInfo rebackInfo) 
{
	u8 Le_u_i;
	u8 Le_u_matchFlg = 0U;
	switch (rebackInfo.Dt.DtSrt.Seqnr) 
	{
		 case Block_0x14://��ȡС��id
		 {
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//������������֤ʧ��
				{
					Sereadcard_SetKeyflag = 0U;
					USART_PRINTF_S("����5 key������֤ʧ��  X\n");
				}
				// ������ݿ�0x14��ȡ����ʧ��
				isSectedCard_flag = 1;
				USART_PRINTF_S("������5��0x14ʧ��  X\n");
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
			} 
			else 
			{	
				USART_PRINTF_S("������5��0x14�ɹ�  ��\n");
				if(1U == ReadCard_CardSetSt())
				{//��Ƭ����ģʽ
					//("С��id:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{			
						sminfo.community_id[Le_u_i] = rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}
					readBlock(Read_Block, (uint8_t) 0x15,sizeof(Read_Block));
				}
				else
				{//�ǿ�Ƭ����ģʽ���ȶ�С��id
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{	
						if(sminfo1.community_id[Le_u_i] != rebackInfo.Dt.DtSrt.ValidDt[Le_u_i])
						{
							break;
						}
					}
					if(Le_u_i == rebackInfo.Dt.DtSrt.DtLng)
					{//С��idƥ��
						if(1U == ReadCard_DEVICE_TYPE)
						{//Χǽ����С��idƥ��Ϳ��Կ���
							open_door = 1;
							USART_PRINTF_S("Χǽ����С��idƥ�� -> ����\n");
						}
						else if(2U == ReadCard_DEVICE_TYPE)
						{//�ſڻ�������ƥ��¥�����
							readBlock(Read_Block, (uint8_t)0x15,sizeof(Read_Block));
						}
						else
						{
							USART_PRINTF_D("�豸���ʹ���,������ %d\n",ReadCard_DEVICE_TYPE);
						}
					}
					else
					{//С��id��ƥ��
						isSectedCard_flag = 1U;
						USART_PRINTF_S("������5��0x14С��id��ƥ��  X\n");
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
					}
				}
			}
		 }
		break;
		case Block_0x15://��ȡ¥�����1
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("������5��0x15ʧ��  X\n");
			} 
			else
			{	
				USART_PRINTF_S("������5��0x15�ɹ�   ��\n");
				if(1U == GetZR60Ctrl_u_CardSetSt())
				{//��Ƭ����ģʽ
					//("¥�����1:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						sminfo.build_numOne[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		

					readBlock(Read_Block, (uint8_t)0x16,sizeof(Read_Block));
				}
				else
				{//�ǿ�Ƭ����ģʽ���ȶ�¥�����
					if((*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[0U])) == 0xAAAAAAAAAAAAAAAA) && (*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[8U])) == 0xAAAAAAAAAAAAAAAA))
					{//������Ա��
						open_door = 1;
						USART_PRINTF_S("������Ա��\n");
					}
					else
					{
						for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
						{//ÿ��¥�����2�ֽڣ�һ�������װ8��¥�����
							if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								if(*((uint16*)(&sminfo1.build_numOne[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
								{//�ȶ�¥�����
									Le_u_matchFlg = 1U;
									break;
								}
							}
						}
						
						if(1U == Le_u_matchFlg)
						{//ƥ�䵽¥�����
							open_door = 1;
							USART_PRINTF_S("������5��0x15ƥ�䵽¥����� -> ����\n");
						}
						else
						{//δƥ�䵽¥�����
							USART_PRINTF_S("����5��0x15δƥ�䵽¥�����,����ƥ������0x16¥�����\n");
							readBlock(Read_Block, (uint8_t)0x16,sizeof(Read_Block));
						}
					}
				}
			}
		break;
		case Block_0x16://��ȡ¥�����
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x16 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("������5��0x16ʧ��  X\n");
			} 
			else
			{
				USART_PRINTF_S("������5��0x16�ɹ�  ��\n");
				if(1U == GetZR60Ctrl_u_CardSetSt())
				{//��Ƭ����ģʽ
					//("¥�����2:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						sminfo.build_numTwo[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		
					#ifdef ZR50
					setcard = 1U;
					#else
					readBlock(Read_Block, (uint8_t)0x18,sizeof(Read_Block));//��ȡ¥��id
					#endif
				}
				else
				{
					for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
					{//ÿ��¥�����2�ֽڣ�һ�������װ8��¥�����
						if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
						{
							if(*((uint16*)(&sminfo1.build_numTwo[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								Le_u_matchFlg = 1U;
								break;
							}
						}
					}

					if(1U == Le_u_matchFlg)
					{//ƥ�䵽¥�����
						open_door = 1;
						USART_PRINTF_S("������5��0x16ƥ�䵽¥����� -> ����\n");
					}
					else
					{//δƥ�䵽¥�����
						isSectedCard_flag = 1U;
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
						USART_PRINTF_S("������5��0x16δƥ�䵽¥�����\n");
					}
				}
			}
		}
		break;
		case Block_0x18://��ȡ¥��id����Ƭ����ģʽ�£�
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//������������֤ʧ��
				{
					Sereadcard_SetKeyflag = 0U;
					USART_PRINTF_S("����6 key������֤ʧ��  X\n");
				}
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("������6��0x18ʧ��  X\n");
			} 
			else
			{
				//("¥��id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					sminfo.build_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				readBlock(Read_Block, (uint8_t)0x19,sizeof(Read_Block));//��ȡ¥����id
			}
		}
		break;
		case Block_0x19://��ȡ��id����Ƭ����ģʽ�£�
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x19 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("������6��0x19ʧ��  X\n");
			} 
			else
			{
				//("��id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					sminfo.door_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				readBlock(Read_Block, (uint8_t)0x1A,sizeof(Read_Block));//��ȡ�±�
			}
		}
		break;
		case Block_0x1A://��ȡ�±꣨��Ƭ����ģʽ�£�
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x20 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("������6��0x1Aʧ��  X\n");
			} 
			else
			{
				//("�±�:");
				sminfo.suffix = rebackInfo.Dt.DtSrt.ValidDt[0U];	
				setcard = 1U;
			}
		}
		break;
		default:
		break;
	}
}		

                                               

