/**
  ******************************************************************************
  * @file    tcp_client_LngConnect.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header file for tcp_client_LngConnect.c
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
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include	"Include.h"
//#include	"netconf.h"
#include "STM8_FM17550_iap.h"
#include "http_client_iap.h"

#define  LNGCNNT_SCHEDULING_CYCLE		  35U//调度周期
#define  CLIENT_LNGCNNT_PERIOD   	      (33000U/LNGCNNT_SCHEDULING_CYCLE)
#define  CLIENT_LNGCNNT_REPLY_TIMEOUT  	  (75000U/LNGCNNT_SCHEDULING_CYCLE)
#define  LNGCNNT_ESTABLISHED_TIMEOUT	  (3000U/LNGCNNT_SCHEDULING_CYCLE)

#define  CLIENT_LNGCNNT_IDLE   	   0U
#define  CLIENT_LNGCNNT_CNNT       1U
#define  CLIENT_LNGCNNT_DISCNNT    2U


#define CLIENT_LNGCNNT_DEVICETYPE   machine_type
#ifdef USE_DHCP
#define CLIENT_LNGCNNT_DHCP_STATE    DHCP_state
#else
#define CLIENT_LNGCNNT_DHCP_STATE    DHCP_ADDRESS_ASSIGNED
#endif


#define GetLngCnnt_PerformCondition     ((Gethttp_CnntTxSt() ==  0U) && (GetStm8_fm17550_iapIdle() == 1))

typedef struct
{
	char Alias[39U];//别名
	unsigned char lng;//有效位长度
}tcp_client_Struct;

extern tcp_client_Struct  Setcp_Alias;
extern u8_t  volatile Setcp_client_u_Open;//远程开锁标志
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void tcp_LngConnect_Parameter(void);
extern void tcp_LngConnect_MainFunction(void);
extern uint8 Gettcp_u_LngConnect_Timeout(void);
extern void tcp_client_LngConnect_callback(void);
extern void Gettcp_LngConnect_PhoneNo(char* Le_u_dt);
#endif /**/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
