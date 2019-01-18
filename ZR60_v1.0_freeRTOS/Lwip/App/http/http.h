/**************************************************************
  * @file    http.h
  * @author  
  * @version V1.0
  * @date    22-11-2018
  * @brief
***************************************************************/
  
/* Define to prevent recursive inclusion ---------------------*/
#ifndef __HTTP_H__
#define __HTTP_H__

/*C��׼���ļ�*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Include.h"


/* Includes ---------------------------------------------------*/
/* Exported types ---------------------------------------------*/
/* Exported constants -----------------------------------------*/
/* Exported macro ---------------------------------------------*/
#define HTTP_GET					0//GET����
#define HTTP_POST					1//POST����


#define HTTP_FAIL					0//����ʧ��(��Ӧ���200)
#define HTTP_OK						1
#define HTTP_NOT_FRAMEHEAD			2//��֡ͷ
#define HTTP_OVERFLOW				3//���ݳ������

/* Exported struct --------------------------------------------*/
typedef struct
{
	unsigned int Content_lng;//
}http_ResPackage;



/* Exported functions -----------------------------------------*/
extern void http_RequestPackage(unsigned char ReqStye,char* Path,char* hostIpaddr,char* Le_u_in, \
									char* Le_u_out,unsigned char Le_u_keepAlive);
extern unsigned char http_MsgParser(char* payload,http_ResPackage* package);
#endif /**/

/********** (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
