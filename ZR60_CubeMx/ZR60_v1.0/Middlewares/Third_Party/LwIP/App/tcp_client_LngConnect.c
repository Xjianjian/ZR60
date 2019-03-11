/**
  ******************************************************************************
  * @file    tcp_client.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   tcp client application using LwIP RAW API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "tcp_client_LngConnect.h"


#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static char  LngRecev_buf[350] = {0};
struct ip_addr LngConnect_DestIPaddr;
tcp_client_Struct Setcp_Alias;
static char   Setcp_client_u_TxBuf[60] = {0};
static uint16_t se_w_lng;

u8_t  volatile Setcp_client_u_Open = 0U;//远程开锁标志
u8_t  volatile Setcp_client_u_wxOpen = 0U;//微信开锁标志
static u8_t  Setcp_u_WxRxSuccess = 0U;//微信正常接收到消息标志

static u8_t  Setcp_client_u_cnntSt = 0U;//tcp连接状态:0--空闲
static u8_t  Setcp_client_u_HeartFlg = 0U;//心跳发送完成标志
static u16_t Setcp_client_w_DelayTimer = 0U;//开机延时连接计时器
static u16_t Setcp_client_w_HeartTimer =  (28000U/LNGCNNT_SCHEDULING_CYCLE);//心跳计时器
static u16_t Setcp_client_w_CnntTimeout = 0U;//连接超时计时器
static u8_t  Se_u_CnntTimeoutFlg = 0U;//连接超时标志
static u8_t  Se_u_CnntTimeoutCnt = 0U;//连接超时计数

static char   Setcp_client_u_PhoneNo[12] = {0};
static char   Setcp_client_u_url[200] = {0};

const char ENCODE_TABLE[64U] = {'A', 'b', 'C', 'd', 'I', 'f', 'G', 'h', 'E', 'j', \
						   'K', 'l', 'M', 'a', 'B', 'c', 'D', 'e', 'F', 'g', \
						   'H', 'i', 'J', 'k', 'L', 'm', 'P', 'o', 'N', 'q', \
						   'R', 's', 'T', 'u', 'X', 'w', 'V', 'y', 'Z', 'n', \
						   'O', 'p', 'Q', 'r', 'S', 't', 'U', 'v', 'W', 'x', \
						   'Y', 'z', '9', '1', '2', '4', '3', '5', '6', '8', \
						   '7', '0', '-', '_'};
/* Private function prototypes -----------------------------------------------*/
static uint8 tcp_LngConnect_parseJson(char * pMsg);
#ifdef NET_DEBUG
static char tcp_LngConnect_HexToChar(uint8_t temp);
static void tcp_LngConnect_StrToHex(char* Le_in, uint8_t* Le_out);
#endif
static char getByte(char* nums, uint8_t index);
/* Private functions ---------------------------------------------------------*/

void tcp_LngConnect_Parameter(void)
{
	uint8_t Le_u_i;
	unsigned char La_u_ID[16U] = {0};
	char La_u_suffix[4U] = {'\0'};
	unsigned short Le_w_suffix;
	memset(Setcp_Alias.Alias,0,sizeof(Setcp_Alias.Alias));//清0
	if(1U == CLIENT_LNGCNNT_DEVICETYPE)//围墙机
	{//别名：小区id_3_下标
		GetIcUnlock_communityID(La_u_ID);
		Json_HexToStr(Setcp_Alias.Alias,La_u_ID,16);
		Setcp_Alias.Alias[32U] =  '_';
		Setcp_Alias.Alias[33U] =  '3';
		Setcp_Alias.Alias[34U] =  '_';
	}
	else//门口机
	{//别名：楼栋id_1_下标
		GetIcUnlock_buildID(La_u_ID);
		Json_HexToStr(Setcp_Alias.Alias,La_u_ID,16);
		Setcp_Alias.Alias[32U] =  '_';
		Setcp_Alias.Alias[33U] =  '1';
		Setcp_Alias.Alias[34U] =  '_';
	}
	Le_w_suffix = GetIcUnlock_w_suffix();
	if(Le_w_suffix > 999)
	{
		Le_w_suffix = 999;
	}
	sprintf(La_u_suffix,"%d",Le_w_suffix);
	strcat(Setcp_Alias.Alias, La_u_suffix);
	LNGCONN_PRINTF_S(Setcp_Alias.Alias);
	for(Le_u_i = 0U;Le_u_i < 39U;Le_u_i++)
	{//大写字母转小写
		if((Setcp_Alias.Alias[Le_u_i] >= 'A') && (Setcp_Alias.Alias[Le_u_i] <= 'Z'))
		{
			Setcp_Alias.Alias[Le_u_i] += 0x20;
		}
	}
	Setcp_Alias.lng = sizeof(Setcp_Alias.Alias);
	memset(Setcp_client_u_TxBuf,0,sizeof(Setcp_client_u_TxBuf));//清0
	Json_HexToJson(Setcp_Alias.Alias,&se_w_lng,JSON_HEART_BEAT,Setcp_client_u_TxBuf);//初始化心跳包
	NetConnIf_rxConfig(TcpLngConnect,LngRecev_buf,350,tcp_client_LngConnect_callback);
}

