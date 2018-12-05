/******************************************************
文件名：	UnlockLogCache

描述：		
Data			Vasion			author
20187/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "UnlockLogCache.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static UnlockLogCacheStruct  SaUnlockLogCache_h_QueueData[UnlockLogCache_QUEUE_LNG];/*数据发送队列*/
static uint8 SeUnlockLogCache_u_HeadLabel;/*头标签*/
static uint8 SeUnlockLogCache_u_TialLabel;/*尾标签*/

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/

/******************************************************
description： function code
******************************************************/

/******************************************************
*函数名：InitUnlockLogCache_Parameter

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void InitUnlockLogCache_Parameter(void)
{
	ClrUnlockLogCache_Queue();//清缓存队列
}


/******************************************************
*函数名：WrUnlockLogCache_Queue

*形  参：

*返回值：

*描  述：写数据到数据队列

*备  注：
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
	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_HeadLabel].e_u_NonEmptyFlg = 1U;/*置非空标志*/
	
	SeUnlockLogCache_u_HeadLabel++;
	if(UnlockLogCache_QUEUE_LNG == SeUnlockLogCache_u_HeadLabel)
	{
		SeUnlockLogCache_u_HeadLabel = 0U;
	}
}


/******************************************************
*函数名：GetUnlockLogCache_doorLog

*形  参：

*返回值：

*描  述：获取开门记录log

*备  注：
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
	SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_NonEmptyFlg = 0U;	/*清非空标志*/
		
	SeUnlockLogCache_u_TialLabel++;
	if(UnlockLogCache_QUEUE_LNG == SeUnlockLogCache_u_TialLabel)
	{
		SeUnlockLogCache_u_TialLabel = 0U;
	}
}



/******************************************************
*函数名：GetUnlockLogCache_u_LogAvild

*形  参：

*返回值：

*描  述：获取开锁记录有效性

*备  注：
******************************************************/
uint8 GetUnlockLogCache_u_LogAvild(void) 
{
	return (SaUnlockLogCache_h_QueueData[SeUnlockLogCache_u_TialLabel].e_u_NonEmptyFlg);
}




/******************************************************
*函数名：ClrUnlockLogCache_Queue

*形  参：

*返回值：

*描  述：清数据队列

*备  注：
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

