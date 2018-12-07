/******************************************************
文件名：	BListMng.c

描述：		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "BListMng.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
#ifdef TIME_DEBUG
static uint32  SeBListMng_dw_time[8U] = {0U};
#endif
static uint8   Se_u_WrFlag = 0U;
static uint8   Se_u_UpdataFlag[BLISTMNG_BLIST_NUM];
static uint8 	Se_u_Index = 0U;//下标索引
/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/
static uint8 BListMng_CntrstBList(uint32 Le_dw_data);
static void BListMng_BubbleSort(BListMng_cardNumStruct* Le_p_data,uint16 Le_w_lng);
static uint8 BListMng_BinSearch(uint32 num, BListMng_Struct* Le_p_data, uint16 length,uint8* Le_u_pos);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：BListMng_Init

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void BListMng_Init(void)
{
	uint8 Le_u_i;
#ifdef BLISTMNG_SAVEPOWEROFF
	uint16 	Le_w_Lng = 0U;
	printf("\n【5】\n");
#endif	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
#ifdef BLISTMNG_SAVEPOWEROFF
		if(1U == GetBListMngCfg_u_DtVild(CeBListMng_u_BlockIndex[Le_u_i]))
		{//非易失性存储区数据读入ram缓存
			printf("\n非易失性存储器数据块区 %d 的数据有效\n",Le_u_i);
			Le_w_Lng = GetBListMngCfg_w_DtLng(CeBListMng_u_BlockIndex[Le_u_i]);//当前数据块区数据有效数据长
			//剩余存储空间可存储卡的数量
			printf("\n剩余存储空间可存储卡的数量 %d\n",(((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG));
			BListMngCfg_RdEE(CeBListMng_u_BlockIndex[Le_u_i],&CardNumCache[Le_u_i].CardNumArray[0U].CardNum[0U], Le_w_Lng);
			CardNumCache[Le_u_i].lng = Le_w_Lng;
			CardNumCache[Le_u_i].validity = 1U;
			printf("\n非易失性存储器数据块区 %d 的数据读入ram缓存\n",Le_u_i);
		}
		else
		{
			printf("\n非易失性存储器数据块区 %d 的数据无效  √\n",Le_u_i);
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
*函数名：TskBListMng_MainFunction

*形  参：void

*返回值：void

*描  述：

*备  注：2000张黑名单卡划分了10个数据块分块存储，查找时分块查找
	   目前最坏情况下每个存储块数据排序大概需要1个多毫秒。
	   下一步考虑采用"动态链表"方式提升性能。
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
	{//检查黑名单卡号，黑名单卡号存到本地非易失性存储器
#ifdef TIME_DEBUG
		SeBListMng_dw_time[0]  = GetSysTick_Timer();
#endif
		for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
		{
			if(1U == CardNumCache[Le_u_i].validity)
			{//当前数据块区数据有效
				Le_w_Lng = CardNumCache[Le_u_i].lng;
				if((Le_w_Lng%BLISTMNG_CARD_NO_LNG == 0) && (Le_w_Lng < (BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)))
				{//当前数据块有多余存储空间，数据写入当前数据块
					//USART_PRINTF_D("非易失性存储器数据块区 %d 有多余存储空间，黑名单卡号可写入\n",Le_u_i);
					Le_w_n = (Le_w_Lng/BLISTMNG_CARD_NO_LNG);//当前数据块存储的卡号数量
					for(Le_w_j = 0U;Le_w_j < Le_w_n;Le_w_j++)
					{
						*((uint32*)(&CARD_Array[Le_w_j])) =  *((uint32*)(&CardNumCache[Le_u_i].CardNumArray[Le_w_j]));
					}
					Le_w_num = (((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG);//当前数据块剩余存储空间可存储卡的数量
					//USART_PRINTF_D("剩余存储空间可存储卡的数量 %d\n",Le_w_num);
				}
				else
				{
					//USART_PRINTF_D("非易失性存储器数据块区 %d 无多余存储空间\n",Le_u_i);
					continue;
				}
			}
			else
			{//当前数据块区数据无效
				//USART_PRINTF_D("非易失性存储器数据块区 %d 为空，黑名单卡号可写入\n",Le_u_i);
				Le_w_num = ((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)/BLISTMNG_CARD_NO_LNG);//剩余存储空间可存储卡的数量
				//USART_PRINTF_D("剩余存储空间可存储卡的数量 %d\n",Le_w_num);
				Le_w_Lng = 0U;
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[6]  = GetSysTick_Timer();
#endif
			for(Le_w_j = 0U;Le_w_j < Le_w_num;Le_w_j++)
			{
				if(1U == GetBListCache_u_BListValidity())//黑名单缓存区有卡号数据
				{
					*((uint32*)(&Le_u_CardNum[0U])) = GetBListCache_dw_BlistData();//读取黑名单缓存区卡号数据
					if(0U == BListMng_CntrstBList(*((uint32*)(&Le_u_CardNum[0U]))))//非易失性存储区未匹配到该卡号
					{
						//USART_PRINTF_CARD_NUM("卡号：%x%x%x%x 加入黑名单\n",Le_u_CardNum[0],Le_u_CardNum[1],	\
											  Le_u_CardNum[2],Le_u_CardNum[3]);
						*((uint32*)(&(CARD_Array[(Le_w_Lng/BLISTMNG_CARD_NO_LNG)].CardNum[0U]))) = 	\
							*((uint32*)(&Le_u_CardNum[0U]));
						Le_w_Lng = Le_w_Lng + BLISTMNG_CARD_NO_LNG;
						Se_u_WrFlag = 1U;
					}
					else
					{
						//USART_PRINTF_CARD_NUM("卡号%x%x%x%x已存在\n",Le_u_CardNum[0],Le_u_CardNum[1],	\
											  Le_u_CardNum[2],Le_u_CardNum[3]);
					}
				}
				else
				{//黑名单缓存区无卡号数据
					break;
				}
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[7]  = GetSysTick_Timer();
#endif		
			if(1U == Se_u_WrFlag)//黑名单列表有更新
			{
				Se_u_WrFlag = 0U;
				Se_u_UpdataFlag[Le_u_i] = 1U;
				Le_w_num = (Le_w_Lng/BLISTMNG_CARD_NO_LNG);
#ifdef TIME_DEBUG
				SeBListMng_dw_time[2]  = GetSysTick_Timer();
#endif
				BListMng_BubbleSort(CARD_Array,Le_w_num);//排序
#ifdef TIME_DEBUG
				SeBListMng_dw_time[3]  = GetSysTick_Timer();
#endif
				//USART_PRINTF_D("写入到非易失性存储器数据块区为 %d\n",Le_u_i);
				//USART_PRINTF_S("写入到非易失性存储器的卡号如下:");
#ifdef TIME_DEBUG
				SeBListMng_dw_time[4]  = GetSysTick_Timer();
#endif
				for(Le_w_j = 0U;Le_w_j < Le_w_num;Le_w_j++)
				{//复制到ram区
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
				BListMngCfg_WrEE(CeBListMng_u_BlockIndex[Le_u_i],&CARD_Array[0U].CardNum[0U],Le_w_Lng);//黑名单卡号数据写入非易失性存储器
#endif			
			}
#ifdef TIME_DEBUG
			SeBListMng_dw_time[1]  = GetSysTick_Timer();
			printf("卡号比对前，当前时间：%d\n",SeBListMng_dw_time[6]);
			printf("卡号比对后，当前时间：%d\n",SeBListMng_dw_time[7]);
			printf("排序前，当前时间：%d\n",SeBListMng_dw_time[2]);
			printf("排序后，当前时间：%d\n",SeBListMng_dw_time[3]);
			printf("复制ram前，当前时间：%d\n",SeBListMng_dw_time[4]);
			printf("复制ram后，当前时间：%d\n",SeBListMng_dw_time[5]);
			printf("进入黑名单卡号处理，当前时间：%d\n",SeBListMng_dw_time[0]);
			printf("退出黑名单卡号处理，当前时间：%d\n",SeBListMng_dw_time[1]);
			printf("非易失性存储器数据块区 %d\n",Le_u_i);
			printf("剩余存储空间可存储卡的数量 %d\n",(((BLISTMNG_CARD_NO_LNG * BLISTMNG_CARD_NUM)-Le_w_Lng)/BLISTMNG_CARD_NO_LNG));
#endif
			break;
		}
		
		if(Le_u_i == BLISTMNG_BLIST_NUM)
		{
#ifdef TIME_DEBUG
			//黑名单存储空间不足,黑名单卡号未录入黑名单
			printf("黑名单存储空间不足");
#endif
			ClrBListCache_Queue();//清缓存队列数据
		}
	}
	
	
	if(1U == tcp_client_BListUpdataSt())
	{//黑名单增量更新中
		CardNumCacheBkUp[Se_u_Index].validity = CardNumCache[Se_u_Index].validity;
		if(1U == Se_u_UpdataFlag[Se_u_Index])
		{//有更新
			Se_u_UpdataFlag[Se_u_Index] = 0U;
			CardNumCacheBkUp[Se_u_Index].lng = CardNumCache[Se_u_Index].lng;
			//USART_PRINTF_S("复制到备份ram区卡号如下:");
			for(Le_w_j = 0U;Le_w_j < (CardNumCache[Se_u_Index].lng/BLISTMNG_CARD_NO_LNG);Le_w_j++)
			{//复制到备份ram区，用于黑名单比对
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
*函数名：BListMng_u_ContrastBList

*形  参：void

*返回值：void

*描  述：匹配黑名单

*备  注：
******************************************************/
uint8 BListMng_u_ContrastBList(uint32 Le_dw_data)
{
	uint8 	Le_u_i;
	uint16 	Le_w_Lng = 0U;
	uint8 	Le_u_pos;
	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		if(1U == CardNumCacheBkUp[Le_u_i].validity)
		{//当前数据块区数据有效
			Le_w_Lng = (CardNumCacheBkUp[Le_u_i].lng/BLISTMNG_CARD_NO_LNG);//卡号个数
			if(1U == BListMng_BinSearch(Le_dw_data,&CardNumCacheBkUp[Le_u_i],Le_w_Lng,&Le_u_pos))
			{//当前卡号已存在
				return 1U;
			}
		}
	}
	
	return 0U;
}


