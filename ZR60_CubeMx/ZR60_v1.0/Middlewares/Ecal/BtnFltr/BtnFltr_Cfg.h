/******************************************************
�ļ�����	BtnFltr

������		 

Data			  Vasion			author
2017/7/22		   V1.0			    liujian      
*******************************************************/
#ifndef		BTNFLTR_CFG_H
#define		BTNFLTR_CFG_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"


/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/


/**********�곣������*********/



#define  BTNFLTR_SHORTTIME   4U/*�����̰�ʱ������*/
#define  BTNFLTR_LONGTIME    500U/*��������ʱ������*/
#define  BTNFLTR_STUCKTIME   4000U/*��������ʱ������*/
/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint8 e_u_BtnOldSt;	  /*����ǰһ��״̬*/
	uint8 e_u_BtnShrtSt;  /*�����̰�����״̬*/
	uint8 e_u_BtnSt;  /*����״̬:0 -- �ͷţ�1 -- ���� */
	//uint8 e_u_BtnShrtValid;  /*�����̰�������Ч��*/
	uint8 e_u_BtnLngSt;   /*������������״̬*/
	//uint8 e_u_BtnLngValid;  /*��������������Ч��*/
	uint8 e_u_BtnStuckSt; /*��������״̬*/
	uint16 e_w_BtnFltrCnt;/*����״̬�˲���ʱ��*/	
}BtnFltrStruct;/*���������ṹ��*/


/******enum definitions******/
typedef enum
{
	Key_card_set = 0U,	  /*ĸ������*/
	//Key_PasswodAuth,	  /*������֤*/
	Key_Unlock,	  /*���ڿ���*/
	//Key_Alarm	  /*��������*/
	BTNFLTR_BTN_NUM/*��������*/
}BtnFltrEnum;

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/
extern const uint8  CaBtnFltr_u_BtnActive[BTNFLTR_BTN_NUM];
extern const uint16 CaBtnFltr_w_BtnStuckThre[BTNFLTR_BTN_NUM];


/*******************************************************
description�� function External declaration
*******************************************************/
extern uint8 GetBtnFltrCfg_u_BtnRTSt(uint8 LeBtnFltr_u_Index);
    
#endif