/******************************************************
*函数名：tcp_LngConnect_MainFunction

*形  参：

*返回值：

*描  述：消息记主任务,5ms周期调用

*备  注：
******************************************************/
void tcp_LngConnect_MainFunction(void)
{
#ifdef ZR50
	return;
#endif
	//u16_t Le_w_i;	
	if(0 == LNGCONN_GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		Setcp_client_w_CnntTimeout = 0U;
		Setcp_client_u_HeartFlg = 0U;
		Setcp_client_w_DelayTimer = 0u;
		Setcp_client_u_cnntSt = CLIENT_LNGCNNT_IDLE;
		Setcp_client_w_HeartTimer =  (28000U/LNGCNNT_SCHEDULING_CYCLE);
		NetConnIf_disconnect(TcpLngConnect);
		return;
	}
	
	if(Setcp_client_w_HeartTimer >= CLIENT_LNGCNNT_PERIOD)//每33s心跳1次
	{
		Setcp_client_w_HeartTimer = 0U;
		Setcp_client_u_HeartFlg = 1U;
	}
	else
	{
		Setcp_client_w_HeartTimer++;
	}
	
	switch(Setcp_client_u_cnntSt)
	{
		case CLIENT_LNGCNNT_IDLE:
		{
			if(Setcp_client_w_DelayTimer >= (5000/LNGCNNT_SCHEDULING_CYCLE))
			{
				Setcp_client_w_DelayTimer = 0U;
				if(CLIENT_LNGCNNT_DHCP_STATE == DHCP_ADDRESS_ASSIGNED)
				{			
					if(1U != GetdnsAnalysis_u_ipValidity(LngConnect))
					{
						return;
					}
					GetdnsAnalysis_ipAddr(LngConnect,&LngConnect_DestIPaddr);
					//IP4_ADDR(&LngConnect_DestIPaddr, 10, 0, 0, 215);
					//IP4_ADDR(&LngConnect_DestIPaddr, 10, 0, 0, 16);
					if(ERR_OK == NetConnIf_Connect(TcpLngConnect,&LngConnect_DestIPaddr,9800))
					{
						LNGCONN_PRINTF_S("建立长连接...");
						Setcp_client_w_CnntTimeout = 0U;
						Setcp_client_w_DelayTimer = 0U;
						Setcp_client_u_cnntSt = CLIENT_LNGCNNT_CNNT;
					}
				}
			}
			else
			{
				Setcp_client_w_DelayTimer++;
			}
		}
		break;
		case CLIENT_LNGCNNT_CNNT:
		{		
			Setcp_client_w_CnntTimeout++;
			if(Setcp_client_w_CnntTimeout >= CLIENT_LNGCNNT_REPLY_TIMEOUT)
			{//回应超时，75s
				LNGCONN_PRINTF_S("\r\nErrorLogging：长连接回应超时\r\n");
				Se_u_CnntTimeoutFlg = 1U;//连接超时标志
				SetdnsAnalysis_ipUpdate(LngConnect);//更新ip
				Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
				return;
			}
			
			if(1U == NetConnIf_ConnectStatus(TcpLngConnect))
			{
				Setcp_client_w_DelayTimer = 0U;
				Se_u_CnntTimeoutFlg = 0U;//连接超时标志
				if((1U == Setcp_client_u_HeartFlg) && (GetLngCnnt_PerformCondition))
				{//发送心跳==注册别名
					Setcp_client_u_HeartFlg = 0U;
					NetConnIf_sendMsg(TcpLngConnect,Setcp_client_u_TxBuf,se_w_lng);					
					LNGCONN_PRINTF_S("心跳包");
					LNGCONN_PRINTF_S(Setcp_client_u_TxBuf);
				}
			}
			else//延时等待一段时间
			{
				if(NetConnIf_RemoteConnectClose(TcpLngConnect) == 1)
				{//远端关闭连接
					Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
					return;
				}
				
				Setcp_client_w_DelayTimer++;
				if(Setcp_client_w_DelayTimer >= LNGCNNT_ESTABLISHED_TIMEOUT)
				{
					LNGCONN_PRINTF_S("\r\nErrorLogging：建立长连接超时\r\n");
					Se_u_CnntTimeoutCnt++;
					if(Se_u_CnntTimeoutCnt >= 3U)
					{
						Se_u_CnntTimeoutCnt = 0U;
						Se_u_CnntTimeoutFlg = 1U;//连接超时标志
						SetdnsAnalysis_ipUpdate(LngConnect);//更新ip
					}
					Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
				}
			}
		}
		break;
		case CLIENT_LNGCNNT_DISCNNT:
		{
			NetConnIf_disconnect(TcpLngConnect);
			LNGCONN_PRINTF_S("长连接断开...");
			Setcp_client_w_CnntTimeout = 0U;
			Setcp_client_u_HeartFlg = 0U;
			Setcp_client_w_DelayTimer = 0U;
			Setcp_client_u_cnntSt = CLIENT_LNGCNNT_IDLE;
		}
		break;
		default:
		break;
	}
}


