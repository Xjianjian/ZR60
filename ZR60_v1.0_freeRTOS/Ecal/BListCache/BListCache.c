/******************************************************
�ļ�����	BListCache

������		
Data			Vasion			author
20187/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "BListCache.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static BListCacheStruct  SaBListCache_h_QueueData[BLISTCACHE_QUEUE_LNG];/*���ݷ��Ͷ���*/
static uint8 SeBListCache_u_HeadLabel;/*ͷ��ǩ*/
static uint8 SeBListCache_u_TialLabel;/*β��ǩ*/

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/

/******************************************************
description�� function code
******************************************************/

/******************************************************
*��������InitBListCache_Parameter

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void InitBListCache_Parameter(void)
{
	ClrBListCache_Queue();//�建�����
}


/******************************************************
*��������WrBListCache_BListQueue

*��  �Σ�

*����ֵ��

*��  ����д���ݵ����ݶ���

*��  ע��
******************************************************/
void WrBListCache_BListQueue(const uint8* LeBListCache_u_data)
{
	if(0U == SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_NonEmptyFlg)/*���е�ǰ��Ա���ݿ�*/
	{
		*((uint32*)(&(SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_Data[0U]))) = *((uint32*)LeBListCache_u_data); 
		SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_NonEmptyFlg = 1U;/*�÷ǿձ�־*/
		
		SeBListCache_u_HeadLabel++;
		if(BLISTCACHE_QUEUE_LNG == SeBListCache_u_HeadLabel)
		{
			SeBListCache_u_HeadLabel = 0U;
		}
	}
	else
	{
		//������
	}
}


/******************************************************
*��������GetBListCache_dw_BlistData

*��  �Σ�

*����ֵ��

*��  ������ȡ����������

*��  ע��
******************************************************/
uint32 GetBListCache_dw_BlistData(void)
{
	uint32 LeBListCache_dw_data;

	LeBListCache_dw_data = *((uint32*)(&(SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_Data[0U])));
	SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_NonEmptyFlg = 0U;	/*��ǿձ�־*/
		
	SeBListCache_u_TialLabel++;
	if(BLISTCACHE_QUEUE_LNG == SeBListCache_u_TialLabel)
	{
		SeBListCache_u_TialLabel = 0U;
	}	
  return LeBListCache_dw_data;
}



/******************************************************
*��������GetBListCache_u_BListValidity

*��  �Σ�

*����ֵ��

*��  ������ȡ������������Ч��

*��  ע��
******************************************************/
uint8 GetBListCache_u_BListValidity(void) 
{
	return (SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_NonEmptyFlg);
}




/******************************************************
*��������ClrBListCache_Queue

*��  �Σ�

*����ֵ��

*��  ���������ݶ���

*��  ע��
******************************************************/
void ClrBListCache_Queue(void) 
{
	uint8 LeBListCache_u_Index_i;
	for(LeBListCache_u_Index_i = 0U;LeBListCache_u_Index_i < BLISTCACHE_QUEUE_LNG;LeBListCache_u_Index_i++)
	{
		SaBListCache_h_QueueData[LeBListCache_u_Index_i].e_u_NonEmptyFlg = 0U;
		//*((uint32*)(&(SaBListCache_h_QueueData[LeBListCache_u_Index_i].e_u_Data[0U]))) = 0x00U;
	}
	SeBListCache_u_HeadLabel = 0U;
	SeBListCache_u_TialLabel = 0U;
}

