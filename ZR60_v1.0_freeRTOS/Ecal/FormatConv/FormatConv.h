/******************************************************
文件名：	FormatConv.h

描述：
Data			  Vasion			author
2019/1/18		  V1.0			    liujian
*******************************************************/
#ifndef		_FORMAT_CONV_H
#define		_FORMAT_CONV_H

/*******************************************************
description锛歩nclude the header file
*******************************************************/
/*C标准库文件*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Include.h"

/*******************************************************
description锛歮acro definitions
*******************************************************/
/**********瀹忓紑鍏冲畾涔*******/


/**********瀹忓父閲忓畾涔*******/



/***********瀹忓嚱鏁*********/

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
extern void FormatConv_StrToHex(char* Le_in,unsigned char* Le_out,unsigned int Le_w_lng);
extern void FormatConv_MacAddFlags(char* Le_u_mac);
extern char FormatConv_HexToChar(unsigned char Le_u_dt);
extern void FormatConv_IpAddComma(unsigned long Le_dw_in,char* Le_u_out);
#endif
