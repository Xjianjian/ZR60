/******************************************************
�ļ�����	STD_ADFliterCfg

������		

Data			  Vasion			author
2018/6/27		  V1.0			  liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"STD_ADFliterCfg.h"

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/

/*******************************************************
description�� global variable definitions
*******************************************************/
const uint8 CaADFliter_u_ADChannelIndex[ADFLITERCFG_CHANNELNUM] = //�ӵײ���ȡAD����ֵ��ͨ������
{0};

const int16 CaADFliter_sw_DefaultADResult[ADFLITERCFG_CHANNELNUM] = //�ϵ�ȱʡADֵ
{10};

const uint8 CaADFliter_u_ADDataSampleNum[ADFLITERCFG_CHANNELNUM] =   //���˲������д洢��AD���ݲ�������
{8}; 

const uint8 CaADFliter_u_GetADTimeReach = 1;   //��ȡAD���ݵ���ʱʱ�䵽ʵ��ʱ�� = CaADFliter_u_GetADTimeReach*��������
/*******************************************************
description�� static variable definitions
*******************************************************/


/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��
******************************************************/
