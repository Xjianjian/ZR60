/******************************************************
文件名：	Json

描述：		
Data			Vasion			author
2017/7/22		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"JsonIf.h"
//#include	"Include.h"
/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/



/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/



/******************************************************
description： function code
******************************************************/
//static uint8 HexToChar(uint8 temp);


/******************************************************
*函数名：Json_HexToJson

*形  参：

*返回值：

*描  述：转换为Json数据格式

*备  注：
******************************************************/
void Json_HexToJson(void *Le_u_in,uint16* Len,unsigned char type, char *Le_u_out)
{
    uint16  Le_w_i;
	char *buf = NULL;
	//创建一个空的文档（对象）
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
		{	//{"token":"token","type":"黑白名单标志","timeStamp":"时间戳","page":"页码","pageNum":"页大小"}
			cJSON_AddStringToObject(json,"token",((tcp_client_BListPullStruct*)Le_u_in)->token);
			cJSON_AddNumberToObject(json,"type",((tcp_client_BListPullStruct*)Le_u_in)->Listtype);
			cJSON_AddNumberToObject(json,"timeStamp",((tcp_client_BListPullStruct*)Le_u_in)->timestamp);
			cJSON_AddNumberToObject(json,"page",((tcp_client_BListPullStruct*)Le_u_in)->page);
			cJSON_AddNumberToObject(json,"pageNum",((tcp_client_BListPullStruct*)Le_u_in)->pageSize);
		}
		break;
		case JSON_REPORT_DOORST:
		{	//{"token":"token","state":"门锁状态"}
			cJSON_AddStringToObject(json,"token",((tcp_client_doorStStruct*)Le_u_in)->token);
			cJSON_AddNumberToObject(json,"state",((tcp_client_doorStStruct*)Le_u_in)->state);
		}	
		break;
		
		case JSON_REPORT_UNLOCKLOG:
		{	//{"token":"token","deviceSys":"设备系统","log":[{"openType":"开锁方式","openInfo":"开锁信息","openTime":"开锁时间"}]}
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
		{//{"doorId":"门id","mac":"MAC地址","isWifi":"mac是否是无线的MAC地址","bluetooth":"蓝牙地址"}
			cJSON_AddStringToObject(json,"doorId",&(((tcp_client_DeviceInitStruct*)Le_u_in)->doorID[0U]));
			cJSON_AddStringToObject(json,"mac",&(((tcp_client_DeviceInitStruct*)Le_u_in)->mac[0U]));
			cJSON_AddNumberToObject(json,"isWifi",((tcp_client_DeviceInitStruct*)Le_u_in)->addrtype);
			cJSON_AddStringToObject(json,"bluetooth",&(((tcp_client_DeviceInitStruct*)Le_u_in)->Blemac[0U]));
		}
		break;
		#endif
		case JSON_DEVICE_ECHO_REMOTEUNLOCK:
		{//{"t":-5,"mid":"消息id"}
			cJSON_AddNumberToObject(json,"t",-5);
			cJSON_AddNumberToObject(json,"mid",((cJSON *)Le_u_in)->valuedouble);
		}
		break;	
		default:
		break;
	}
	//将json结构格式化到缓冲区
	buf = cJSON_PrintUnformatted(json);
	if(buf !=NULL)
	{
		*Len = strlen(buf);
		for(Le_w_i = 0U;Le_w_i < (*Len);Le_w_i++)
		{
			Le_u_out[Le_w_i] = buf[Le_w_i];
		}
		Le_u_out[Le_w_i++]= '\n';//字符串末尾加\n，服务器根据\n判断一帧完整数据
		*Len = Le_w_i;
		free(buf);
	}
	//释放json结构所占用的内存
	cJSON_Delete(json);
}


/* 
// C prototype : void HexToStr(char *pszDest, byte *pbSrc, int nLen) 
// parameter(s): [OUT] pszDest - 存放目标字符串 
//  [IN] pbSrc - 输入16进制数的起始地址 
//  [IN] nLen - 16进制数的字节数 
// return value: 
// remarks : 将16进制数转化为字符串 
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

