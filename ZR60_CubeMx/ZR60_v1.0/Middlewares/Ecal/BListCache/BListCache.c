/******************************************************
文件名：	BListCache

描述：		
Data			Vasion			author
20187/4/19		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "BListCache.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static BListCacheStruct  SaBListCache_h_QueueData[BLISTCACHE_QUEUE_LNG];/*数据发送队列*/
static uint8 SeBListCache_u_HeadLabel;/*头标签*/
static uint8 SeBListCache_u_TialLabel;/*尾标签*/

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
*函数名：InitBListCache_Parameter

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void InitBListCache_Parameter(void)
{
	ClrBListCache_Queue();//清缓存队列
}


/******************************************************
*函数名：WrBListCache_BListQueue

*形  参：

*返回值：

*描  述：写数据到数据队列

*备  注：
******************************************************/
void WrBListCache_BListQueue(const uint8* LeBListCache_u_data)
{
	if(0U == SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_NonEmptyFlg)/*队列当前成员数据空*/
	{
		*((uint32*)(&(SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_Data[0U]))) = *((uint32*)LeBListCache_u_data); 
		SaBListCache_h_QueueData[SeBListCache_u_HeadLabel].e_u_NonEmptyFlg = 1U;/*置非空标志*/
		
		SeBListCache_u_HeadLabel++;
		if(BLISTCACHE_QUEUE_LNG == SeBListCache_u_HeadLabel)
		{
			SeBListCache_u_HeadLabel = 0U;
		}
	}
	else
	{
		//队列满
	}
}


/******************************************************
*函数名：GetBListCache_dw_BlistData

*形  参：

*返回值：

*描  述：获取黑名单数据

*备  注：
******************************************************/
uint32 GetBListCache_dw_BlistData(void)
{
	uint32 LeBListCache_dw_data;

	LeBListCache_dw_data = *((uint32*)(&(SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_Data[0U])));
	SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_NonEmptyFlg = 0U;	/*清非空标志*/
		
	SeBListCache_u_TialLabel++;
	if(BLISTCACHE_QUEUE_LNG == SeBListCache_u_TialLabel)
	{
		SeBListCache_u_TialLabel = 0U;
	}	
  return LeBListCache_dw_data;
}



/******************************************************
*函数名：GetBListCache_u_BListValidity

*形  参：

*返回值：

*描  述：获取黑名单数据有效性

*备  注：
******************************************************/
uint8 GetBListCache_u_BListValidity(void) 
{
	return (SaBListCache_h_QueueData[SeBListCache_u_TialLabel].e_u_NonEmptyFlg);
}




/******************************************************
*函数名：ClrBListCache_Queue

*形  参：

*返回值：

*描  述：清数据队列

*备  注：
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

