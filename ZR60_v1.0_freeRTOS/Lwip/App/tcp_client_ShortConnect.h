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

//#define SHORTCNNT_HEART  //����ʱ����������Ҫ��������
#define  SHORTCNNT_EXT_NET	  //����ʱʹ���ⲿ������,δ����ʹ������������
#define  SHORTCNNT_HTTPS		  //����ʱʹ��https����,δ����ʹ��http����


#define  SHORTCNNT_SCHEDULING_CYCLE		  5000U//��������

#define  CLIENT_SHORTCNNT_CARDNUM_LNG     4U
#define  CLIENT_SHORTCNNT_LNG   	      1000U//�������ݳ���
#define  CLIENT_LOGRECORD_PERIOD   	  	  (15000U/SHORTCNNT_SCHEDULING_CYCLE)//������¼���ڣ�15s
#define  CLIENT_DOORST_PERIOD   	  	  (30000U/SHORTCNNT_SCHEDULING_CYCLE)//����״̬�ϱ�����30s
#define  CLIENT_HEART_PERIOD   	  	  	  (30000U/SHORTCNNT_SCHEDULING_CYCLE)//�������ڣ�30s
#define  CLIENT_LOGRECORD_NUM   	      5U//һ���ϱ���log��¼����
#define  SHORTCNNT_PULLBLIST_PERIOD   	  (1800000U/SHORTCNNT_SCHEDULING_CYCLE)//������ȡ����������,0.5h
#define  SHORTCNNT_BLIST_PULLNUM   	  	  50//Ĭ��1�������ȡ��������


#define  CLIENT_SHORTCNNT_IDLE   	      0U
#define  CLIENT_SHORTCNNT_WAIT 			  1U
#define  CLIENT_SHORTCNNT_TXPCKT 		  2U//���ݷ���״̬
#define  CLIENT_SHORTCNNT_DISCNNT         3U

#define  CLIENT_SHORTCNNT_REMOTEPORT      443


#ifdef USE_DHCP
#define CLIENT_SHORTCNNT_DHCP_STATE    DHCP_state
#else
#define CLIENT_SHORTCNNT_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif

//�꺯������
#define Gettcp_client_u_DoorSt       GetPSFltr_u_PeriSt(DoorLockSt)
#define Gettcp_client_u_LogAvild     GetUnlockLogCache_u_LogAvild()
#define Gettcp_client_doorLog(x)     GetUnlockLogCache_doorLog(x)
#define GetShortCnnt_PerformCondition     ((Gethttp_CnntTxSt() ==  0U) && (GetStm8_fm17550_iapIdle() == 1))


#define tcp_client_BleMacFlg    Se_u_BleMacFlg//��ȡ����mac��־

typedef enum
{
	Pckt_Unknow = 0U,/*δ֪����*/
	Pckt_Init,/*�豸��ʼ����Ϣ*/
#ifdef SHORTCNNT_HEART
	Pckt_Heart,/*����*/
#endif
	Pckt_TempBuf,/*��ʱ������*/
	Pckt_DoorSt,/*����״̬*/
	Pckt_OpenReport,/*���ż�¼*/
	Pckt_BList/*��ȡ������*/
}tcp_client_TxPcktType;//��ǰ���͵ı�������


typedef struct
{
	//char TskFlag;//���������������־
	char InitFlag;//�ϱ���ʼ����Ϣ�ϱ�����ִ�б�־
#ifdef SHORTCNNT_HEART
	char HeartFlag;//
#endif
	char DoorStFlag;//����״̬�ϱ�����ִ�б�־
	char ReportFlag;//���ż�¼�ϱ�����ִ�б�־
	char BListFlag;//��ȡ����������ִ�б�־
	//char EchoFlag;//������Ӧ��־��0--����δ���ͣ�1--�ȴ���Ӧ��2--��Ӧ�ɹ�
	//char BusyFlag;//æ��־
	//char PcktType;//��ǰ���ڷ��͵����ݱ�������:��tcp_client_TxPcktType����
	char tokenOverdueFlag;//token���ڱ�־
}tcp_client_TxFlagStruct;

typedef struct
{
	char token[40];
	char Listtype;//��������
	uint64_t timestamp;//ʱ���
	uint64_t Oldtimestamp;//��ʱ���
	uint32_t page;//ҳ��
	uint32_t Oldpage;//��ҳ��
	uint16_t pageSize;//ҳ��
	uint8_t  UpdataFlag;//���±�־��0--ȫ�����£�1--��������
}tcp_client_BListPullStruct;

typedef struct
{
	char token[40];
	char state;//����״̬
}tcp_client_doorStStruct;

typedef struct
{
	char doorID[33U];//��id,���1�ֽڴ��'\0'
	char mac[18U];//stm32 mac��ַ,12���ֽڴ�ŵ�ַ�ַ���5���ֽڴ��'��'�����1�ֽڴ��'\0'
	char addrtype;//��ַ������Ϣ�������ߵ�MAC��ַ��0�������ߵ�MAC��ַ
	char Blemac[18U];//����mac��ַ,12���ֽڴ�ŵ�ַ�ַ���5���ֽڴ��'��'�����1�ֽڴ��'\0'
	char addrRecombineFlg;//��ַ�����־��112233445566����Ϊ11:22:33:44:55:66��־��λ
}tcp_client_DeviceInitStruct;


typedef struct
{
	char data[CLIENT_SHORTCNNT_LNG];//��ʱ���ݻ���buf
	u16_t lng;//���ݳ���
	char dtAlidity;//������Ч��
	//char txflag;//��ʱ���ݷ��ͱ�־
}tcp_client_TxTempBufStruct;

typedef struct
{
	char token[40];
	char LogNum;//��ǰ�ϱ��Ŀ��ż�¼����
	UnlockLogCacheStruct  Log[CLIENT_LOGRECORD_NUM];//���ż�¼������
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