/******************************************************
*函数名：BListMng_CntrstBList

*形  参：void

*返回值：void

*描  述：匹配黑名单(内部卡号去重使用)

*备  注：
******************************************************/
static uint8 BListMng_CntrstBList(uint32 Le_dw_data)
{
	uint8 	Le_u_i;
	uint16 	Le_w_Lng = 0U;
	uint8 	Le_u_pos;
	
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		if(1U == CardNumCache[Le_u_i].validity)
		{//当前数据块区数据有效
			Le_w_Lng = (CardNumCache[Le_u_i].lng/BLISTMNG_CARD_NO_LNG);//卡号个数
			if(1U == BListMng_BinSearch(Le_dw_data,&CardNumCache[Le_u_i],Le_w_Lng,&Le_u_pos))
			{//当前卡号已存在
				return 1U;
			}
		}
	}
	
	return 0U;
}




/******************************************************
*函数名：BListMng_BubbleSort

*形  参：

*返回值：void

*描  述：冒泡排序

*备  注：
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
*函数名：BListMng_BubbleSort

*形  参：

*返回值：void

*描  述：二分查找

*备  注：
******************************************************/
static uint8 BListMng_BinSearch(uint32 num, BListMng_Struct* Le_p_data, uint16 length,uint8* Le_u_pos)/*num为待查找数字，length为数组a的长度*/
{
    int low, mid, high;//必须为有符号型
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
			*Le_u_pos = mid;//返回位置
			return 1U;/*查找到num返回1*/
		}
    }

    return 0U;        /*未查找到num返回0*/
}



/******************************************************
*函数名：ClrBListMng_ListData

*形  参：void

*返回值：void

*描  述：清黑名单列表数据

*备  注：
******************************************************/
void ClrBListMng_ListData(void)
{
	uint8 	Le_u_i;
	for(Le_u_i = 0U;Le_u_i < BLISTMNG_BLIST_NUM;Le_u_i++)
	{
		CardNumCache[Le_u_i].validity = 0U;
	}
}
