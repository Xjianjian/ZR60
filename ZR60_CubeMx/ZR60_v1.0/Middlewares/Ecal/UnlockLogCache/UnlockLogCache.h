/******************************************************
文件名：	UnlockLogCache

描述：		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		UNLOCKLOGCACHE_H
#define		UNLOCKLOGCACHE_H
/*******************************************************
description： include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/
#define UnlockLogCache_QUEUE_LNG  100U/*数据队列长*/
//#define UnlockLogCache_TIME_LNG  4U/*时间数据长*/
#define UnlockLogCache_ID_LNG  32U/*开锁信息数据长*/
//#define UnlockLogCache_EVENT_LNG  4U/*事件数据长*/
/***********宏函数***********/

/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/
/*
0表示未知，1表示门卡开锁，2表示呼叫开锁，
3表示密码开锁，4表示手机NFC开锁，5表示近距离wifi开锁，
6表示近距离蓝牙开锁，7表示远距离开锁，8表示微信开锁，
9表示临时密码开锁，10表示人脸识别开锁，11表示二维码开锁，
12表示快照开锁，13身份证开锁，14声波,15门内开关,16消防警报
*/
/*
刷卡：卡id ; 呼叫开锁：房号、管理员、手机号 ; 
密码开锁：房号 ; NFC开锁：暂定 ; 近距离wifi：房间号，已弃用 ; 
蓝牙开锁：手机号 ; 远距离开锁：手机号、中心机id ; 微信开锁：open_id ; 
临时密码开锁：未知 ; 人脸识别：类似{"user_id":"0001_1024_666_18183037622","face_token":"c48a71f1fff887617cd4528b531cb4d9"} ; 
二维码开锁: 楼栋号_房间号/未知 ; 快照开锁：楼栋号_房间号/手机号 ; 身份证开锁：身份证卡号
*/
#define UNLOCKLOGCACHE_UNKNOW   	0U
#define UNLOCKLOGCACHE_CARD     	1U
#define UNLOCKLOGCACHE_CALL   		2U
#define UNLOCKLOGCACHE_PASSWORD   	3U
#define UNLOCKLOGCACHE_NFC     		4U
#define UNLOCKLOGCACHE_WIFI         5U
#define UNLOCKLOGCACHE_BLE         	6U
#define UNLOCKLOGCACHE_REMOTE         7U
#define UNLOCKLOGCACHE_WECHAT         	8U
#define UNLOCKLOGCACHE_TEMPPSWORD     	9U
#define UNLOCKLOGCACHE_FACE         	10U
#define UNLOCKLOGCACHE_TWODIME         	11U
#define UNLOCKLOGCACHE_SNAP         	12U
#define UNLOCKLOGCACHE_IDCARD         	13U
#define UNLOCKLOGCACHE_DOORSWITCH       14U
#define UNLOCKLOGCACHE_FIREALARM        15U
#define UNLOCKLOGCACHE_SOUNDWAVE        16U


typedef struct
{
	uint8  e_u_NonEmptyFlg;	/*数据非空标志*/
	uint64_t  e_u_Time;    /*时间*/
	char  e_u_ID[UnlockLogCache_ID_LNG];/*开锁信息*/
	uint8  e_u_Event;/*开锁方式*/
}UnlockLogCacheStruct;/*按键信号数据队列结构体*/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/

/*******************************************************
description： function External declaration
*******************************************************/
extern void InitUnlockLogCache_Parameter(void);
extern void WrUnlockLogCache_Queue(uint64_t* Time,char* Id,uint8 Event);
extern uint8 GetUnlockLogCache_u_LogAvild(void);
extern void GetUnlockLogCache_doorLog(UnlockLogCacheStruct* UnlockLog);
extern void ClrUnlockLogCache_Queue(void);

#endif
