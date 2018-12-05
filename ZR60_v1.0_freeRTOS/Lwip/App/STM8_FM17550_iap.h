/**
  ******************************************************************************
  * @file    STM8_FM17550_iap.h
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
#ifndef __HTTP_STM8_FM17550_IAP_H__
#define __HTTP_STM8_FM17550_IAP_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyType.h"
#include "stm32f4xx_flash.h"
#include "dnsAnalysis.h"
/*Lwip���ļ�*/
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
#include "./crc32/crc32.h"
#include "tcp_client_ShortConnect.h"

#define  STM8_IAP_SCHEDULING_CYCLE    50U//��������
#define  STM8_FM17550_TXBUF   	      200U//�������ݳ���

#define  STM8_FM17550_IDLE   	     0U
#define  STM8_FM17550_WAIT 			 1U
#define  STM8_FM17550_LOADPCKT 		 2U//��������
#define  STM8_FM17550_DISCNNT        3U

#define  STM8_FM17550_DOWNLOAD_IDLE   	     0U
#define  STM8_FM17550_DOWNLOAD_UNDERWAY 	 1U
#define  STM8_FM17550_DOWNLOAD_FINISH 		 2U//�����������

#define  STM8_FM17550_NNT_REMOTEPORT     	 80


#ifdef USE_DHCP
#define STM8_FM17550_DHCP_STATE    DHCP_state
#else
#define STM8_FM17550_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif

/* Base address of the STM32 Flash sectors ������stm32��Ƭ��flash��������ַ�������޸�*/
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

#define STM8_FM17550_CODE_SEC  ADDR_FLASH_SECTOR_3//������ģ�������̼����ش�ŵ���������ַ
//#define HTTP_CLIENT_UPLOAD_INFO_SEC ADDR_FLASH_SECTOR_3//������ģ�������̼����ش�ŵ���������ַ




#define  STM8_FM17550_DOWNLOAD_DATA_LNG   	      (16*1024)//��������С���ֽ�����
#define  STM8_FM17550_DOWNLOAD_TIMEOUT   	      (6000U/(STM8_IAP_SCHEDULING_CYCLE/5U))//���س�ʱʱ��

//�꺯������
#define stm8_fm17550_PerformCondition  (0U == Gettcp_shortCnntTxSt())


typedef struct
{
	char TskFlag;//���������������־
	char downloadFlag;//��������ִ�б�־
	char EchoFlag;//usart����ָ��/������Ӧ��־
	char SlaveRxSt;//�ӻ����ݽ���״̬��0--ok
	char TxBusyFlag;//����<->�ӻ���æ��־
}stm8_fm17550_IapFlagStruct;//����������־�ṹ��


#if 0
typedef struct
{
	char data[STM8_FM17550_TXBUF];//��ʱ���ݻ���buf
	u16_t lng;//���ݳ���
	char dtAlidity;//������Ч��
	char txflag;//��ʱ���ݷ��ͱ�־
}stm8_fm17550_TxTempBufStruct;
#endif

typedef struct
{
	//uint32 Xor;/*�����������ȡ��У���*/
	uint32 DtFirstAddr;/*���ݶ��׵�ַ*/
	//uint32 DtendAddr;/*���ݶ�ĩ��ַ*/
	uint32 Lng;/*���ݳ���*/
	uint32 CheckSum;/*����������У���*/
	uint32 Cnt;/*�ֽڼ�����*/
}stm8_fm17550_UpdataInfoStruct;//������Ϣ

typedef union
{
	unsigned char FrmTx[70U];
	struct
	{
		unsigned char Header;//֡ͷ
		unsigned char Seqnr;//����֡�����
		unsigned char Cmd;//���ݳ���,0�����ݣ�1����ֹ
		unsigned char Lng;//���ݳ���
		unsigned char Data[64];//����
		unsigned char CC;//У���
		unsigned char ETX;//�����ַ�
	}Frame;
}stm8_fm17550_Frame_Tx;


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void stm8_fm17550_iap_parameter(void);
extern void stm8_fm17550_iap_MainFunction(void);
extern void Setstm8_fm17550_SlaveReplySt(u8_t Le_u_St);
extern uint8_t GetStm8_fm17550_iapIdle(void);
#endif /*  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
