/******************************************************
�ļ�����	STD_ADFliter

������		

Data			Vasion			author
2018/6/27		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"STD_ADFliter.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void ADFliter_PwrOnFastFliter(void);
static void ADFliter_FliterArrayFill(void);
static void ADFliter_ADFliterDeal(void);

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint8  SeADFliter_u_ADFliterSt;//AD�˲�״̬��״̬
static TsADFliter_h_FliterParm  SaADFliter_h_FliterParm[ADFLITERCFG_CHANNELNUM];
static uint8 SeADFliter_u_GetADTimer;//��ȡADֵ����ʱ��ʱ��

static void ALibrary_AscendingBubbleSort(int *ptr, uint8 size);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������void InitADFliter_parameter(void)

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitADFliter_parameter(void)
{
	uint8 LeADFliter_u_Index;
	uint8 LeADFliter_u_ArrayIndex;
	SeADFliter_u_ADFliterSt = ADFLITERTYPES_PWRONFASTFLITER;//�ϵ�����˲�״̬
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
*��������void MngADFliter_mainFunction(void)

*��  �Σ�void

*����ֵ��void

*��  ������������

*��  ע��
******************************************************/
void MngADFliter_mainFunction(void)
{
	switch(SeADFliter_u_ADFliterSt)
	{
		case  ADFLITERTYPES_PWRONFASTFLITER:
		{
			ADFliter_PwrOnFastFliter();//�ϵ�����˲�
			SeADFliter_u_ADFliterSt = ADFLITERTYPES_NORMALFLITER;//״̬���л�  
		}
		break;
		case  ADFLITERTYPES_NORMALFLITER:
		{
			if(CaADFliter_u_GetADTimeReach == SeADFliter_u_GetADTimer)
			{
				SeADFliter_u_GetADTimer = STD_NULL;
				ADFliter_FliterArrayFill();//����˲�����
				ADFliter_ADFliterDeal();//�����õ��˲������˲�����  
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
*��������static void ADFliter_PwrOnFastFliter(void)

*��  �Σ�void

*����ֵ��void

*��  �����ϵ�����˲�,���״β���ֵ��ΪAD�˲����ֵ

*��  ע��
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
*��������static void ADFliter_FliterArrayFill(void)

*��  �Σ�void

*����ֵ��void

*��  ��������˲�����

*��  ע��
******************************************************/
static void ADFliter_FliterArrayFill(void)
{
	uint8 LeADFliter_u_Index;
	for(LeADFliter_u_Index =0;LeADFliter_u_Index < ADFLITERCFG_CHANNELNUM;LeADFliter_u_Index++)
	{
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt] = \
			GetADFliter_sw_ADSampleData(LeADFliter_u_Index);
		SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt++;
		/*��ֹ������˲�����������*/
		if(CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index] == SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt)
		{
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_ADDataCnt = STD_NULL;//���㣬ȷ���´�ѭ���������
			SaADFliter_h_FliterParm[LeADFliter_u_Index].e_u_FliterArrayFullFlag = STD_ACTIVE;
		}
		else
		{}
	}
}



/******************************************************
*��������static void ADFliter_ADFliterDeal(void)

*��  �Σ�void

*����ֵ��void

*��  ���������õ��˲������˲�����

*��  ע��
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
			/*****************ð������****************/
			ALibrary_AscendingBubbleSort(SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData,	\
										 CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index]);
			/*****************************************/
			LeADFliter_u_Middle = (CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index] >> 1);
			if(0 == (CaADFliter_u_ADDataSampleNum[LeADFliter_u_Index]%2))//�˲����������ݸ���Ϊż��
			{//ȡ�м�2��ֵ�ľ�ֵ��ΪAD�˲����
				SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = \
				((SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[LeADFliter_u_Middle] \
				+ SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[(LeADFliter_u_Middle-1)]) >> 1);
			}
			else//�˲����������ݸ���Ϊ����
			{                                                            //ȡ�м�ֵ��ΪAD�˲����
				SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADResult = \
					SaADFliter_h_FliterParm[LeADFliter_u_Index].e_sw_ADData[LeADFliter_u_Middle]; 
			}
		}
		else
		{}
	}
}


/******************************************************
*��������int16 GetADFliter_ADFliterResult(uint8 Index_CH)

*��  �Σ�Index_CH ADͨ������

*����ֵ��void

*��  ��������AD�˲�ͨ��������ȡ�˲�������AD���

*��  ע��
******************************************************/
int16 GetADFliter_ADFliterResult(uint8 LeADFliter_u_IndexCH)
{
	return SaADFliter_h_FliterParm[LeADFliter_u_IndexCH].e_sw_ADResult;
}

/******************************************************
*��������void ALibrary_AscendingBubbleSort(int *ptr, int size)

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��
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


