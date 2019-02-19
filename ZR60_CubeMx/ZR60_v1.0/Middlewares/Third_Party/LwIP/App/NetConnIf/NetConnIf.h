/**************************************************************
  * @file    NetConnIf.h
  * @author  MCD Application Team
  * @version V1.0
  * @date    22-11-2018
  * @brief
***************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NET_CONN_IF_H__
#define __NET_CONN_IF_H__

#include "./NetConnIf/NetConnIf_Cfg.h"


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//extern void NetConnIf_Parameter(void);
//extern void NetConnIf_MainFunction(void);
extern void NetConnIf_Parameter(void);
extern char NetConnIf_Connect(char Le_u_obj,ip_addr_t *Le_u_DestIPaddr, uint16_t Le_w_port);
extern void NetConnIf_disconnect(char Le_u_obj);
extern void NetConnIf_sendMsg(char Le_u_obj,char* data,uint16_t len);
extern void NetConnIf_rxConfig(char Le_u_obj,char* Le_p_rxbuf,uint16_t Le_w_lng,void* rx_callback_fn);
extern char NetConnIf_ConnectStatus(char Le_u_obj);
extern char NetConnIf_RemoteConnectClose(char Le_u_obj);
#endif /**/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
