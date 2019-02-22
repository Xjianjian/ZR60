/******************************************************
�ļ�����	BtnFltr

������		
Data			Vasion			author
2017/7/22		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"BtnFltr/BtnFltr.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static BtnFltrStruct  SaBtnFltr_h_BtnPara[BTNFLTR_BTN_NUM];/*���������ṹ��*/


/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static void BtnFltr_FltrDeal(void);


/******************************************************
description�� function code
******************************************************/

/******************************************************
*��������InitBtnFltr_Parameter

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void InitBtnFltr_Parameter(void)
{
	uint8 LeBtnFltr_u_Index;
	for(LeBtnFltr_u_Index = 0U;LeBtnFltr_u_Index < BTNFLTR_BTN_NUM;LeBtnFltr_u_Index++)
	{
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = !CaBtnFltr_u_BtnActive[LeBtnFltr_u_Index];
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*�����̰�����״̬��0--�޶̰���1--�̰����� */
		//SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtValid = STD_INACTIVE;/*�����̰�������Ч�ԣ�0--��Ч��1--��Ч*/
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;
		//SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngValid = STD_INACTIVE;
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_INACTIVE;
		SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = STD_INACTIVE;	
	}
}


/******************************************************
*��������TskETP_MainFunction

*��  �Σ�

*����ֵ��

*��  ������������

*��  ע����������5ms
******************************************************/
void TskBtnFltr_MainFunction(void)
{
	BtnFltr_FltrDeal();
}

/******************************************************
*��������BtnFltr_FltrDeal

*��  �Σ�

*����ֵ��

*��  ��������״̬���̰����������������˲�����

*��  ע����������5ms
******************************************************/
static void BtnFltr_FltrDeal(void)
{
	static uint8 LeBtnFltr_u_Index;
	static uint8 LeBtnFltr_u_BtnSt;
	for(LeBtnFltr_u_Index = 0U;LeBtnFltr_u_Index < BTNFLTR_BTN_NUM;LeBtnFltr_u_Index++)
	{
		LeBtnFltr_u_BtnSt = GetBtnFltrCfg_u_BtnRTSt(LeBtnFltr_u_Index);
		if(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt != LeBtnFltr_u_BtnSt)/*����״̬��������*/
		{			
			if(CaBtnFltr_u_BtnActive[LeBtnFltr_u_Index] == LeBtnFltr_u_BtnSt)/*������ǰʵ��״̬��Ч������*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt = STD_ACTIVE;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = LeBtnFltr_u_BtnSt;
			}
			else/*�����ͷ�*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt = STD_INACTIVE;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnOldSt = LeBtnFltr_u_BtnSt;
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*��̰�������־*/
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;/*�峤��������־*/			
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_INACTIVE;/*�忨����־*/
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = 0U;	/*���˲���ʱ��*/				
			}
		}
		
		if(STD_ACTIVE == SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnSt)/*����δ�ͷ�*/
		{
			SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt++;/*�˲���ʱ*/
			if(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt >= BTNFLTR_STUCKTIME)/*������������ʱ����������*/
			{
				SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt = BTNFLTR_STUCKTIME;
			}
			switch(SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_w_BtnFltrCnt)
			{
				case BTNFLTR_SHORTTIME://�����̰�
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_ACTIVE;/*�̰�����*/
				}
				break;
				case BTNFLTR_LONGTIME://��������
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_ACTIVE;/*��������*/
				}
				break;
				case BTNFLTR_STUCKTIME://��������
				{
					SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt = STD_ACTIVE;/*��������*/
				}
				break;
				default:
				break;
			}
		}
	}
}


/******************************************************
*��������GetBtnFltr_u_BtnShrtSt

*��  �Σ�

*����ֵ��

*��  ������ȡ�̰�״̬

*��  ע��
******************************************************/
uint8 GetBtnFltr_u_BtnShrtSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt;
}



/******************************************************
*��������GetBtnFltr_BtnLngSt

*��  �Σ�

*����ֵ��

*��  ������ȡ����״̬

*��  ע��
******************************************************/
uint8 GetBtnFltr_u_BtnLngSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt;
}


/******************************************************
*��������GetBtnFltr_u_BtnStuckSt

*��  �Σ�

*����ֵ��

*��  ������ȡ��������״̬

*��  ע��
******************************************************/
uint8 GetBtnFltr_u_BtnStuckSt(uint8 LeBtnFltr_u_Index)
{
	return SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnStuckSt;
}

/******************************************************
*��������ClrBtnFltr_BtnLngValid

*��  �Σ�

*����ֵ��

*��  �����峤����Ч��

*��  ע��
******************************************************/
void ClrBtnFltr_BtnLngValid(uint8 LeBtnFltr_u_Index)
{
	 SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnLngSt = STD_INACTIVE;/*�峤��������Ч��*/
}

/******************************************************
*��������ClrBtnFltr_BtnShrtValid

*��  �Σ�

*����ֵ��

*��  ������̰���Ч��

*��  ע��
******************************************************/
void ClrBtnFltr_BtnShrtValid(uint8 LeBtnFltr_u_Index)
{
	SaBtnFltr_h_BtnPara[LeBtnFltr_u_Index].e_u_BtnShrtSt = STD_INACTIVE;/*��̰�������Ч��*/
}

