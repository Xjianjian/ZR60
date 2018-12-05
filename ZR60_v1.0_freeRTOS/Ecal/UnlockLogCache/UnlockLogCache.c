/******************************************************
�ļ�����	UnlockLogCache

������		
Data			Vasion			author
20187/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "UnlockLogCache.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static UnlockLogCacheStruct  SaUnlockLogCache_h_QueueData[UnlockLogCache_QUEUE_LNG];/*���ݷ��Ͷ���*/
static uint8 SeUnlockLogCache_u_HeadLabel;/*ͷ��ǩ*/
static uint8 SeUnlockLogCache_u_TialLabel;/*β��ǩ*/

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
*��������InitUnlockLogCache_Parameter

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void InitUnlockLogCache_Parameter(void)
{
	ClrUnlockLogCache_Queue();//�建�����
}


/******************************************************
*��������WrUnlockLogCache_Queue

*��  �Σ�

*����ֵ��

*��  ����д���ݵ����ݶ���

*��  ע��
******************************************************/
void WrUnlockLogCache_Queue(uint64_t* Time,char* Id,uint8 Event)
{
	uint8 LeUnlockLogCache_u_Lng;

	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_HeadLabel].e_u_Time = *Time;
	for(LeUnlockLogCache_u_Lng = 0U;LeUnlockLogCache_u_Lng< UnlockLogCache_ID_LNG;LeUnlockLogCache_u_Lng++)
	{
		SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_HeadLabel].e_u_ID[LeUnlockLogCache_u_Lng] = Id[LeUnlockLogCache_u_Lng];
	}
	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_HeadLabel].e_u_Event = Event;	
	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_HeadLabel].e_u_NonEmptyFlg = 1U;/*�÷ǿձ�־*/
	
	SeUnlockLogCache_u_HeadLabel++;
	if(UnlockLogCache_QUEUE_LNG == SeUnlockLogCache_u_HeadLabel)
	{
		SeUnlockLogCache_u_HeadLabel = 0U;
	}
}


/******************************************************
*��������GetUnlockLogCache_doorLog

*��  �Σ�

*����ֵ��

*��  ������ȡ���ż�¼log

*��  ע��
******************************************************/
void GetUnlockLogCache_doorLog(UnlockLogCacheStruct* UnlockLog)
{
	uint8 LeUnlockLogCache_u_Lng;
	UnlockLog->e_u_Time = SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_Time;
	for(LeUnlockLogCache_u_Lng = 0U;LeUnlockLogCache_u_Lng< UnlockLogCache_ID_LNG;LeUnlockLogCache_u_Lng++)
	{
		UnlockLog->e_u_ID[LeUnlockLogCache_u_Lng] = SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_ID[LeUnlockLogCache_u_Lng];
	}
	UnlockLog->e_u_Event = SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_Event;
	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_NonEmptyFlg = 0U;	/*��ǿձ�־*/
		
	SeUnlockLogCache_u_TialLabel++;
	if(UnlockLogCache_QUEUE_LNG == SeUnlockLogCache_u_TialLabel)
	{
		SeUnlockLogCache_u_TialLabel = 0U;
	}
}



/******************************************************
*��������GetUnlockLogCache_u_LogAvild

*��  �Σ�

*����ֵ��

*��  ������ȡ������¼��Ч��

*��  ע��
******************************************************/
uint8 GetUnlockLogCache_u_LogAvild(void) 
{
	return (SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_NonEmptyFlg);
}




/******************************************************
*��������ClrUnlockLogCache_Queue

*��  �Σ�

*����ֵ��

*��  ���������ݶ���

*��  ע��
******************************************************/
void ClrUnlockLogCache_Queue(void) 
{
	uint8 LeUnlockLogCache_u_Index_i;
	uint8 LeUnlockLogCache_u_Lng;
	for(LeUnlockLogCache_u_Index_i = 0U;LeUnlockLogCache_u_Index_i < UnlockLogCache_QUEUE_LNG;LeUnlockLogCache_u_Index_i++)
	{
		SaUnlockLogCache_h_QueueData[LeUnlockLogCache_u_Index_i].e_u_NonEmptyFlg = 0U;
		SaUnlockLogCache_h_QueueData[LeUnlockLogCache_u_Index_i].e_u_Time = 0U;
		for(LeUnlockLogCache_u_Lng = 0U;LeUnlockLogCache_u_Lng< UnlockLogCache_ID_LNG;LeUnlockLogCache_u_Lng++)
		{
			SaUnlockLogCache_h_QueueData[LeUnlockLogCache_u_Index_i].e_u_ID[LeUnlockLogCache_u_Lng] = 0U;
		}
		SaUnlockLogCache_h_QueueData[LeUnlockLogCache_u_Index_i].e_u_Event = 0U;
	}
	SeUnlockLogCache_u_HeadLabel = 0U;
	SeUnlockLogCache_u_TialLabel = 0U;
}

