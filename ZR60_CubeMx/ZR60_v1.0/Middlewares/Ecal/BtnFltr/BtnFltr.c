/******************************************************
文件名：	BtnFltr

描述：		
Data			Vasion			author
2017/7/22		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"BtnFltr/BtnFltr.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static BtnFltrStruct  SaBtnFltr_h_BtnPara[BTNFLTR_BTN_NUM];/*按键参数结构体*/


/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static void BtnFltr_FltrDeal(void);


/******************************************************
description： function code
******************************************************/

/******************************************************
*函数名：InitBtnFltr_Parameter

*形  参：

*返回值：

*描  述：

*备  注：
******************************************************/
void InitBtnFltr_Parameter(void)
{
	uint8 LeBtnFltr_u_Index;
	for(LeBtnFltr_u_Index = 0U;LeBtnFltr_u_Index < BTNFLTR_BTN_NUM;LeBtnFltr_u_Index++)
	{
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = !CaBtnFltr_u_BtnActive[LeBtnFltr_u_Index];
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*按键短按触发状态，0--无短按；1--短按触发 */
		//SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtValid = STD_INACTIVE;/*按键短按触发有效性，0--无效；1--有效*/
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;
		//SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngValid = STD_INACTIVE;
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_INACTIVE;
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = STD_INACTIVE;	
	}
}


/******************************************************
*函数名：TskETP_MainFunction

*形  参：

*返回值：

*描  述：主任务函数

*备  注：调用周期5ms
******************************************************/
void TskBtnFltr_MainFunction(void)
{
	BtnFltr_FltrDeal();
}

/******************************************************
*函数名：BtnFltr_FltrDeal

*形  参：

*返回值：

*描  述：按键状态：短按、长按、卡键，滤波处理

*备  注：调用周期5ms
******************************************************/
static void BtnFltr_FltrDeal(void)
{
	static uint8 LeBtnFltr_u_Index;
	static uint8 LeBtnFltr_u_BtnSt;
	for(LeBtnFltr_u_Index = 0U;LeBtnFltr_u_Index < BTNFLTR_BTN_NUM;LeBtnFltr_u_Index++)
	{
		LeBtnFltr_u_BtnSt = GetBtnFltrCfg_u_BtnRTSt(LeBtnFltr_u_Index);
		if(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt != LeBtnFltr_u_BtnSt)/*按键状态发生跳变*/
		{			
			if(CaBtnFltr_u_BtnActive[LeBtnFltr_u_Index] == LeBtnFltr_u_BtnSt)/*按键当前实际状态有效：按下*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt = STD_ACTIVE;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = LeBtnFltr_u_BtnSt;
			}
			else/*按键释放*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt = STD_INACTIVE;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = LeBtnFltr_u_BtnSt;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*清短按触发标志*/
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;/*清长按触发标志*/			
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_INACTIVE;/*清卡键标志*/
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = 0U;	/*清滤波计时器*/				
			}
		}
		
		if(STD_ACTIVE == SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt)/*按键未释放*/
		{
			SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt++;/*滤波计时*/
			if(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt >= BTNFLTR_STUCKTIME)/*按键卡键触发时间门限条件*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = BTNFLTR_STUCKTIME;
			}
			switch(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt)
			{
				case BTNFLTR_SHORTTIME://按键短按
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_ACTIVE;/*短按触发*/
				}
				break;
				case BTNFLTR_LONGTIME://按键长按
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_ACTIVE;/*长按触发*/
				}
				break;
				case BTNFLTR_STUCKTIME://按键卡键
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_ACTIVE;/*卡键触发*/
				}
				break;
				default:
				break;
			}
		}
	}
}


/******************************************************
*函数名：GetBtnFltr_u_BtnShrtSt

*形  参：

*返回值：

*描  述：获取短按状态

*备  注：
******************************************************/
uint8 GetBtnFltr_u_BtnShrtSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt;
}



/******************************************************
*函数名：GetBtnFltr_BtnLngSt

*形  参：

*返回值：

*描  述：获取长按状态

*备  注：
******************************************************/
uint8 GetBtnFltr_u_BtnLngSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt;
}


/******************************************************
*函数名：GetBtnFltr_u_BtnStuckSt

*形  参：

*返回值：

*描  述：获取按键卡键状态

*备  注：
******************************************************/
uint8 GetBtnFltr_u_BtnStuckSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt;
}

/******************************************************
*函数名：ClrBtnFltr_BtnLngValid

*形  参：

*返回值：

*描  述：清长按有效性

*备  注：
******************************************************/
void ClrBtnFltr_BtnLngValid(uint8 LeBtnFltr_u_Index)
{
	 SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;/*清长按触发有效性*/
}

/******************************************************
*函数名：ClrBtnFltr_BtnShrtValid

*形  参：

*返回值：

*描  述：清短按有效性

*备  注：
******************************************************/
void ClrBtnFltr_BtnShrtValid(uint8 LeBtnFltr_u_Index)
{
	SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*清短按触发有效性*/
}

