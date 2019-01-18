/**************************************************************
  * @file    http.c
  * @author  
  * @version V1.0
  * @date    22-11-2018
  * @brief   tcp client application using LwIP RAW API
***************************************************************/

/* Includes ---------------------------------------------------*/
#include "./http/http.h"

/* Private typedef --------------------------------------------*/
/* Private define ---------------------------------------------*/
/* Private macro ----------------------------------------------*/
/* Private variables ------------------------------------------*/


/*function prototypes ---------------------------------*/
/******************************************************
*��������http_RequestPackage

*��  �Σ�
		ReqStye:0 -- get
				1 -- post
		Path:·������"POST /doorcard/getPassList HTTP/1.1\n"	
		hostIpaddr:����ip����192.168.1.1
		Le_u_in:��������body
		Le_u_out:��װ����
		Le_u_keepAlive:����
		
*����ֵ��

*��  ����http �����ķ�װ

*��  ע��
******************************************************/
void http_RequestPackage(unsigned char ReqStye,char* Path,char* hostIpaddr,char* Le_u_in, \
							char* Le_u_out,unsigned char Le_u_keepAlive)
{
	char str[5U] = {0};
	
	if(0 == ReqStye)
	{
		strcat(Le_u_out, Path);
		strcat(Le_u_out, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n");
		//strcat(Le_u_out, "Accept-Encoding: gzip, deflate\r\n");
		//strcat(Le_u_out, "Accept-Language: zh-CN,zh;q=0.9\r\n");
		if(Le_u_keepAlive == 1U)
		{
			strcat(Le_u_out, "Connection: keep-alive\r\n");
		}
		else
		{
			strcat(Le_u_out, "Connection: close\r\n");
		}
		//strcat(Le_u_out, "Host: mobile.zzwtec.com\r\n");
		strcat(Le_u_out, "Host: ");
		strcat(Le_u_out, hostIpaddr);
		strcat(Le_u_out, "\r\n");
		strcat(Le_u_out, "\r\n\r\n");
	}
	else
	{
		sprintf(str, "%d", strlen(Le_u_in));
		strcat(Le_u_out, Path);
		strcat(Le_u_out, "Content-Type: application/json;charset=UTF-8\n");
		strcat(Le_u_out, "Host: ");
		strcat(Le_u_out, hostIpaddr);
		strcat(Le_u_out, "\n");
		strcat(Le_u_out, "Content-Length: ");
		strcat(Le_u_out, str);
		strcat(Le_u_out, "\n");
		if(Le_u_keepAlive == 1U)
		{
			strcat(Le_u_out, "Connection: keep-alive\n");
		}
		else
		{
			strcat(Le_u_out, "Connection: close\n");
		}
		strcat(Le_u_out, "\n");//��Ҫ��һ��
		strcat(Le_u_out, Le_u_in);//Le_u_in ��ֵΪpost������
		strcat(Le_u_out, "\r\n\r\n");
	}
}

/******************************************************
*��������http_MsgParser

*��  �Σ�

*����ֵ��package:��Ӧ֡�������ֶ�ֵ
		{
			.Content_lng:����body����
		}

*��  ����http ��Ӧ���Ľ���

*��  ע��
******************************************************/
unsigned char http_MsgParser(char* payload,http_ResPackage* package)
{
	unsigned int Le_w_i;
	char *position;
	char *endposition;
	unsigned char Le_u_CnntLngVal[7U] = {0};
	if(NULL != strstr(payload,"HTTP/1.1"))
	{//��һ֡����(֡ͷ)
		if(NULL != strstr(payload,"HTTP/1.1 200 OK"))
		{//�ޱ���ʱ������Content-Length��Content-Length�ֶε�ֵ�������ص������ܳ����ֽ���
			position = strstr(payload,"Content-Length: ") + 16U;//Content-Length�ֶ���ֵ����ʼλ��
			endposition = strchr(&(payload[position - payload]), '\r\n');//�״γ���'\r\n'�ĵ�ַ
			if((endposition - position) <= sizeof(Le_u_CnntLngVal))
			{
				for(Le_w_i = 0;Le_w_i < (endposition - position);Le_w_i++)
				{
					Le_u_CnntLngVal[Le_w_i] = payload[(position - payload) + Le_w_i];
				}
				package->Content_lng = atoi(Le_u_CnntLngVal);/*���ݳ���*/
				return HTTP_OK;
			}
			else
			{
				return HTTP_OVERFLOW;
			}
		}
		else
		{
			return HTTP_FAIL;
		}
	}
	else
	{
		return HTTP_NOT_FRAMEHEAD;
	}
}

