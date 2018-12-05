/******************************************************
文件名：	BListMng_cfg.c

描述：		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "BListMng_cfg.h"

//uint8 CeMemIfCfg_u_CardSetLng = EE_CARD_SET_LNG;
/*******************************************************
description： global variable definitions
*******************************************************/
BListMng_Struct  CardNumCache[BLISTMNG_BLIST_NUM];
BListMng_Struct  CardNumCacheBkUp[BLISTMNG_BLIST_NUM];
BListMng_cardNumStruct CARD_Array[BLISTMNG_CARD_NUM];
/*******************************************************
description： static variable definitions
*******************************************************/
#ifdef BLISTMNG_SAVEPOWEROFF
const uint8 CeBListMng_u_BlockIndex[BLISTMNG_BLIST_NUM] =
{
	EepromCfg_Blist_one,//黑名单数据块索引1
	EepromCfg_Blist_two,//黑名单数据块索引2
	EepromCfg_Blist_three,//黑名单数据块索引3
	EepromCfg_Blist_four, //黑名单数据块索引4
	EepromCfg_Blist_five,//黑名单数据块索引5
	EepromCfg_Blist_six, //黑名单数据块索引6
	EepromCfg_Blist_seven,//黑名单数据块索引7
	EepromCfg_Blist_eight, //黑名单数据块索引8
	EepromCfg_Blist_nine, //黑名单数据块索引9
	EepromCfg_Blist_ten //黑名单数据块索引10
};
#endif

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*函数名：BListMngCfg_WrEE
 
*形  参：
 
*返回值：void
 
*描  述：写Flash
 
*备  注：
******************************************************/
void BListMngCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*函数名：BListMngCfg_RdEE
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
******************************************************/
void BListMngCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_ReadEE(Le_u_Obj,Le_u_Data,Le_w_Lng);
}


/******************************************************
*函数名：GetBListMngCfg_EEIdle
 
*形  参：
 
*返回值：void
 
*描  述：读Flash
 
*备  注：
******************************************************/
uint8 GetBListMngCfg_EEIdle(void)
{
	uint8 ret = 0U;
	if((1U == GetMemIf_u_Idle()) && (1U == MemIfCfg_FLASH_IDLE()))
	{
		ret = 1U;
	}
	return ret;
}

/******************************************************
*函数名：GetMemIf_u_DtVild

*形  参：

*返回值：

*描  述：获取扇区数据有效性

*备  注：
******************************************************/
uint8 GetBListMngCfg_u_DtVild(uint8 Obj)
{

	return GetMemIf_u_DtVild(Obj);
}

/******************************************************
*函数名：GetMemIf_w_DtLng

*形  参：

*返回值：

*描  述：获取扇区有效数据长度

*备  注：
******************************************************/
uint16 GetBListMngCfg_w_DtLng(uint8 Obj)
{
	return GetMemIf_w_DtLng(Obj);
}





