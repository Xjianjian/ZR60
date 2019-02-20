/******************************************************
�ļ�����	STD_ADFliterCfg

������		 

Data			  Vasion			author
2018/6/27		V1.0			  liujian         
*******************************************************/
#ifndef		STD_ADFLITER_CFG_H
#define		STD_ADFLITER_CFG_H
/*******************************************************
description�� include the header file
*******************************************************/
#include	"STD_ADFliterTypes.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/


/**********�곣������*********/
#define   ADFLITERCFG_ADARRAYLENGTH_MAX      	10//�洢AD��ֵ��������󳤶�
#define   ADFLITERCFG_CHANNELNUM             	1//ADͨ����

/***********�꺯��***********/
 #define   GetADFliter_sw_ADSampleData(ADChannelIndex)    ADC_ConvertedValue
/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{
  uint8 e_u_FliterArrayFullFlag;//�˲������������־
  int 	e_sw_ADData[ADFLITERCFG_ADARRAYLENGTH_MAX];//�洢ADֵ������
  uint8 e_u_ADDataCnt;//AD�˲�������������
  int16 e_sw_ADResult; //�˲�����Ľ��  
}TsADFliter_h_FliterParm;//AD�˲�����


/******enum definitions******/
//typedef enum
//{
  
//}

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/
extern const uint8 CaADFliter_u_ADChannelIndex[ADFLITERCFG_CHANNELNUM];//�ӵײ���ȡAD����ֵ��ͨ������
extern const int16 CaADFliter_sw_DefaultADResult[ADFLITERCFG_CHANNELNUM]; //�ϵ�ȱʡADֵ
extern const uint8 CaADFliter_u_ADDataSampleNum[ADFLITERCFG_CHANNELNUM]; //���˲������д洢��AD���ݲ�������
extern const uint8 CaADFliter_u_GetADTimeReach;   //��ȡAD���ݵ���ʱʱ�䵽
/*******************************************************
description�� function External declaration
*******************************************************/

    
#endif

