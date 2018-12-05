/******************************************************
�ļ�����	BListMng_cfg.c

������		
Data			Vasion			author
2018/4/12		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "BListMng_cfg.h"

//uint8 CeMemIfCfg_u_CardSetLng = EE_CARD_SET_LNG;
/*******************************************************
description�� global variable definitions
*******************************************************/
BListMng_Struct  CardNumCache[BLISTMNG_BLIST_NUM];
BListMng_Struct  CardNumCacheBkUp[BLISTMNG_BLIST_NUM];
BListMng_cardNumStruct CARD_Array[BLISTMNG_CARD_NUM];
/*******************************************************
description�� static variable definitions
*******************************************************/
#ifdef BLISTMNG_SAVEPOWEROFF
const uint8 CeBListMng_u_BlockIndex[BLISTMNG_BLIST_NUM] =
{
	EepromCfg_Blist_one,//���������ݿ�����1
	EepromCfg_Blist_two,//���������ݿ�����2
	EepromCfg_Blist_three,//���������ݿ�����3
	EepromCfg_Blist_four, //���������ݿ�����4
	EepromCfg_Blist_five,//���������ݿ�����5
	EepromCfg_Blist_six, //���������ݿ�����6
	EepromCfg_Blist_seven,//���������ݿ�����7
	EepromCfg_Blist_eight, //���������ݿ�����8
	EepromCfg_Blist_nine, //���������ݿ�����9
	EepromCfg_Blist_ten //���������ݿ�����10
};
#endif

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*��������BListMngCfg_WrEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����дFlash
 
*��  ע��
******************************************************/
void BListMngCfg_WrEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_WriteEE( Le_u_Obj,Le_u_Data, Le_w_Lng);
}

/******************************************************
*��������BListMngCfg_RdEE
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
******************************************************/
void BListMngCfg_RdEE(uint8 Le_u_Obj,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	(void)MemIf_ReadEE(Le_u_Obj,Le_u_Data,Le_w_Lng);
}


/******************************************************
*��������GetBListMngCfg_EEIdle
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������Flash
 
*��  ע��
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
*��������GetMemIf_u_DtVild

*��  �Σ�

*����ֵ��

*��  ������ȡ����������Ч��

*��  ע��
******************************************************/
uint8 GetBListMngCfg_u_DtVild(uint8 Obj)
{

	return GetMemIf_u_DtVild(Obj);
}

/******************************************************
*��������GetMemIf_w_DtLng

*��  �Σ�

*����ֵ��

*��  ������ȡ������Ч���ݳ���

*��  ע��
******************************************************/
uint16 GetBListMngCfg_w_DtLng(uint8 Obj)
{
	return GetMemIf_w_DtLng(Obj);
}