/*
	长连接接收报文解析
*/
static uint8 tcp_LngConnect_parseJson(char * pMsg)
{
	char La_u_TxMsg[35U] = {0};
	uint16_t Le_u_Lng = 0U;
	char *position;
	cJSON * pJson = cJSON_Parse(pMsg);
	if(NULL == pJson)                                                                                         
	{
		// parse faild, return
		LNGCONN_PRINTF_S("\r\nErrorLogging：接收数据解析成json格式失败\r\n");
		LNGCONN_PRINTF_S(pMsg);
		return 0U;
	}
	// get string from json
	cJSON * pSub_t = cJSON_GetObjectItem(pJson, "t");
	if(NULL == pSub_t)
	{
		//get object named "t" faild
		LNGCONN_PRINTF_S("\r\nErrorLogging：获取成员 t 失败\r\n");
		LNGCONN_PRINTF_S(pMsg);
		cJSON_Delete(pJson);
		return 0U;
	}
	if(pSub_t->type == cJSON_String)
	{
		pSub_t->valueint = (int)(Json_DecNumber(&(pSub_t->valuestring[0])));
	}

	switch(pSub_t->valueint)
	{
		case 0:
		{
			LNGCONN_PRINTF_S("\r\nErrorLogging：别名错误\r\n");
			LNGCONN_PRINTF_S(pMsg);
		}
		break;
		case (-1):
		{
			LNGCONN_PRINTF_S("心跳正常");
			LNGCONN_PRINTF_S(pMsg);
		}
		break;
		case 5://远程开门
		{
			LNGCONN_PRINTF_S("长连接接收推送数据帧：");
			LNGCONN_PRINTF_S(pMsg);
			cJSON * pSubMsg = cJSON_GetObjectItem(pJson, "msg");
			if(NULL == pSubMsg)
			{
				cJSON_Delete(pJson);
				return 0U;
			}

			uint16 Le_w_i;
			uint16 Le_w_len;
			Le_w_len = strlen(pSubMsg->valuestring); 
			for(Le_w_i = 0U;Le_w_i < Le_w_len;Le_w_i++)//msg字符串非标准JSon格式，转化成JSon格式的字符串
			{
				if(pSubMsg->valuestring[Le_w_i] == '\'')
				{
					pSubMsg->valuestring[Le_w_i] = '"';
				}
			}
			cJSON * pJsonMsg = cJSON_Parse(pSubMsg->valuestring);//对msg字符串解析成json格式
			if(NULL == pJsonMsg)                                                                                         
			{
				// parse faild, return
				cJSON_Delete(pJson);
				return 0U;
			}

			cJSON * pSub_ex = cJSON_GetObjectItem(pJsonMsg, "ex");
			if(NULL == pSub_ex)
			{
				cJSON_Delete(pJsonMsg);
				cJSON_Delete(pJson);
				return 0U;
			}
			
			if(0 == strcmp("open", cJSON_GetObjectItem(pSub_ex,"m")->valuestring))
			{//接收到远程开锁请求
				for(Le_w_i = 0U;Le_w_i < 11U;Le_w_i++)
				{
					Setcp_client_u_PhoneNo[Le_w_i] = cJSON_GetObjectItem(pSub_ex,"p")->valuestring[Le_w_i];
				}
				Setcp_client_u_Open = 1U;//远程开锁标志
				cJSON * pSubMid = cJSON_GetObjectItem(pJson, "mid");
				Json_HexToJson(pSubMid,&Le_u_Lng,JSON_DEVICE_ECHO_REMOTEUNLOCK,La_u_TxMsg);
				NetConnIf_sendMsg(TcpLngConnect,La_u_TxMsg,Le_u_Lng);
				LNGCONN_PRINTF_S("远程开门应答：");
				LNGCONN_PRINTF_S(La_u_TxMsg);
			}
			else if((0 == strcmp("thirdopen", cJSON_GetObjectItem(pSub_ex,"m")->valuestring)) && \
				(0 == strcmp("door", cJSON_GetObjectItem(pSub_ex,"a")->valuestring)))
			{//微信公众号开门	
				Setcp_client_u_wxOpen = 1U;//微信开锁标志
				
				cJSON * pJsonObj = cJSON_GetObjectItem(pSub_ex, "p");
				cJSON * urlStr = cJSON_GetObjectItem(pJsonObj,"url");
				LNGCONN_PRINTF_S(urlStr->valuestring);
				
				position = strchr(urlStr->valuestring,'=');//=字符数值的起始位置
				if(NULL != position)
				{
					Le_u_Lng = strlen(urlStr->valuestring) -(position - urlStr->valuestring + 1);
					if(Le_u_Lng < 200)
					{
						memset(Setcp_client_u_url,0,200);//清0
						for(Le_w_i = 0;Le_w_i < Le_u_Lng;Le_w_i++)
						{
							Setcp_client_u_url[Le_w_i] = urlStr->valuestring[Le_w_i + position - urlStr->valuestring + 1];
						}
						LNGCONN_PRINTF_S(Setcp_client_u_url);
						Setcp_u_WxRxSuccess = 1U;
						cJSON * pSubMid = cJSON_GetObjectItem(pJson, "mid");
						Json_HexToJson(pSubMid,&Le_u_Lng,JSON_DEVICE_ECHO_REMOTEUNLOCK,La_u_TxMsg);
						NetConnIf_sendMsg(TcpLngConnect,La_u_TxMsg,Le_u_Lng);
						LNGCONN_PRINTF_S("微信开门应答：");
						LNGCONN_PRINTF_S(La_u_TxMsg);
					}
					else//数据长度溢出
					{
						LNGCONN_PRINTF_S("微信推送数据长度溢出\n");
						LNGCONN_PRINTF_S(urlStr->valuestring);
						Setcp_u_WxRxSuccess = 0U;
					}
				}
				else
				{
					LNGCONN_PRINTF_S("微信推送开门数据有误\n");
					LNGCONN_PRINTF_S(urlStr->valuestring);
					Setcp_u_WxRxSuccess = 0U;
				}
			}
			else if((0 == strcmp("updatedoorcard", cJSON_GetObjectItem(pSub_ex,"m")->valuestring)) && \
				   (0 == strcmp("doorcard", cJSON_GetObjectItem(pSub_ex,"a")->valuestring)))
			{
				LNGCONN_PRINTF_S("推送更新黑明单\n");
				//tcp_client_BListUpdata();
			}
			else
			{}
			
			cJSON_Delete(pJsonMsg);
		}
		break;
#ifdef NET_DEBUG
		case 24://测试hym8563时钟
		{//{"t":24,"time":""}
			cJSON * pSubTime = cJSON_GetObjectItem(pJson, "time");
			if(NULL == pSubTime)
			{
				cJSON_Delete(pJson);
				return 0U;
			}
			struct rtc_time  Le_h_tm;
			timestamp_timeCalibration(&pSubTime->valuestring[0],0U);//校准时钟芯片时间
			(void)hym8563_read_datetime(&Le_h_tm);//读取hym8563日期时间		
			//Le_h_tm.tm_year,Le_h_tm.tm_mon,Le_h_tm.tm_mday,Le_h_tm.tm_hour,Le_h_tm.tm_min,Le_h_tm.tm_sec
			Setcp_client_u_TxBuf[0] = 't';
			Setcp_client_u_TxBuf[1] = ':';
			Setcp_client_u_TxBuf[2] = '-';
			Setcp_client_u_TxBuf[3] = '2';
			Setcp_client_u_TxBuf[4] = '4';
			Setcp_client_u_TxBuf[5] = ',';
			Setcp_client_u_TxBuf[6] = 't';
			Setcp_client_u_TxBuf[7] = 'i';
			Setcp_client_u_TxBuf[8] = 'm';
			Setcp_client_u_TxBuf[9] = 'e';
			Setcp_client_u_TxBuf[10] = ':';
			Setcp_client_u_TxBuf[11] = tcp_LngConnect_HexToChar(Le_h_tm.tm_year/1000%10);
			Setcp_client_u_TxBuf[12] = tcp_LngConnect_HexToChar(Le_h_tm.tm_year/100%10);
			Setcp_client_u_TxBuf[13] = tcp_LngConnect_HexToChar(Le_h_tm.tm_year/10%10);
			Setcp_client_u_TxBuf[14] = tcp_LngConnect_HexToChar(Le_h_tm.tm_year%10);
			
			Setcp_client_u_TxBuf[15] = tcp_LngConnect_HexToChar(Le_h_tm.tm_mon/10%10);
			Setcp_client_u_TxBuf[16] = tcp_LngConnect_HexToChar(Le_h_tm.tm_mon%10);
			
			Setcp_client_u_TxBuf[17] = tcp_LngConnect_HexToChar(Le_h_tm.tm_mday/10%10);
			Setcp_client_u_TxBuf[18] = tcp_LngConnect_HexToChar(Le_h_tm.tm_mday%10);
			
			Setcp_client_u_TxBuf[19] = tcp_LngConnect_HexToChar(Le_h_tm.tm_hour/10%10);
			Setcp_client_u_TxBuf[20] = tcp_LngConnect_HexToChar(Le_h_tm.tm_hour%10);
			
			Setcp_client_u_TxBuf[21] = tcp_LngConnect_HexToChar(Le_h_tm.tm_min/10%10);
			Setcp_client_u_TxBuf[22] = tcp_LngConnect_HexToChar(Le_h_tm.tm_min%10);
			
			Setcp_client_u_TxBuf[23] = tcp_LngConnect_HexToChar(Le_h_tm.tm_sec/10%10);
			Setcp_client_u_TxBuf[24] = tcp_LngConnect_HexToChar(Le_h_tm.tm_sec%10);
			tcp_LngConnect_sendMsg(Setcp_client_u_TxBuf,25);	
		}
		break;
		case 25://测试黑名单
		{//{"t":25,"BList":["卡号1","卡号2",...]}
			cJSON * pSubBList = cJSON_GetObjectItem(pJson, "BList");
			if(NULL == pSubBList)
			{
				cJSON_Delete(pJson);
				return 0U;
			}
			
			unsigned int BList_size = cJSON_GetArraySize(pSubBList);
			LNGCONN_PRINTF_D("：黑名单卡号数量%d\n",BList_size);
			unsigned int BList_i = 0;
			uint8_t BList_cardNum[4U];
			cJSON *BList_item;
			for(BList_i=0; BList_i< BList_size; BList_i++) 
			{
				BList_item = cJSON_GetArrayItem(pSubBList, BList_i);
				if(strlen(BList_item->valuestring) != 8U)
				{
					LNGCONN_PRINTF_D("卡号编号 %s长度不符\n：",BList_item->valuestring);
					continue;
				}
				tcp_LngConnect_StrToHex(BList_item->valuestring,BList_cardNum);
				WrBListCache_BListQueue(BList_cardNum);
				LNGCONN_PRINTF_D("卡号编号 %d 转换为Hex格式卡号：",BList_i);
				LNGCONN_PRINTF_CARD_NUM("%x%x%x%x",BList_cardNum[0U],BList_cardNum[1U],BList_cardNum[2U],BList_cardNum[3U]);
			}
			tcp_LngConnect_sendMsg("t:-25",strlen("t:-25"));
		}
		break;
#endif
		default:
		break;
	}
	cJSON_Delete(pJson);
	return 1U;
}

