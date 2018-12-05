/******************************************************
文件名：	STD_ADFliter

描述：		

Data			Vasion			author
2018/6/27		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"STD_ADFliter.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void ADFliter_PwrOnFastFliter(void);
static void ADFliter_FliterArrayFill(void);
static void ADFliter_ADFliterDeal(void);

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static uint8  SeADFliter_u_ADFliterSt;//AD滤波状态机状态
static TsADFliter_h_FliterParm  SaADFliter_h_FliterParm[ADFLITERCFG_CHANNELNUM];
static uint8 SeADFliter_u_GetADTimer;//获取AD值的延时计时器

static void ALibrary_AscendingBubbleSort(int *ptr, uint8 size);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：void InitADFliter_parameter(void)

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitADFliter_parameter(void)
{
	uint8 LeADFliter_u_Index;
	uint8 LeADFliter_u_ArrayIndex;
	SeADFliter_u_ADFliterSt = ADFLITERTYPES_PWRONFASTFLITER;//上电快速滤波状态
	for(LeADFliter_u_Index=0;LeADFliter_u_Index < ADFLITERCFG_CHANNELNUM;LeADFliter_u_Index++)
	{
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_FliterArrayFullFlag = STD_INACTIVE;
		for(LeADFliter_u_ArrayIndex=0;LeADFliter_u_ArrayIndex < ADFLITERCFG_ADARRAYLENGTH_MAX;LeADFliter_u_ArrayIndex++)
		{
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[LeADFliter_u_ArrayIndex] = STD_NULL;
		}
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt = STD_NULL;
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = CaADFliter_sw_DefaultADResult[LeADFliter_u_Index];
	}
	SeADFliter_u_GetADTimer = STD_NULL;
}


/******************************************************
*函数名：void MngADFliter_mainFunction(void)

*形  参：void

*返回值：void

*描  述：主任务函数

*备  注：
******************************************************/
void MngADFliter_mainFunction(void)
{
	switch(SeADFliter_u_ADFliterSt)
	{
		case  ADFLITERTYPES_PWRONFASTFLITER:
		{
			ADFliter_PwrOnFastFliter();//上电快速滤波
			SeADFliter_u_ADFliterSt = ADFLITERTYPES_NORMALFLITER;//状态机切换  
		}
		break;
		case  ADFLITERTYPES_NORMALFLITER:
		{
			if(CaADFliter_u_GetADTimeReach == SeADFliter_u_GetADTimer)
			{
				SeADFliter_u_GetADTimer = STD_NULL;
				ADFliter_FliterArrayFill();//填充滤波数组
				ADFliter_ADFliterDeal();//对填充好的滤波数组滤波处理  
			}
			else
			{
				SeADFliter_u_GetADTimer++;
			}
		}
		break;
		default:
		break;
	}
}



/******************************************************
*函数名：static void ADFliter_PwrOnFastFliter(void)

*形  参：void

*返回值：void

*描  述：上电快速滤波,将首次采样值作为AD滤波结果值

*备  注：
******************************************************/
static void ADFliter_PwrOnFastFliter(void)
{
	uint8 LeADFliter_u_Index;
	for(LeADFliter_u_Index =0;LeADFliter_u_Index < ADFLITERCFG_CHANNELNUM;LeADFliter_u_Index++)
	{
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = \
			GetADFliter_sw_ADSampleData(CaADFliter_u_ADChannelIndex[LeADFliter_u_Index]);
	}
}


/******************************************************
*函数名：static void ADFliter_FliterArrayFill(void)

*形  参：void

*返回值：void

*描  述：填充滤波数组

*备  注：
******************************************************/
static void ADFliter_FliterArrayFill(void)
{
	uint8 LeADFliter_u_Index;
	for(LeADFliter_u_Index =0;LeADFliter_u_Index < ADFLITERCFG_CHANNELNUM;LeADFliter_u_Index++)
	{
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt] = \
			GetADFliter_sw_ADSampleData(LeADFliter_u_Index);
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt++;
		/*防止溢出，滤波数组填充完成*/
		if(CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index] == SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt)
		{
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt = STD_NULL;//清零，确保下次循环重新填充
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_FliterArrayFullFlag = STD_ACTIVE;
		}
		else
		{}
	}
}



/******************************************************
*函数名：static void ADFliter_ADFliterDeal(void)

*形  参：void

*返回值：void

*描  述：对填充好的滤波数组滤波处理

*备  注：
******************************************************/
static void ADFliter_ADFliterDeal(void)
{
	uint8 LeADFliter_u_Index;
	uint8 LeADFliter_u_Middle;
	for(LeADFliter_u_Index=0;LeADFliter_u_Index < ADFLITERCFG_CHANNELNUM;LeADFliter_u_Index++)
	{
		if(STD_ACTIVE == SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_FliterArrayFullFlag)
		{
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_FliterArrayFullFlag = STD_INACTIVE;
			/*****************冒泡排序****************/
			ALibrary_AscendingBubbleSort(SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData,	\
										 CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index]);
			/*****************************************/
			LeADFliter_u_Middle = (CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index] >> 1);
			if(0 == (CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index]%2))//滤波数组中数据个数为偶数
			{//取中间2个值的均值作为AD滤波结果
				SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = \
				((SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[LeADFliter_u_Middle] \
				+ SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[(LeADFliter_u_Middle-1)]) >> 1);
			}
			else//滤波数组中数据个数为奇数
			{                                                            //取中间值作为AD滤波结果
				SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = \
					SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[LeADFliter_u_Middle]; 
			}
		}
		else
		{}
	}
}


/******************************************************
*函数名：int16 GetADFliter_ADFliterResult(uint8 Index_CH)

*形  参：Index_CH AD通道索引

*返回值：void

*描  述：根据AD滤波通道索引获取滤波处理后的AD结果

*备  注：
******************************************************/
int16 GetADFliter_ADFliterResult(uint8 LeADFliter_u_IndexCH)
{
	return SaADFliter_h_FliterParm[LeADFliter_u_IndexCH].e_sw_ADResult;
}

/******************************************************
*函数名：void ALibrary_AscendingBubbleSort(int *ptr, int size)

*形  参：void

*返回值：void

*描  述：

*备  注：
******************************************************/
static void ALibrary_AscendingBubbleSort(int *ptr, uint8 size)
{
	int i,j,temp;

	for(i=0;i < (size-1);i++)
	{
		for(j=(size-1);j > i;j--)
		{
			if(ptr[j] < ptr[j-1])
			{
				temp = ptr[j];
				ptr[j] = ptr[j-1];
				ptr[j-1] = temp;
			}
		}
	}
}


