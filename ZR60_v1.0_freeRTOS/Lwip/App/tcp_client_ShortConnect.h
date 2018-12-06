/**
  ******************************************************************************
  * @file    tcp_echoclient.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header file for tcp_echoclient.c
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_ECHOCLIENT_H__
#define __TCP_ECHOCLIENT_H__
#include "./ZR60_Ctrl/ZR60_Ctrl.h"
#include "./PeriStFliter/PSFltr.h"
#include "Include.h"
#include "STM8_FM17550_iap.h"
#include "http_client_iap.h"

//#define SHORTCNNT_HEART  //定义时，短连接需要发送心跳
#define  SHORTCNNT_EXT_NET	  //定义时使用外部服务器,未定义使用内网服务器
#define  SHORTCNNT_HTTPS		  //定义时使用https连接,未定义使用http连接


#define  SHORTCNNT_SCHEDULING_CYCLE		  5000U//调度周期

#define  CLIENT_SHORTCNNT_CARDNUM_LNG     4U
#define  CLIENT_SHORTCNNT_LNG   	      1000U//发送数据长度
#define  CLIENT_LOGRECORD_PERIOD   	  	  (15000U/SHORTCNNT_SCHEDULING_CYCLE)//开锁记录周期，15s
#define  CLIENT_DOORST_PERIOD   	  	  (30000U/SHORTCNNT_SCHEDULING_CYCLE)//门锁状态上报周期30s
#define  CLIENT_HEART_PERIOD   	  	  	  (30000U/SHORTCNNT_SCHEDULING_CYCLE)//心跳周期，30s
#define  CLIENT_LOGRECORD_NUM   	      5U//一次上报的log记录条数
#define  SHORTCNNT_PULLBLIST_PERIOD   	  (1800000U/SHORTCNNT_SCHEDULING_CYCLE)//增量拉取黑名单周期,0.5h
#define  SHORTCNNT_BLIST_PULLNUM   	  	  50//默认1次最多拉取卡号数量


#define  CLIENT_SHORTCNNT_IDLE   	      0U
#define  CLIENT_SHORTCNNT_WAIT 			  1U
#define  CLIENT_SHORTCNNT_TXPCKT 		  2U//数据发送状态
#define  CLIENT_SHORTCNNT_DISCNNT         3U

#define  CLIENT_SHORTCNNT_REMOTEPORT      443


#ifdef USE_DHCP
#define CLIENT_SHORTCNNT_DHCP_STATE    DHCP_state
#else
#define CLIENT_SHORTCNNT_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif

//宏函数定义
#define Gettcp_client_u_DoorSt       GetPSFltr_u_PeriSt(DoorLockSt)
#define Gettcp_client_u_LogAvild     GetUnlockLogCache_u_LogAvild()
#define Gettcp_client_doorLog(x)     GetUnlockLogCache_doorLog(x)
#define GetShortCnnt_PerformCondition     ((Gethttp_CnntTxSt() ==  0U) && (GetStm8_fm17550_iapIdle() == 1))


#define tcp_client_BleMacFlg    Se_u_BleMacFlg//获取蓝牙mac标志

typedef enum
{
	Pckt_Unknow = 0U,/*未知类型*/
	Pckt_Init,/*设备初始化信息*/
#ifdef SHORTCNNT_HEART
	Pckt_Heart,/*心跳*/
#endif
	Pckt_TempBuf,/*临时缓存区*/
	Pckt_DoorSt,/*门锁状态*/
	Pckt_OpenReport,/*开门记录*/
	Pckt_BList/*拉取黑名单*/
}tcp_client_TxPcktType;//当前发送的报文类型


typedef struct
{
	//char TskFlag;//建立短连接任务标志
	char InitFlag;//上报初始化信息上报任务执行标志
#ifdef SHORTCNNT_HEART
	char HeartFlag;//
#endif
	char DoorStFlag;//门锁状态上报任务执行标志
	char ReportFlag;//开门记录上报任务执行标志
	char BListFlag;//拉取黑名单任务执行标志
	//char EchoFlag;//报文响应标志：0--报文未发送；1--等待响应；2--响应成功
	//char BusyFlag;//忙标志
	//char PcktType;//当前正在发送的数据报文类型:见tcp_client_TxPcktType定义
	char tokenOverdueFlag;//token过期标志
}tcp_client_TxFlagStruct;

typedef struct
{
	char token[40];
	char Listtype;//名单类型
	uint64_t timestamp;//时间戳
	uint64_t Oldtimestamp;//旧时间戳
	uint32_t page;//页码
	uint32_t Oldpage;//旧页码
	uint16_t pageSize;//页码
	uint8_t  UpdataFlag;//更新标志：0--全量更新；1--增量更新
}tcp_client_BListPullStruct;

typedef struct
{
	char token[40];
	char state;//门锁状态
}tcp_client_doorStStruct;

typedef struct
{
	char doorID[33U];//门id,多出1字节存放'\0'
	char mac[18U];//stm32 mac地址,12个字节存放地址字符，5个字节存放'：'，多出1字节存放'\0'
	char addrtype;//地址类型信息：是无线的MAC地址，0不是无线的MAC地址
	char Blemac[18U];//蓝牙mac地址,12个字节存放地址字符，5个字节存放'：'，多出1字节存放'\0'
	char addrRecombineFlg;//地址重组标志：112233445566重组为11:22:33:44:55:66标志置位
}tcp_client_DeviceInitStruct;


typedef struct
{
	char data[CLIENT_SHORTCNNT_LNG];//临时数据缓存buf
	u16_t lng;//数据长度
	char dtAlidity;//数据有效性
	//char txflag;//临时数据发送标志
}tcp_client_TxTempBufStruct;

typedef struct
{
	char token[40];
	char LogNum;//当前上报的开门记录条数
	UnlockLogCacheStruct  Log[CLIENT_LOGRECORD_NUM];//开门记录缓存区
}tcp_client_OpenLogStruct;


typedef struct 
{
	int sec;
	int min;
	int hour;
	unsigned int  Timer;
	char flag;
	char UpdataTimeSetflag;
}tcp_client_UpdateBListStruct;

extern char Vetcp_client_u_token[40];
extern tcp_client_BListPullStruct BListPull;
extern tcp_client_DeviceInitStruct  DeviceInit;

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void tcp_ShortConnect_parameter(void);
extern void tcp_ShortConnect_MainFunction(void);
extern u8_t Gettcp_shortCnntTxSt(void);
extern uint8_t tcp_client_BListUpdataSt(void);
extern void Settcp_client_DeviceInit(void);
#endif /*  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
