/******************************************************
�ļ�����	PSFltr

������		
Data			Vasion			author
2018/6/28		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "./PeriStFliter/PSFltr.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static PSFltrStruct  SaPSFltr_h_Para[PSFLTR_NUM];/*���������ṹ��*/


/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static void PSFltr_FltrDeal(void);


/******************************************************
description�� function code
******************************************************/

/******************************************************
*��������InitPSFltr_Parameter

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
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
*��������TskPSFltr_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע����������5ms
******************************************************/
void TskPSFltr_MainFunction(void)
{
	PSFltr_FltrDeal();
}

/******************************************************
*��������PSFltr_FltrDeal

*��  �Σ�

*����ֵ��

*��  ����

*��  ע����������5ms
******************************************************/
static void PSFltr_FltrDeal(void)
{
	uint8 LePSFltr_u_Index;
	uint8 LePSFltr_u_St;
	for(LePSFltr_u_Index = 0U;LePSFltr_u_Index < PSFLTR_NUM;LePSFltr_u_Index++)
	{
		LePSFltr_u_St = GetPSFltrCfg_u_RTSt(LePSFltr_u_Index);
		if(CaPSFltr_u_Active[LePSFltr_u_Index] == LePSFltr_u_St)/*��/�ص�ǰʵ��״̬������*/
		{
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt =0U;
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt++;/*�˲���ʱ*/
			if(SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt >= PSFLTR_FLTRTIME)/*�����̰�����ʱ����������*/
			{
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt =0U;
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt = STD_ON;/*��״̬*/
			}
		}
		else/*��/�ص�ǰʵ��״̬���ر�*/
		{
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OnFltrCnt =0U;
			SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt++;/*�˲���ʱ*/
			if(SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt >= PSFLTR_FLTRTIME)/*�����̰�����ʱ����������*/
			{
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_OffFltrCnt =0U;
				SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt = STD_OFF;/*��״̬*/
			}
		}
	}
}


/******************************************************
*��������GetPSFltr_u_PeriSt

*��  �Σ�

*����ֵ��

*��  ������ȡ����״̬

*��  ע��
******************************************************/
uint8 GetPSFltr_u_PeriSt(uint8 LePSFltr_u_Index)
{
	return SaPSFltr_h_Para[LePSFltr_u_Index].e_u_PSt;
}

