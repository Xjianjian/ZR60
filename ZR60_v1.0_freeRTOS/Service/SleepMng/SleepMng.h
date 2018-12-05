/******************************************************
鏂囦欢鍚嶏細	SleepMng.h

鎻忚堪锛

Data			  Vasion			author
2018/8/6		  V1.0			    liujian
*******************************************************/
#ifndef		SLEEP_MNG_H
#define		SLEEP_MNG_H
/*******************************************************
description锛歩nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description锛歮acro definitions
*******************************************************/
#define  SLEEPMNG_SCHEDULING_CYCLE     (uint16)20U//调度周期  单位ms



/*******************************************************
description锛歴truct definitions
*******************************************************/

/*******************************************************
description锛歵ypedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description锛歷ariable External declaration
*******************************************************/


/*******************************************************
description锛歠unction External declaration
*******************************************************/
extern void InitSleepMng_parameter(void);
extern void TskSleepMng_MainFunction(void);

#endif
