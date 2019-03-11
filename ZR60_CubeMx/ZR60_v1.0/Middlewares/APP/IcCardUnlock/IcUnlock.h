/******************************************************
文件名：	IcUnlock

描述：		

Data			  Vasion			author
2018/04/7		  V1.0			    liujian
*******************************************************/
#ifndef		_IC_UNLOCK_H
#define		_IC_UNLOCK_H
/*******************************************************
description： include the header file
*******************************************************/
#include "IcCardUnlock/IcUnlock_cfg.h"

/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/

/**********宏常量定义*********/
#define  IC_UNLOCK_SCHEDULING_CYCLE		  10U//调度周期


#define IC_UNLOCK_DEVICE_TYPE  1

#define OK  			0x00//获取的数据有效
/**
* 获取到卡号
*/
#define GetCard_Num  0x00
/**
* 加载key
*/
#define Load_Key  0x01
/**
* 激活卡片并获取卡片号
*/
#define Get_CardSnr  0x02
/**
* 设置为主动检查卡片模式 IDLE模式
*/
#define SetDetectCard_ALL  0x03
/**
* 扇区0x14 存储了小区ID信息communityId
*/
#define Block_0x14  0x14
/**
* 扇区0x15 存储了楼栋编号信息buildNum1
*/
#define Block_0x15  0x15
/**
* 扇区0x16 村粗了楼栋编号信息buildNum2
*/
#define Block_0x16  0x16
/**
 * 扇区0x18 楼栋id
 */
#define Block_0x18  0x18
/**
 * 扇区0x19 门id
 */
#define Block_0x19  0x19
/**
 * 扇区0x20 下标
 */
#define Block_0x1A  0x1a
/**
* 数据块10, 旧IC卡存储用户ID块
*/
#define Block_0x10  0x10
/**
* 数据块11, 旧IC卡存储手机号码的块
*/
#define Block_0x11 0x11

/***********宏函数***********/
#define GetIcUnlock_u_TxIdle()  1//GetUartCmn_u_TxIdle()


/*******************************************************
description： struct definitions
*******************************************************/

/*******************************************************
description： typedef definitions
*******************************************************/
/*****struct definitions*****/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/


/*******************************************************
description： function External declaration
*******************************************************/
extern void  InitIcUnlock_parameter(void);
extern void  TskIcUnlock_MainFunction(void);
extern uint8_t	GetIcUnlock_u_CardSetSt(void);
extern char IcUnlock_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len);
extern void SetIcUnlock_u_CardReadSuccess(void);
extern void GetIcUnlock_communityID(unsigned char* community_id);
extern void GetIcUnlock_buildID(unsigned char* build_id);
extern void GetIcUnlock_doorID(unsigned char* door_id);
extern unsigned short GetIcUnlock_w_suffix(void);
extern char GetIcUnlock_u_MachineType(void);
#endif 
