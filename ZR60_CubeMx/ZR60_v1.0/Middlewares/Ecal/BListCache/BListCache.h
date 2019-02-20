/******************************************************
文件名：	BListCache

描述：		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		BLISTCACHE_H
#define		BLISTCACHE_H
/*******************************************************
description： include the header file
*******************************************************/
#include	"Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/
#define BLISTCACHE_QUEUE_LNG  100U/*数据队列长,队列长设为一次拉取黑名单最大个数的2倍，防止短时间连续拉取2次黑名单时缓存队列溢出*/
#define BLISTCACHE_DATA_LNG   4U/*数据长*/
/***********宏函数***********/

/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint8  e_u_NonEmptyFlg;	/*数据非空标志*/
	uint8  e_u_Data[BLISTCACHE_DATA_LNG];/*数据*/
}BListCacheStruct;/*按键信号数据队列结构体*/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/

/*******************************************************
description： function External declaration
*******************************************************/
extern void InitBListCache_Parameter(void);
extern void WrBListCache_BListQueue(const uint8* LeBListCache_u_data);
extern uint8 GetBListCache_u_BListValidity(void);
extern uint32 GetBListCache_dw_BlistData(void);
extern void ClrBListCache_Queue(void);

#endif
