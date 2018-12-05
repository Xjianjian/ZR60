/******************************************************
æ–‡ä»¶åï¼š	BListMng.h

æè¿°ï¼	

Data			  Vasion			author
2018/4/12		  V1.0			    liujian
*******************************************************/
#ifndef		BLISTMNG_H
#define		BLISTMNG_H
/*******************************************************
descriptionï¼include the header file
*******************************************************/
#include "BListMng_cfg.h"
#include "MyType.h"
#include "stm32f4xx.h"

/*******************************************************
descriptionï¼macro definitions
*******************************************************/
/**********å®å¼€å…³å®šä¹********/

/**********å®å¸¸é‡å®šä¹********/



/***********å®å‡½æ•**********/

/*******************************************************
descriptionï¼struct definitions
*******************************************************/


/*******************************************************
descriptionï¼typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
descriptionï¼variable External declaration
*******************************************************/


/*******************************************************
descriptionï¼function External declaration
*******************************************************/
extern void BListMng_Init(void);
extern void TskBListMng_MainFunction(void);
extern uint8 BListMng_u_ContrastBList(uint32 Le_dw_data);
extern void ClrBListMng_ListData(void);
#endif