#ifdef NET_DEBUG
/*
	十六进制0~F转为字符'0'~'F'
*/
static char tcp_LngConnect_HexToChar(uint8_t temp)
{
    char dst;
    if (temp < 10)
	{
        dst = temp + '0';
    }
	else
	{
        dst = temp -10 +'A';
    }
    return dst;
}


/*
	字符串转Hex格式:AABBCCDD转为0xAA,0xBB,0xCC,0xDD
*/
static void tcp_LngConnect_StrToHex(char* Le_in, uint8_t* Le_out)
{
	uint8_t Le_u_i;
	for(Le_u_i = 0U;Le_u_i < 4U;Le_u_i++)
	{
		if((Le_in[2*Le_u_i] >= '0') && (Le_in[2*Le_u_i] <= '9'))
		{
			Le_out[Le_u_i] = ((Le_in[2*Le_u_i]-0x30) << 4U);
		}
		else
		{
			Le_out[Le_u_i] = ((Le_in[2*Le_u_i]-0x37) << 4U);
		}
		
		if((Le_in[2*Le_u_i+1] >= '0') && (Le_in[2*Le_u_i+1] <= '9'))
		{
			Le_out[Le_u_i] |= ((Le_in[2*Le_u_i + 1] -0x30) & 0x0f);
		}
		else
		{
			Le_out[Le_u_i] |= ((Le_in[2*Le_u_i + 1] -0x37) & 0x0f);
		}
	}
}
#endif

