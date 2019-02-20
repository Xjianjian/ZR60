/******************************************************
文件名：	BtnFltr

描述：		 

Data			  Vasion			author
2017/7/22		   V1.0			    liujian      
*******************************************************/
#ifndef		BTNFLTR_CFG_H
#define		BTNFLTR_CFG_H
/*******************************************************
description： include the header file
*******************************************************/
#include "Include.h"


/*******************************************************
description： macro definitions
*******************************************************/
/**********宏开关定义*********/


/**********宏常量定义*********/



#define  BTNFLTR_SHORTTIME   4U/*按键短按时间门限*/
#define  BTNFLTR_LONGTIME    500U/*按键长按时间门限*/
#define  BTNFLTR_STUCKTIME   4000U/*按键卡键时间门限*/
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
	uint8 e_u_BtnOldSt;	  /*按键前一次状态*/
	uint8 e_u_BtnShrtSt;  /*按键短按触发状态*/
	uint8 e_u_BtnSt;  /*按键状态:0 -- 释放；1 -- 按下 */
	//uint8 e_u_BtnShrtValid;  /*按键短按触发有效性*/
	uint8 e_u_BtnLngSt;   /*按键长按触发状态*/
	//uint8 e_u_BtnLngValid;  /*按键长按触发有效性*/
	uint8 e_u_BtnStuckSt; /*按键卡键状态*/
	uint16 e_w_BtnFltrCnt;/*按键状态滤波计时器*/	
}BtnFltrStruct;/*按键参数结构体*/


/******enum definitions******/
typedef enum
{
	Key_card_set = 0U,	  /*母卡设置*/
	//Key_PasswodAuth,	  /*密码验证*/
	Key_Unlock,	  /*门内开锁*/
	//Key_Alarm	  /*消防报警*/
	BTNFLTR_BTN_NUM/*按键数量*/
}BtnFltrEnum;

/******union definitions*****/

/*******************************************************
description： variable External declaration
*******************************************************/
extern const uint8  CaBtnFltr_u_BtnActive[BTNFLTR_BTN_NUM];
extern const uint16 CaBtnFltr_w_BtnStuckThre[BTNFLTR_BTN_NUM];


/*******************************************************
description： function External declaration
*******************************************************/
extern uint8 GetBtnFltrCfg_u_BtnRTSt(uint8 LeBtnFltr_u_Index);
    
#endif

