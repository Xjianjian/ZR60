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
//#include "lwip/debug.h"
//#include "lwip/stats.h"
//#include "lwip/tcp.h"
//#include "memp.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "netconf.h"
//#include "LAN8742A.h"
#include "./NetConnIf/NetConnIf.h"
#include "tcp_client_LngConnect.h"
//#include "Include.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static char  LngRecev_buf[200] = {0};
struct ip_addr LngConnect_DestIPaddr;
tcp_client_Struct Setcp_Alias;
static char   Setcp_client_u_TxBuf[60] = {0};
static uint16_t se_w_lng;

u8_t  volatile Setcp_client_u_Open = 0U;//Զ�̿�����־

static u8_t  Setcp_client_u_cnntSt = 0U;//tcp����״̬:0--����
static u8_t  Setcp_client_u_HeartFlg = 0U;//����������ɱ�־
static u16_t Setcp_client_w_DelayTimer = 0U;//������ʱ���Ӽ�ʱ��
static u16_t Setcp_client_w_HeartTimer =  (28000U/LNGCNNT_SCHEDULING_CYCLE);//������ʱ��
static u16_t Setcp_client_w_CnntTimeout = 0U;//���ӳ�ʱ��ʱ��
static u8_t  Se_u_CnntTimeoutFlg = 0U;//���ӳ�ʱ��־
static u8_t  Se_u_CnntTimeoutCnt = 0U;//���ӳ�ʱ����

static char   Setcp_client_u_PhoneNo[12] = {0};
/* Private function prototypes -----------------------------------------------*/
static uint8 tcp_LngConnect_parseJson(char * pMsg);
static char tcp_LngConnect_HexToChar(uint8_t temp);
static void tcp_LngConnect_StrToHex(char* Le_in, uint8_t* Le_out);
/* Private functions ---------------------------------------------------------*/

void tcp_LngConnect_Parameter(void)
{
	uint8_t Le_u_i;
	char La_u_suffix[4U] = {0};
	memset(Setcp_Alias.Alias,0,sizeof(Setcp_Alias.Alias));//��0
	if(1U == CLIENT_LNGCNNT_DEVICETYPE)//Χǽ��
	{//������С��id_3_�±�
		Json_HexToStr(Setcp_Alias.Alias,sminfo1.community_id,16);
		Setcp_Alias.Alias[32U] =  '_';
		Setcp_Alias.Alias[33U] =  '3';
		Setcp_Alias.Alias[34U] =  '_';
	}
	else//�ſڻ�
	{//������¥��id_1_�±�
		Json_HexToStr(Setcp_Alias.Alias,sminfo1.build_id,16);
		Setcp_Alias.Alias[32U] =  '_';
		Setcp_Alias.Alias[33U] =  '1';
		Setcp_Alias.Alias[34U] =  '_';
	}
	
	if(sminfo1.suffix > 999)
	{
		sminfo1.suffix = 999;
	}
	sprintf(La_u_suffix,"%d",sminfo1.suffix);
	USART_PRINTF_S(La_u_suffix);
	strcat(Setcp_Alias.Alias, La_u_suffix);
	USART_PRINTF_S(Setcp_Alias.Alias);
	for(Le_u_i = 0U;Le_u_i < 39U;Le_u_i++)
	{//��д��ĸתСд
		if((Setcp_Alias.Alias[Le_u_i] >= 'A') && (Setcp_Alias.Alias[Le_u_i] <= 'Z'))
		{
			Setcp_Alias.Alias[Le_u_i] += 0x20;
		}
	}
	Setcp_Alias.lng = sizeof(Setcp_Alias.Alias);
	memset(Setcp_client_u_TxBuf,0,sizeof(Setcp_client_u_TxBuf));//��0
	Json_HexToJson(Setcp_Alias.Alias,&se_w_lng,JSON_HEART_BEAT,Setcp_client_u_TxBuf);//��ʼ��������
	NetConnIf_rxConfig(TcpLngConnect,LngRecev_buf,200,tcp_client_LngConnect_callback);
}

