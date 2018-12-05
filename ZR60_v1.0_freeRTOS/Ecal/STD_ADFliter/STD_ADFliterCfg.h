/******************************************************
文件名：	STD_ADFliterCfg

描述：		 

Data			  Vasion			author
2018/6/27		V1.0			  liujian         
*******************************************************/
#ifndef		STD_ADFLITER_CFG_H
#define		STD_ADFLITER_CFG_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"STD_ADFliterTypes.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/
#define   ADFLITERCFG_ADARRAYLENGTH_MAX      	10//存储AD数值的数组最大长度
#define   ADFLITERCFG_CHANNELNUM             	1//AD通道数

/***********宏函数***********/
 #define   GetADFliter_sw_ADSampleData(ADChannelIndex)    ADC_ConvertedValue
/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct 
{
  uint8 e_u_FliterArrayFullFlag;//滤波数组填充满标志
  int 	e_sw_ADData[ADFLITERCFG_ADARRAYLENGTH_MAX];//存储AD值的数组
  uint8 e_u_ADDataCnt;//AD滤波数组填充计数器
  int16 e_sw_ADResult; //滤波处理的结果  
}TsADFliter_h_FliterParm;//AD滤波参数


/******enum definitions******/
//typedef enum
//{
  
//}

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/
extern const uint8 CaADFliter_u_ADChannelIndex[ADFLITERCFG_CHANNELNUM];//从底层获获取AD采样值的通道索引
extern const int16 CaADFliter_sw_DefaultADResult[ADFLITERCFG_CHANNELNUM]; //上电缺省AD值
extern const uint8 CaADFliter_u_ADDataSampleNum[ADFLITERCFG_CHANNELNUM]; //各滤波数组中存储的AD数据采样个数
extern const uint8 CaADFliter_u_GetADTimeReach;   //获取AD数据的延时时间到
/*******************************************************
description： function External declaration
*******************************************************/

    
#endif

