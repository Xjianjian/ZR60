/**
  ******************************************************************************
  * @file    http_client_iap.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header file for http_client_iap.c
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
#ifndef __HTTP_CLIENT_IAP_H__
#define __HTTP_CLIENT_IAP_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyType.h"
#include "stm32f4xx_flash.h"
#include "dnsAnalysis.h"
/*Lwip库文件*/
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netconf.h"
#include "lwip/pbuf.h"
//#include "lwipopts.h"
//#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/ip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "netconf.h"
#include "LAN8742A.h"
#include "memp.h"
#include "tcp_client_ShortConnect.h"

#define  IAP_SCHEDULING_CYCLE     50U//调度周期
#define  CLIENT_CNNT_LNG   	      200U//发送数据长度

#define  CLIENT_CNNT_IDLE   	     0U
#define  CLIENT_CNNT_WAIT 			 1U
#define  CLIENT_CNNT_LOADPCKT 		 2U//数据下载
#define  CLIENT_CNNT_DISCNNT         3U

#define  DOWNLOAD_IDLE   	     0U
#define  DOWNLOAD_UNDERWAY 		 1U
#define  DOWNLOAD_FINISH 		 2U//数据下载完成
#define  DOWNLOAD_FAIL 		 3U//数据下载失败

#define  CLIENT_CNNT_REMOTEPORT     80


#ifdef USE_DHCP
#define CLIENT_CNNT_DHCP_STATE    DHCP_state
#else
#define CLIENT_CNNT_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif

/* Base address of the STM32 Flash sectors 以下是stm32的片上flash扇区基地址，不可修改*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define HTTP_CLIENT_UPLOAD_INFO_SEC ADDR_FLASH_SECTOR_2//升级信息存放的扇区基地址
#define HTTP_CLIENT_USERCODE_SEC1 ADDR_FLASH_SECTOR_9//用户下载代码存放的扇区基地址
#define HTTP_CLIENT_USERCODE_SEC2 ADDR_FLASH_SECTOR_10//用户下载代码存放的扇区基地址
#define HTTP_CLIENT_USERCODE_SEC3 ADDR_FLASH_SECTOR_11//用户下载代码存放的扇区基地址


#define  CLIENT_DOWNLOAD_DATA_LNG   	      (128*3*1024)//下载区大小（字节数）

//宏函数定义
#define http_Client_PerformCondition ((0U == Gettcp_shortCnntTxSt()) && (GetAudioIO_u_PlaySt() == 0U))


typedef struct
{
	char TskFlag;//建立短连接任务标志
	char downloadFlag;//下载任务执行标志
	char firstFrameFlag;//下载任务执行标志
}http_client_TxFlagStruct;


#if 0
typedef struct
{
	char data[CLIENT_CNNT_LNG];//临时数据缓存buf
	u16_t lng;//数据长度
	char dtAlidity;//数据有效性
	char txflag;//临时数据发送标志
}http_client_TxTempBufStruct;
#endif

typedef struct
{
	uint32 Xor;/*本段数据异或取反校验和*/
	uint32 DtFirstAddr;/*数据段首地址*/
	uint32 DtendAddr;/*数据段末地址*/
	uint32 Lng;/*数据长度*/
	uint32 CheckSum;/*升级区数据校验和*/
	uint32 IapFlag;/*在线升级标志*/
}http_client_UpdataInfoStruct;//升级信息


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void http_client_iap_parameter(void);
extern void http_client_iap_MainFunction(void);
extern char http_client_iap_connect(void);
extern void http_client_iap_disconnect(void);
extern void http_client_iap_sendMsg(char* data,u16_t len);
extern u8_t Gethttp_CnntTxSt(void);
#endif /*  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
