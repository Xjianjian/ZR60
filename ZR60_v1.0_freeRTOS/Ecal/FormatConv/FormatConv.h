/******************************************************
�ļ�����	FormatConv.h

������
Data			  Vasion			author
2019/1/18		  V1.0			    liujian
*******************************************************/
#ifndef		_FORMAT_CONV_H
#define		_FORMAT_CONV_H

/*******************************************************
description：include the header file
*******************************************************/
/*C��׼���ļ�*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Include.h"

/*******************************************************
description：macro definitions
*******************************************************/
/**********宏开关定�*******/


/**********宏常量定�*******/



/***********宏函�*********/

/*******************************************************
description：struct definitions
*******************************************************/

/*******************************************************
description：typedef definitions
*******************************************************/
/*****struct definitions*****/

/******enum definitions******/

/******union definitions*****/

/*******************************************************
description：variable External declaration
*******************************************************/


/*******************************************************
description：function External declaration
*******************************************************/
extern void FormatConv_StrToHex(char* Le_in,unsigned char* Le_out,unsigned int Le_w_lng);
extern void FormatConv_MacAddFlags(char* Le_u_mac);
extern char FormatConv_HexToChar(unsigned char Le_u_dt);
extern void FormatConv_IpAddComma(unsigned long Le_dw_in,char* Le_u_out);
#endif
