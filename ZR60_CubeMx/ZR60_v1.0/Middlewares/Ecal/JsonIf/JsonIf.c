/******************************************************
�ļ�����	Json

������		
Data			Vasion			author
2017/7/22		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"JsonIf.h"
//#include	"Include.h"
/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/



/******************************************************
description�� function code
******************************************************/
//static uint8 HexToChar(uint8 temp);


/******************************************************
*��������Json_HexToJson

*��  �Σ�

*����ֵ��

*��  ����ת��ΪJson���ݸ�ʽ

*��  ע��
******************************************************/
void Json_HexToJson(void *Le_u_in,uint16* Len,unsigned char type, char *Le_u_out)
{
    uint16  Le_w_i;
	char *buf = NULL;
	//����һ���յ��ĵ�������
	cJSON *json = cJSON_CreateObject();
	switch(type)
	{
		case JSON_HEART_BEAT:
		{
			cJSON_AddNumberToObject(json,"t",1);
			cJSON_AddStringToObject(json,"alias",(char*)Le_u_in);
		}
		break;
	#if 0	
		case JSON_GET_BLIST:
		{	//{"token":"token","type":"�ڰ�������־","timeStamp":"ʱ���","page":"ҳ��","pageNum":"ҳ��С"}
			cJSON_AddStringToObject(json,"token",((tcp_client_BListPullStruct*)Le_u_in)->token);
			cJSON_AddNumberToObject(json,"type",((tcp_client_BListPullStruct*)Le_u_in)->Listtype);
			cJSON_AddNumberToObject(json,"timeStamp",((tcp_client_BListPullStruct*)Le_u_in)->timestamp);
			cJSON_AddNumberToObject(json,"page",((tcp_client_BListPullStruct*)Le_u_in)->page);
			cJSON_AddNumberToObject(json,"pageNum",((tcp_client_BListPullStruct*)Le_u_in)->pageSize);
		}
		break;
		case JSON_REPORT_DOORST:
		{	//{"token":"token","state":"����״̬"}
			cJSON_AddStringToObject(json,"token",((tcp_client_doorStStruct*)Le_u_in)->token);
			cJSON_AddNumberToObject(json,"state",((tcp_client_doorStStruct*)Le_u_in)->state);
		}	
		break;
		
		case JSON_REPORT_UNLOCKLOG:
		{	//{"token":"token","deviceSys":"�豸ϵͳ","log":[{"openType":"������ʽ","openInfo":"������Ϣ","openTime":"����ʱ��"}]}
			cJSON *log_Array = NULL;
			cJSON *Arrayobj = NULL;
			cJSON_AddStringToObject(json,"token",((tcp_client_OpenLogStruct*)Le_u_in)->token);
			cJSON_AddStringToObject(json,"deviceSys",CeZR60Ctrl_u_SoftVersion);
			cJSON_AddItemToObject(json,"log", log_Array = cJSON_CreateArray());  
			for(Le_w_i = 0U;Le_w_i < (((tcp_client_OpenLogStruct*)Le_u_in)->LogNum);Le_w_i++)
			{
				cJSON_AddItemToArray(log_Array,Arrayobj=cJSON_CreateObject());
				cJSON_AddNumberToObject(Arrayobj,"openType",((tcp_client_OpenLogStruct*)Le_u_in)->Log[Le_w_i].e_u_Event);
				cJSON_AddStringToObject(Arrayobj,"openInfo",&(((tcp_client_OpenLogStruct*)Le_u_in)->Log[Le_w_i].e_u_ID[0U]));
				cJSON_AddNumberToObject(Arrayobj,"openTime",((tcp_client_OpenLogStruct*)Le_u_in)->Log[Le_w_i].e_u_Time);
			}
		}	
		break;
		
		case JSON_DEVICE_INIT:
		{//{"doorId":"��id","mac":"MAC��ַ","isWifi":"mac�Ƿ������ߵ�MAC��ַ","bluetooth":"������ַ"}
			cJSON_AddStringToObject(json,"doorId",&(((tcp_client_DeviceInitStruct*)Le_u_in)->doorID[0U]));
			cJSON_AddStringToObject(json,"mac",&(((tcp_client_DeviceInitStruct*)Le_u_in)->mac[0U]));
			cJSON_AddNumberToObject(json,"isWifi",((tcp_client_DeviceInitStruct*)Le_u_in)->addrtype);
			cJSON_AddStringToObject(json,"bluetooth",&(((tcp_client_DeviceInitStruct*)Le_u_in)->Blemac[0U]));
		}
		break;
		#endif
		case JSON_DEVICE_ECHO_REMOTEUNLOCK:
		{//{"t":-5,"mid":"��Ϣid"}
			cJSON_AddNumberToObject(json,"t",-5);
			cJSON_AddNumberToObject(json,"mid",((cJSON *)Le_u_in)->valuedouble);
		}
		break;	
		default:
		break;
	}
	//��json�ṹ��ʽ����������
	buf = cJSON_PrintUnformatted(json);
	if(buf !=NULL)
	{
		*Len = strlen(buf);
		for(Le_w_i = 0U;Le_w_i < (*Len);Le_w_i++)
		{
			Le_u_out[Le_w_i] = buf[Le_w_i];
		}
		Le_u_out[Le_w_i++]= '\n';//�ַ���ĩβ��\n������������\n�ж�һ֡��������
		*Len = Le_w_i;
		free(buf);
	}
	//�ͷ�json�ṹ��ռ�õ��ڴ�
	cJSON_Delete(json);
}


/* 
// C prototype : void HexToStr(char *pszDest, byte *pbSrc, int nLen) 
// parameter(s): [OUT] pszDest - ���Ŀ���ַ��� 
//  [IN] pbSrc - ����16����������ʼ��ַ 
//  [IN] nLen - 16���������ֽ��� 
// return value: 
// remarks : ��16������ת��Ϊ�ַ��� 
*/  
void Json_HexToStr(char *pszDest, unsigned char *pbSrc, int nLen)  
{  
    char    ddl, ddh;  
    for (int i = 0; i < nLen; i++)  
    {  
        ddh = 48 + pbSrc[i] / 16;  
        ddl = 48 + pbSrc[i] % 16;  
        if (ddh > 57) ddh = ddh + 7;  
        if (ddl > 57) ddl = ddl + 7;  
        pszDest[i * 2] = ddh;  
        pszDest[i * 2 + 1] = ddl;  
    }  
  
    pszDest[nLen * 2] = '\0';  
}

uint32 Json_DecNumber(const char* str)
{
    uint32 value;
	
    if (! str)
    {
        return 0;
    }
    value = 0;
    while ((*str >= '0') && (*str <= '9'))
    {
        value = value*10 + (*str - '0');
        str++;
    }
    return value;
}