/******************************************************
*��������tcp_LngConnect_MainFunction

*��  �Σ�

*����ֵ��

*��  ������Ϣ��������,5ms���ڵ���

*��  ע��
******************************************************/
void tcp_LngConnect_MainFunction(void)
{
#ifdef ZR50
	return;
#endif
	//u16_t Le_w_i;	
	if(0 == GET_PHY_LINK_STATUS())/* Get Ethernet link status*/
	{
		Setcp_client_w_CnntTimeout = 0U;
		Setcp_client_u_HeartFlg = 0U;
		Setcp_client_w_DelayTimer = 0u;
		Setcp_client_u_cnntSt = CLIENT_LNGCNNT_IDLE;
		Setcp_client_w_HeartTimer =  (28000U/LNGCNNT_SCHEDULING_CYCLE);
		NetConnIf_disconnect(TcpLngConnect);
		return;
	}
	
	if(Setcp_client_w_HeartTimer >= CLIENT_LNGCNNT_PERIOD)//ÿ33s����1��
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
						USART_PRINTF_S("����������...");
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
			{//��Ӧ��ʱ��75s
				USART_PRINTF_S("\r\nErrorLogging�������ӻ�Ӧ��ʱ\r\n");
				Se_u_CnntTimeoutFlg = 1U;//���ӳ�ʱ��־
				SetdnsAnalysis_ipUpdate(LngConnect);//����ip
				Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
				return;
			}
			
			if(1U == NetConnIf_ConnectStatus(TcpLngConnect))
			{
				Setcp_client_w_DelayTimer = 0U;
				Se_u_CnntTimeoutFlg = 0U;//���ӳ�ʱ��־
				if((1U == Setcp_client_u_HeartFlg) && (GetLngCnnt_PerformCondition))
				{//��������==ע�����
					Setcp_client_u_HeartFlg = 0U;
					NetConnIf_sendMsg(TcpLngConnect,Setcp_client_u_TxBuf,se_w_lng);					
					USART_PRINTF_S("������");
					USART_PRINTF_S(Setcp_client_u_TxBuf);
				}
			}
			else//��ʱ�ȴ�һ��ʱ��
			{
				if(NetConnIf_RemoteConnectClose(TcpLngConnect) == 1)
				{//Զ�˹ر�����
					Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
					return;
				}
				
				Setcp_client_w_DelayTimer++;
				if(Setcp_client_w_DelayTimer >= LNGCNNT_ESTABLISHED_TIMEOUT)
				{
					USART_PRINTF_S("\r\nErrorLogging�����������ӳ�ʱ\r\n");
					Se_u_CnntTimeoutCnt++;
					if(Se_u_CnntTimeoutCnt >= 3U)
					{
						Se_u_CnntTimeoutCnt = 0U;
						Se_u_CnntTimeoutFlg = 1U;//���ӳ�ʱ��־
						SetdnsAnalysis_ipUpdate(LngConnect);//����ip
					}
					Setcp_client_u_cnntSt = CLIENT_LNGCNNT_DISCNNT;
				}
			}
		}
		break;
		case CLIENT_LNGCNNT_DISCNNT:
		{
			NetConnIf_disconnect(TcpLngConnect);
			USART_PRINTF_S("�����ӶϿ�...");
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
	�����ӽ��ձ��Ľ���
*/
static uint8 tcp_LngConnect_parseJson(char * pMsg)
{
	char La_u_TxMsg[35U] = {0};
	uint16_t Le_u_Lng = 0U;
	cJSON * pJson = cJSON_Parse(pMsg);
	if(NULL == pJson)                                                                                         
	{
		// parse faild, return
		USART_PRINTF_S("\r\nErrorLogging���������ݽ�����json��ʽʧ��\r\n");
		USART_PRINTF_S(pMsg);
		return 0U;
	}
	// get string from json
	cJSON * pSub_t = cJSON_GetObjectItem(pJson, "t");
	if(NULL == pSub_t)
	{
		//get object named "t" faild
		USART_PRINTF_S("\r\nErrorLogging����ȡ��Ա t ʧ��\r\n");
		USART_PRINTF_S(pMsg);
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
			USART_PRINTF_S("\r\nErrorLogging����������\r\n");
		}
		break;
		case (-1):
		{
			USART_PRINTF_S("��������");
		}
		break;
		case 5://Զ�̿���
		{
			USART_PRINTF_S("Զ�̿�������֡��");
			USART_PRINTF_S(pMsg);
			cJSON * pSubMsg = cJSON_GetObjectItem(pJson, "msg");
			if(NULL == pSubMsg)
			{
				cJSON_Delete(pJson);
				return 0U;
			}

			uint16 Le_w_i;
			uint16 Le_w_len;
			Le_w_len = strlen(pSubMsg->valuestring); 
			for(Le_w_i = 0U;Le_w_i < Le_w_len;Le_w_i++)//msg�ַ����Ǳ�׼JSon��ʽ��ת����JSon��ʽ���ַ���
			{
				if(pSubMsg->valuestring[Le_w_i] == '\'')
				{
					pSubMsg->valuestring[Le_w_i] = '"';
				}
			}
			cJSON * pJsonMsg = cJSON_Parse(pSubMsg->valuestring);//��msg�ַ���������json��ʽ
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
			{//���յ�Զ�̿�������
				for(Le_w_i = 0U;Le_w_i < 11U;Le_w_i++)
				{
					Setcp_client_u_PhoneNo[Le_w_i] = cJSON_GetObjectItem(pSub_ex,"p")->valuestring[Le_w_i];
				}
				Setcp_client_u_Open = 1U;//Զ�̿�����־
				cJSON * pSubMid = cJSON_GetObjectItem(pJson, "mid");
				Json_HexToJson(pSubMid,&Le_u_Lng,JSON_DEVICE_ECHO_REMOTEUNLOCK,La_u_TxMsg);
				NetConnIf_sendMsg(TcpLngConnect,La_u_TxMsg,Le_u_Lng);
				USART_PRINTF_S("Զ�̿���Ӧ��");
				USART_PRINTF_S(La_u_TxMsg);
			}
			else if((0 == strcmp("updatedoorcard", cJSON_GetObjectItem(pSub_ex,"m")->valuestring)) && \
				   (0 == strcmp("doorcard", cJSON_GetObjectItem(pSub_ex,"a")->valuestring)))
			{
				USART_PRINTF_S("���͸��º�����\n");
				tcp_client_BListUpdata();
			}
			else
			{}
			
			cJSON_Delete(pJsonMsg);
		}
		break;
#ifdef NET_DEBUG
		case 24://����hym8563ʱ��
		{//{"t":24,"time":""}
			cJSON * pSubTime = cJSON_GetObjectItem(pJson, "time");
			if(NULL == pSubTime)
			{
				cJSON_Delete(pJson);
				return 0U;
			}
			struct rtc_time  Le_h_tm;
			timestamp_timeCalibration(&pSubTime->valuestring[0],0U);//У׼ʱ��оƬʱ��
			(void)hym8563_read_datetime(&Le_h_tm);//��ȡhym8563����ʱ��		
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
		case 25://���Ժ�����
		{//{"t":25,"BList":["����1","����2",...]}
			cJSON * pSubBList = cJSON_GetObjectItem(pJson, "BList");
			if(NULL == pSubBList)
			{
				cJSON_Delete(pJson);
				return 0U;
			}
			
			unsigned int BList_size = cJSON_GetArraySize(pSubBList);
			USART_PRINTF_D("����������������%d\n",BList_size);
			unsigned int BList_i = 0;
			uint8_t BList_cardNum[4U];
			cJSON *BList_item;
			for(BList_i=0; BList_i< BList_size; BList_i++) 
			{
				BList_item = cJSON_GetArrayItem(pSubBList, BList_i);
				if(strlen(BList_item->valuestring) != 8U)
				{
					USART_PRINTF_D("���ű�� %s���Ȳ���\n��",BList_item->valuestring);
					continue;
				}
				tcp_LngConnect_StrToHex(BList_item->valuestring,BList_cardNum);
				WrBListCache_BListQueue(BList_cardNum);
				USART_PRINTF_D("���ű�� %d ת��ΪHex��ʽ���ţ�",BList_i);
				USART_PRINTF_CARD_NUM("%x%x%x%x",BList_cardNum[0U],BList_cardNum[1U],BList_cardNum[2U],BList_cardNum[3U]);
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

/*
	ʮ������0~FתΪ�ַ�'0'~'F'
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
	�ַ���תHex��ʽ:AABBCCDDתΪ0xAA,0xBB,0xCC,0xDD
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

/*
	��ȡ�����ӳ�ʱ״̬
*/
uint8 Gettcp_u_LngConnect_Timeout(void)
{
	return Se_u_CnntTimeoutFlg;
}

/*
	���ݽ��ջص�����
*/
void tcp_client_LngConnect_callback(void)
{
	if(1U == tcp_LngConnect_parseJson(LngRecev_buf))
	{
		Setcp_client_w_CnntTimeout = 0U;
	}
}

/*
	��ȡ�绰����
*/
void Gettcp_LngConnect_PhoneNo(char* Le_u_dt)
{
	uint8_t Le_u_i;
	for(Le_u_i = 0U;Le_u_i < 12U;Le_u_i++)
	{
		Le_u_dt[Le_u_i] = Setcp_client_u_PhoneNo[Le_u_i];
	}
}
#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
