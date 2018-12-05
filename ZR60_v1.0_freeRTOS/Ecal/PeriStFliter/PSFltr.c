/******************************************************
文件名：	PSFltr

描述：		
Data			Vasion			author
2018/6/28		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "./PeriStFliter/PSFltr.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static PSFltrStruct  SaPSFltr_h_Para[PSFLTR_NUM];/*按键参数结构体*/


/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static void PSFltr_FltrDeal(void);


/******************************************************
description： function code
******************************************************/

/******************************************************
*函数名：InitPSFltr_Parameter

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void InitPSFltr_Parameter(void)
{
	uint8 LePSFltr_u_Index;
	for(LePSFltr_u_Index = 0U;LePSFltr_u_Index < PSFLTR_NUM;LePSFltr_u_Index++)
	{
		SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt = STD_OFF;
		SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt = STD_NULL;
		SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt = STD_NULL;
	}
}


/******************************************************
*函数名：TskPSFltr_MainFunction

*形  参：

*返回值：

*描  述：主任务函数

*备  注：调用周期5ms
******************************************************/
void TskPSFltr_MainFunction(void)
{
	PSFltr_FltrDeal();
}

/******************************************************
*函数名：PSFltr_FltrDeal

*形  参：

*返回值：

*描  述：

*备  注：调用周期5ms
******************************************************/
static void PSFltr_FltrDeal(void)
{
	uint8 LePSFltr_u_Index;
	uint8 LePSFltr_u_St;
	for(LePSFltr_u_Index = 0U;LePSFltr_u_Index < PSFLTR_NUM;LePSFltr_u_Index++)
	{
		LePSFltr_u_St = GetPSFltrCfg_u_RTSt(LePSFltr_u_Index);
		if(CaPSFltr_u_Active[LePSFltr_u_Index] == LePSFltr_u_St)/*开/关当前实际状态：开启*/
		{
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt =0U;
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt++;/*滤波计时*/
			if(SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt >= PSFLTR_FLTRTIME)/*按键短按触发时间门限条件*/
			{
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt =0U;
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt = STD_ON;/*开状态*/
			}
		}
		else/*开/关当前实际状态：关闭*/
		{
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt =0U;
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt++;/*滤波计时*/
			if(SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt >= PSFLTR_FLTRTIME)/*按键短按触发时间门限条件*/
			{
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt =0U;
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt = STD_OFF;/*关状态*/
			}
		}
	}
}


/******************************************************
*函数名：GetPSFltr_u_PeriSt

*形  参：

*返回值：

*描  述：获取外设状态

*备  注：
******************************************************/
uint8 GetPSFltr_u_PeriSt(uint8 LePSFltr_u_Index)
{
	return SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt;
}

