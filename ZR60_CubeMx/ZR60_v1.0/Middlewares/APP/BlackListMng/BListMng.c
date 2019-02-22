/******************************************************
�ļ�����	BListMng.c

������		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "BListMng.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
#ifdef TIME_DEBUG
static uint32  SeBListMng_dw_time[8U] = {0U};
#endif
static uint8   Se_u_WrFlag = 0U;
static uint8   Se_u_UpdataFlag[BLISTMNG_BLIST_NUM];
static uint8 	Se_u_Index = 0U;//�±�����
/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/
static uint8 BListMng_CntrstBList(uint32 Le_dw_data);
static void BListMng_BubbleSort(BListMng_cardNumStruct* Le_p_data,uint16 Le_w_lng);
static uint8 BListMng_BinSearch(uint32 num, BListMng_Struct* Le_p_data, uint16 length,uint8* Le_u_pos);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������BListMng_Init

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void BListMng_Init(void)
{
	uint8 Le_u_i;
#ifdef BLISTMNG_SAVEPOWEROFF
	uint16 	Le_w_Lng = 0U;
	printf("\n��5��\n");
#endif	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
#ifdef BLISTMNG_SAVEPOWEROFF
		if(1U == GetBListMngCfg_u_DtVild(CeBListMng_u_BlockIndex[Le_u_i]))
		{//����ʧ�Դ洢�����ݶ���ram����
			printf("\n����ʧ�Դ洢�����ݿ��� %d ��������Ч\n",Le_u_i);
			Le_w_Lng = GetBListMngCfg_w_DtLng(CeBListMng_u_BlockIndex[Le_u_i]);//��ǰ���ݿ���������Ч���ݳ�
			//ʣ��洢�ռ�ɴ洢��������
			printf("\nʣ��洢�ռ�ɴ洢�������� %d\n",(((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG));
			BListMngCfg_RdEE(CeBListMng_u_BlockIndex[Le_u_i],&CardNumCache[Le_u_i].CardNumArray[0U].CardNum[0U], Le_w_Lng);
			CardNumCache[Le_u_i].lng = Le_w_Lng;
			CardNumCache[Le_u_i].validity = 1U;
			printf("\n����ʧ�Դ洢�����ݿ��� %d �����ݶ���ram����\n",Le_u_i);
		}
		else
		{
			printf("\n����ʧ�Դ洢�����ݿ��� %d ��������Ч  ��\n",Le_u_i);
		}
#else
	CardNumCache[Le_u_i].lng = 0U;
	CardNumCache[Le_u_i].validity = 0U;
	CardNumCacheBkUp[Le_u_i].lng = 0U;
	CardNumCacheBkUp[Le_u_i].validity = 0U;
	Se_u_UpdataFlag[Le_u_i] = 0U;
#endif
	}
}

/******************************************************
*��������TskBListMng_MainFunction

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��2000�ź�������������10�����ݿ�ֿ�洢������ʱ�ֿ����
	   Ŀǰ������ÿ���洢��������������Ҫ1������롣
	   ��һ�����ǲ���"��̬����"��ʽ�������ܡ�
******************************************************/
void TskBListMng_MainFunction(void)
{
	uint8   Le_u_CardNum[BLISTMNG_CARD_NO_LNG] = {0};
	uint8 	Le_u_i;
	uint16 	Le_w_j;
	uint16 	Le_w_n;
	uint16 	Le_w_num;
	uint16 	Le_w_Lng = 0U;
#ifdef BLISTMNG_SAVEPOWEROFF
	if(1U !=GetBListMngCfg_EEIdle())
	{
		return;
	}
#endif
	if(1U == GetBListCache_u_BListValidity())
	{//�����������ţ����������Ŵ浽���ط���ʧ�Դ洢��
#ifdef TIME_DEBUG
		SeBListMng_dw_time[0]  = GetSysTick_Timer();
#endif
		for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
		{
			if(1U == CardNumCache[Le_u_i].validity)
			{//��ǰ���ݿ���������Ч
				Le_w_Lng = CardNumCache[Le_u_i].lng;
				if((Le_w_Lng%BLISTMNG_CARD_NO_LNG == 0) && (Le_w_Lng < (BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)))
				{//��ǰ���ݿ��ж���洢�ռ䣬����д�뵱ǰ���ݿ�
					//USART_PRINTF_D("����ʧ�Դ洢�����ݿ��� %d �ж���洢�ռ䣬���������ſ�д��\n",Le_u_i);
					Le_w_n = (Le_w_Lng/BLISTMNG_CARD_NO_LNG);//��ǰ���ݿ�洢�Ŀ�������
					for(Le_w_j = 0U;Le_w_j < Le_w_n;Le_w_j++)
					{
						*((uint32*)(&CARD_Array[Le_w_j])) =  *((uint32*)(&CardNumCache[Le_u_i].CardNumArray[Le_w_j]));
					}
					Le_w_num = (((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG);//��ǰ���ݿ�ʣ��洢�ռ�ɴ洢��������
					//USART_PRINTF_D("ʣ��洢�ռ�ɴ洢�������� %d\n",Le_w_num);
				}
				else
				{
					//USART_PRINTF_D("����ʧ�Դ洢�����ݿ��� %d �޶���洢�ռ�\n",Le_u_i);
					continue;
				}
			}
			else
			{//��ǰ���ݿ���������Ч
				//USART_PRINTF_D("����ʧ�Դ洢�����ݿ��� %d Ϊ�գ����������ſ�д��\n",Le_u_i);
				Le_w_num = ((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)/BLISTMNG_CARD_NO_LNG);//ʣ��洢�ռ�ɴ洢��������
				//USART_PRINTF_D("ʣ��洢�ռ�ɴ洢�������� %d\n",Le_w_num);
				Le_w_Lng = 0U;
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[6]  = GetSysTick_Timer();
#endif
			for(Le_w_j = 0U;Le_w_j < Le_w_num;Le_w_j++)
			{
				if(1U == GetBListCache_u_BListValidity())//�������������п�������
				{
					*((uint32*)(&Le_u_CardNum[0U])) = GetBListCache_dw_BlistData();//��ȡ��������������������
					if(0U == BListMng_CntrstBList(*((uint32*)(&Le_u_CardNum[0U]))))//����ʧ�Դ洢��δƥ�䵽�ÿ���
					{
						//USART_PRINTF_CARD_NUM("���ţ�%x%x%x%x ���������\n",Le_u_CardNum[0],Le_u_CardNum[1],	\
											  Le_u_CardNum[2],Le_u_CardNum[3]);
						*((uint32*)(&(CARD_Array[(Le_w_Lng/BLISTMNG_CARD_NO_LNG)].CardNum[0U]))) = 	\
							*((uint32*)(&Le_u_CardNum[0U]));
						Le_w_Lng = Le_w_Lng + BLISTMNG_CARD_NO_LNG;
						Se_u_WrFlag = 1U;
					}
					else
					{
						//USART_PRINTF_CARD_NUM("����%x%x%x%x�Ѵ���\n",Le_u_CardNum[0],Le_u_CardNum[1],	\
											  Le_u_CardNum[2],Le_u_CardNum[3]);
					}
				}
				else
				{//�������������޿�������
					break;
				}
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[7]  = GetSysTick_Timer();
#endif		
			if(1U == Se_u_WrFlag)//�������б��и���
			{
				Se_u_WrFlag = 0U;
				Se_u_UpdataFlag[Le_u_i] = 1U;
				Le_w_num = (Le_w_Lng/BLISTMNG_CARD_NO_LNG);
#ifdef TIME_DEBUG
				SeBListMng_dw_time[2]  = GetSysTick_Timer();
#endif
				BListMng_BubbleSort(CARD_Array,Le_w_num);//����
#ifdef TIME_DEBUG
				SeBListMng_dw_time[3]  = GetSysTick_Timer();
#endif
				//USART_PRINTF_D("д�뵽����ʧ�Դ洢�����ݿ���Ϊ %d\n",Le_u_i);
				//USART_PRINTF_S("д�뵽����ʧ�Դ洢���Ŀ�������:");
#ifdef TIME_DEBUG
				SeBListMng_dw_time[4]  = GetSysTick_Timer();
#endif
				for(Le_w_j = 0U;Le_w_j < Le_w_num;Le_w_j++)
				{//���Ƶ�ram��
					*((uint32*)(&CardNumCache[Le_u_i].CardNumArray[Le_w_j])) =  \
						*((uint32*)(&CARD_Array[Le_w_j]));
					//USART_PRINTF_CARD_NUM("%x%x%x%x\n",CARD_Array[Le_w_j].CardNum[0U],CARD_Array[Le_w_j].CardNum[1U] \
									 ,CARD_Array[Le_w_j].CardNum[2U],CARD_Array[Le_w_j].CardNum[3U]);
				}
#ifdef TIME_DEBUG
				SeBListMng_dw_time[5]  = GetSysTick_Timer();
#endif
				CardNumCache[Le_u_i].lng = Le_w_Lng;
				CardNumCache[Le_u_i].validity = 1U;	
#ifdef BLISTMNG_SAVEPOWEROFF
				BListMngCfg_WrEE(CeBListMng_u_BlockIndex[Le_u_i],&CARD_Array[0U].CardNum[0U],Le_w_Lng);//��������������д�����ʧ�Դ洢��
#endif			
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[1]  = GetSysTick_Timer();
			printf("���űȶ�ǰ����ǰʱ�䣺%d\n",SeBListMng_dw_time[6]);
			printf("���űȶԺ󣬵�ǰʱ�䣺%d\n",SeBListMng_dw_time[7]);
			printf("����ǰ����ǰʱ�䣺%d\n",SeBListMng_dw_time[2]);
			printf("����󣬵�ǰʱ�䣺%d\n",SeBListMng_dw_time[3]);
			printf("����ramǰ����ǰʱ�䣺%d\n",SeBListMng_dw_time[4]);
			printf("����ram�󣬵�ǰʱ�䣺%d\n",SeBListMng_dw_time[5]);
			printf("������������Ŵ�����ǰʱ�䣺%d\n",SeBListMng_dw_time[0]);
			printf("�˳����������Ŵ�����ǰʱ�䣺%d\n",SeBListMng_dw_time[1]);
			printf("����ʧ�Դ洢�����ݿ��� %d\n",Le_u_i);
			printf("ʣ��洢�ռ�ɴ洢�������� %d\n",(((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG));
#endif
			break;
		}
		
		if(Le_u_i == BLISTMNG_BLIST_NUM)
		{
#ifdef TIME_DEBUG
			//�������洢�ռ䲻��,����������δ¼�������
			printf("�������洢�ռ䲻��");
#endif
			ClrBListCache_Queue();//�建���������
		}
	}
	
	
	if(1U == tcp_client_BListUpdataSt())
	{//����������������
		CardNumCacheBkUp[Se_u_Index].validity = CardNumCache[Se_u_Index].validity;
		if(1U == Se_u_UpdataFlag[Se_u_Index])
		{//�и���
			Se_u_UpdataFlag[Se_u_Index] = 0U;
			CardNumCacheBkUp[Se_u_Index].lng = CardNumCache[Se_u_Index].lng;
			//USART_PRINTF_S("���Ƶ�����ram����������:");
			for(Le_w_j = 0U;Le_w_j < (CardNumCache[Se_u_Index].lng/BLISTMNG_CARD_NO_LNG);Le_w_j++)
			{//���Ƶ�����ram�������ں������ȶ�
				*((uint32*)(&CardNumCacheBkUp[Se_u_Index].CardNumArray[Le_w_j])) = \
						*((uint32*)(&CardNumCache[Se_u_Index].CardNumArray[Le_w_j]));
				//USART_PRINTF_CARD_NUM("%x%x%x%x\n",CardNumCacheBkUp[Se_u_Index].CardNumArray[Le_w_j].CardNum[0U],CardNumCacheBkUp[Se_u_Index].CardNumArray[Le_w_j].CardNum[1U] \
									  ,CardNumCacheBkUp[Se_u_Index].CardNumArray[Le_w_j].CardNum[2U],CardNumCacheBkUp[Se_u_Index].CardNumArray[Le_w_j].CardNum[3U]);
			}
		}
		Se_u_Index++;
		if(BLISTMNG_BLIST_NUM == Se_u_Index)
		{
			Se_u_Index = 0U;
		}
	}
}


/******************************************************
*��������BListMng_u_ContrastBList

*��  �Σ�void

*����ֵ��void

*��  ����ƥ�������

*��  ע��
******************************************************/
uint8 BListMng_u_ContrastBList(uint32 Le_dw_data)
{
	uint8 	Le_u_i;
	uint16 	Le_w_Lng = 0U;
	uint8 	Le_u_pos;
	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		if(1U == CardNumCacheBkUp[Le_u_i].validity)
		{//��ǰ���ݿ���������Ч
			Le_w_Lng = (CardNumCacheBkUp[Le_u_i].lng/BLISTMNG_CARD_NO_LNG);//���Ÿ���
			if(1U == BListMng_BinSearch(Le_dw_data,&CardNumCacheBkUp[Le_u_i],Le_w_Lng,&Le_u_pos))
			{//��ǰ�����Ѵ���
				return 1U;
			}
		}
	}
	
	return 0U;
}


/******************************************************
*��������BListMng_CntrstBList

*��  �Σ�void

*����ֵ��void

*��  ����ƥ�������(�ڲ�����ȥ��ʹ��)

*��  ע��
******************************************************/
static uint8 BListMng_CntrstBList(uint32 Le_dw_data)
{
	uint8 	Le_u_i;
	uint16 	Le_w_Lng = 0U;
	uint8 	Le_u_pos;
	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		if(1U == CardNumCache[Le_u_i].validity)
		{//��ǰ���ݿ���������Ч
			Le_w_Lng = (CardNumCache[Le_u_i].lng/BLISTMNG_CARD_NO_LNG);//���Ÿ���
			if(1U == BListMng_BinSearch(Le_dw_data,&CardNumCache[Le_u_i],Le_w_Lng,&Le_u_pos))
			{//��ǰ�����Ѵ���
				return 1U;
			}
		}
	}
	
	return 0U;
}




/******************************************************
*��������BListMng_BubbleSort

*��  �Σ�

*����ֵ��void

*��  ����ð������

*��  ע��
******************************************************/
static void BListMng_BubbleSort(BListMng_cardNumStruct* Le_p_data,uint16 Le_w_lng)
{
	uint16 Le_w_i;
	uint16 Le_w_j;
	uint32 tmp = 0U;
	for(Le_w_i=0;Le_w_i<(Le_w_lng-1);Le_w_i++)
	{
		for(Le_w_j=0;Le_w_j<(Le_w_lng-Le_w_i-1);Le_w_j++)
		{
			if(*((uint32*)(&Le_p_data[Le_w_j])) > *((uint32*)(&Le_p_data[Le_w_j+1])))
			{
				tmp = *((uint32*)(&Le_p_data[Le_w_j]));
				*((uint32*)(&Le_p_data[Le_w_j])) = *((uint32*)(&Le_p_data[Le_w_j+1]));
				*((uint32*)(&Le_p_data[Le_w_j+1])) = tmp;
			}
		}
	}
}


/******************************************************
*��������BListMng_BubbleSort

*��  �Σ�

*����ֵ��void

*��  �������ֲ���

*��  ע��
******************************************************/
static uint8 BListMng_BinSearch(uint32 num, BListMng_Struct* Le_p_data, uint16 length,uint8* Le_u_pos)/*numΪ���������֣�lengthΪ����a�ĳ���*/
{
    int low, mid, high;//����Ϊ�з�����
    low = 0;
    high = length - 1;

    while( low <= high )
    {
        mid = ( low + high ) / 2;
        if( *((uint32*)(&(Le_p_data->CardNumArray[mid]))) > num )
		{
            high = mid - 1;
		}
        else if(*((uint32*)(&(Le_p_data->CardNumArray[mid]))) < num )
		{
            low = mid + 1;
		}
        else
		{
			*Le_u_pos = mid;//����λ��
			return 1U;/*���ҵ�num����1*/
		}
    }

    return 0U;        /*δ���ҵ�num����0*/
}



/******************************************************
*��������ClrBListMng_ListData

*��  �Σ�void

*����ֵ��void

*��  ������������б�����

*��  ע��
******************************************************/
void ClrBListMng_ListData(void)
{
	uint8 	Le_u_i;
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		CardNumCache[Le_u_i].validity = 0U;
	}
}
