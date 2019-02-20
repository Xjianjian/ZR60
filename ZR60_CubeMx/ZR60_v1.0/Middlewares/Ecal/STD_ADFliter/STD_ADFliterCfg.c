/******************************************************
文件名：	STD_ADFliterCfg

描述：		

Data			  Vasion			author
2018/6/27		  V1.0			  liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"STD_ADFliterCfg.h"

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/

/*******************************************************
description： global variable definitions
*******************************************************/
const uint8 CaADFliter_u_ADChannelIndex[ADFLITERCFG_CHANNELNUM] = //从底层获获取AD采样值的通道索引
{0};

const int16 CaADFliter_sw_DefaultADResult[ADFLITERCFG_CHANNELNUM] = //上电缺省AD值
{10};

const uint8 CaADFliter_u_ADDataSampleNum[ADFLITERCFG_CHANNELNUM] =   //各滤波数组中存储的AD数据采样个数
{8}; 

const uint8 CaADFliter_u_GetADTimeReach = 1;   //获取AD数据的延时时间到实际时间 = CaADFliter_u_GetADTimeReach*调度周期
/*******************************************************
description： static variable definitions
*******************************************************/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：

*形  参：void

*返回值：void

*描  述：

*备  注：
******************************************************/