/*
	获取长连接超时状态
*/
uint8 Gettcp_u_LngConnect_Timeout(void)
{
	return Se_u_CnntTimeoutFlg;
}

/*
	数据接收回调函数
*/
void tcp_client_LngConnect_callback(void)
{
	if(1U == tcp_LngConnect_parseJson(LngRecev_buf))
	{
		Setcp_client_w_CnntTimeout = 0U;
	}
}

/*
	读取电话号码
*/
void Gettcp_LngConnect_PhoneNo(char* Le_u_dt)
{
	uint8_t Le_u_i;
	for(Le_u_i = 0U;Le_u_i < 12U;Le_u_i++)
	{
		Le_u_dt[Le_u_i] = Setcp_client_u_PhoneNo[Le_u_i];
	}
}

/*
描述：读取微信开门信息
解码原理：
     * 1 将已编码字符串encode逐个分离得到bChar
     * 2 将每个bChar参照表转换成对应的索引num
     * 3 将除最后一个num的每个索引num去掉前2位并组合成一个位串preBits
     * 4 位串preBits的长度模8得到值len
     * 5 再将位串preBits与最后一个num的后8减len的差位组合成新的位串bits
     * 6 将位串bits每次取8位得到字节数组bytes
     * 7 将字节数组bytes转成UTF-8的字符串
     * <p>
     * 如 encode是MgEzaA
     * 1 将已编码字符串encode逐个分离得到bChar是[M,g,E,z,a,A]
     * 2 将每个bChar参照表转换成对应的索引num是[12,19,8,51,13,0]
     * 3 将除最后一个num的每个索引num去掉前2位[001100,010011,001000,110011,001101]并组合成一个位串preBits是001100010011001000110011001101
     * 4 位串preBits的长度30模8得到值len是6
     * 5 再将位串preBits与最后一个num(000000)的后8-6=2位组合成新的位串bits是00110001001100100011001100110100
     * 6 将位串bits每次取8位得到字节数组bytes[00110001,00110010,00110011,00110100]即[49,50,51,52]
     * 7 将字节数组bytes转成UTF-8的字符串是1234
*/	
char Gettcp_LngConnect_WXOpenInfo(char* Le_u_dt)
{
	uint8_t Le_u_i;
	uint8_t Le_u_j;
	uint8_t Le_u_k;
	char* position;
	char* endposition;
	char index;
	char nums[strlen(Setcp_client_u_url)];
	char bChar;
	uint8_t bytesLen;
	if(Setcp_u_WxRxSuccess == 1)
	{
		Setcp_u_WxRxSuccess = 0;
		for (Le_u_i = 0, Le_u_j = 0; Le_u_j < 200; Le_u_i++, Le_u_j++) 
		{
			bChar = Setcp_client_u_url[Le_u_i];
			for (Le_u_k = 0; Le_u_k < 64U; Le_u_k++) 
			{
				if (ENCODE_TABLE[Le_u_k] == bChar) 
				{
					nums[Le_u_j] = Le_u_k;
					break;
				}
			}
		}
		
		bytesLen = (strlen(Setcp_client_u_url) * 6.0) / 8;
		char bytes[bytesLen+1];
		for (Le_u_i = 0; Le_u_i < bytesLen; Le_u_i++) 
		{
			bytes[Le_u_i] = getByte(nums, Le_u_i);
		}
		
		LNGCONN_PRINTF_S("转换成utf-8格式前字符数据：");
		LNGCONN_PRINTF_S(bytes);
		position = strchr((char*)bytes,'&');//&字段数值的位置
		if(NULL != position)
		{
			index = position - (char*)bytes;//首次出现&位置下标
			endposition = strchr((char*)(&bytes[index+1]),'&');//&字段数值的位置;
			if(NULL != endposition)
			{
				for (Le_u_i = 0; Le_u_i < (endposition - position -1); Le_u_i++) 
				{
					Le_u_dt[Le_u_i] = bytes[index+1+ Le_u_i];
				}
				LNGCONN_PRINTF_S("微信开门用户信息：");
				LNGCONN_PRINTF_S(Le_u_dt);
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

static char getByte(char* nums, uint8_t index) 
{
	char b = 0;
	uint8_t i;
	int bytesIndex = (index * 8 / 6);
	int bitIndex = (index * 8) % 6 + 2;
	int getBits = 0;
	for (i = 0; i < strlen(nums); i++) 
	{
		if (i < bytesIndex) 
		{
			continue;
		}
		else 
		{
			short B = (short) (nums[bytesIndex] & 0x00ff);
			b = (char) (B & (0xff >> bitIndex));
			getBits = 8 - bitIndex;
			int left = 8 - getBits;
			b = (char) (b << left);
			bytesIndex++;
			short nextB = (short) (nums[bytesIndex] & 0x00ff);
			int right = 8 - (left + 2);
			b = (char) (b | (nextB >> right));
			break;
		}
	}
	return b;
}


#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
