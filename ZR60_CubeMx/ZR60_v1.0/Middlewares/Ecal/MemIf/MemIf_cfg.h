/******************************************************
�ļ�����	MemIf_cfg.h

������

Data			  Vasion			author
2018/04/06		  V1.0			    liujian
*******************************************************/
#ifndef		MEMIF_CFG_H
#define		MEMIF_CFG_H
/*******************************************************
description?nclude the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description?acro definitions
*******************************************************/
/**********�꿪�ض��e*******/
#define   MEMIF_HAL     //����ʱ��ʾʹ��HAL��

#ifdef 	  MEMIF_HAL
#define   MEMIF_ERASE_SECTOR   FLASH_SECTOR_4
#else
#define   MEMIF_ERASE_SECTOR   FLASH_Sector_4
#endif

#define MEMIF_DEBUG
#ifdef 	MEMIF_DEBUG
#define MEMIF_PRINTF_S(x)   	 printf("%s\n",x)
#define MEMIF_PRINTF_D(x,d)   	 printf(x,d)
#else
#define MEMIF_PRINTF_S(x)    
#define MEMIF_PRINTF_D(x,d)  
#endif

/**********�곣�����e*******/
/********************************************************/
#define EEPROM_SEC0_START_ADDR       	0x000000//EEPROM����ʼ��ַ:W25Q128 sector0��ʼ��ַ
#define EEPROM_SEC1_START_ADDR       	((uint32_t)0x08010400) /* Ƭ��flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC2_START_ADDR       	0x002000//EEPROM����ʼ��ַ:W25Q128 sector2��ʼ��ַ
#define EEPROM_SEC3_START_ADDR       	0x015000//EEPROM����ʼ��ַ:W25Q128 sector3��ʼ��ַ
#define EEPROM_SEC4_START_ADDR       	0x024000//EEPROM����ʼ��ַ:W25Q128 sector4��ʼ��ַ
#define EEPROM_SEC5_START_ADDR       	((uint32_t)0x08010000) /* Ƭ��flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC6_START_ADDR       	((uint32_t)0x08010800) /* Ƭ��flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC7_START_ADDR       	0x023000//EEPROM����ʼ��ַ:W25Q128 sector7��ʼ��ַ

#define EEPROM_SEC10_START_ADDR       	0x00A000//EEPROM����ʼ��ַ:W25Q128 sector10��ʼ��ַ
#define EEPROM_SEC11_START_ADDR       	0x00B000//EEPROM����ʼ��ַ:W25Q128 sector11��ʼ��ַ
#define EEPROM_SEC12_START_ADDR       	0x00C000//EEPROM����ʼ��ַ:W25Q128 sector12��ʼ��ַ
#define EEPROM_SEC13_START_ADDR       	0x00D000//EEPROM����ʼ��ַ:W25Q128 sector13��ʼ��ַ
#define EEPROM_SEC14_START_ADDR       	0x00E000//EEPROM����ʼ��ַ:W25Q128 sector14��ʼ��ַ
#define EEPROM_SEC15_START_ADDR       	0x00F000//EEPROM����ʼ��ַ:W25Q128 sector15��ʼ��ַ
#define EEPROM_SEC16_START_ADDR       	0x010000//EEPROM����ʼ��ַ:W25Q128 sector16��ʼ��ַ
#define EEPROM_SEC17_START_ADDR       	0x011000//EEPROM����ʼ��ַ:W25Q128 sector17��ʼ��ַ
#define EEPROM_SEC18_START_ADDR       	0x012000//EEPROM����ʼ��ַ:W25Q128 sector18��ʼ��ַ
#define EEPROM_SEC19_START_ADDR       	0x013000//EEPROM����ʼ��ַ:W25Q128 sector19��ʼ��ַ
//#define EEPROM_SEC2_START_ADDR       	0x002000//EEPROM����ʼ��ַ:W25Q128 sector2��ʼ��ַ
/********************************************************/
//#define EE_DEBUG
#define MemIf_SCHEDULING_CYCLE     10U//��������
#define MemIf_IntEE          //����Ƭ��flash

#define EE_IDLE          	0U//����
#define EE_ERASE          	1U//����
#define EE_WR          		2U//д

#define EE_VALID_FIELD_VALUE    0x55//������Чʱд����Ч�ֶε�ֵ
#define EE_CHECKSUM_OBJ_NUM    	3U//У��Ͷ�����

#define EE_CARD_SET_OFFSET     	4U//��Ƭ������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д����У��ͣ�addr[1]addr[2]���ݳ�����Ϣ��addr[3]��Ч�Ա�־)
#define EE_CARD_SET_ADDR       (EEPROM_SEC5_START_ADDR +EE_CARD_SET_OFFSET)//��Ƭ������Ϣ���ݶ���ʼ��ַ
//#define EE_CARD_SET_BACKUPONE  (EEPROM_SEC6_START_ADDR +EE_CARD_SET_OFFSET)//��Ƭ������Ϣ���ݶ���ʼ��ַ
//#define EE_CARD_SET_BACKUPTWO  (EEPROM_SEC7_START_ADDR +EE_CARD_SET_OFFSET)//��Ƭ������Ϣ���ݶ���ʼ��ַ

#define EE_MAC_INFO_OFFSET     	4U//mac��ַ��Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д����У��ͣ�addr[1]addr[2]���ݳ�����Ϣ��addr[3]��Ч�Ա�־)
#define EE_MAC_INFO_ADDR       (EEPROM_SEC1_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ
//#define EE_MAC_INFO_BACKUPONE  (EEPROM_SEC3_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ
//#define EE_MAC_INFO_BACKUPTWO  (EEPROM_SEC4_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ

#define EE_TOKEN_INFO_OFFSET     	4U//token��Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д����У��ͣ�addr[1]addr[2]���ݳ�����Ϣ��addr[3]��Ч�Ա�־)
#define EE_TOKEN_INFO_ADDR       (EEPROM_SEC6_START_ADDR +EE_TOKEN_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ

#define EE_BLIST_TIMESTAMP_PAGE_OFFSET     	4U//������ʱ�����ҳ����Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ��addr[3]��Ч�Ա�־)
#define EE_BLIST_TIMESTAMP_PAGE_ADDR       (EEPROM_SEC2_START_ADDR +EE_BLIST_TIMESTAMP_PAGE_OFFSET)//������ʱ�����ҳ����Ϣ���ݶ���ʼ��ַ

#define EE_BLIST_NO1_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO1_ADDR       	(EEPROM_SEC10_START_ADDR +EE_BLIST_NO1_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO2_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO2_ADDR       	(EEPROM_SEC11_START_ADDR +EE_BLIST_NO2_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO3_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO3_ADDR       	(EEPROM_SEC12_START_ADDR +EE_BLIST_NO3_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO4_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO4_ADDR       	(EEPROM_SEC13_START_ADDR +EE_BLIST_NO4_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO5_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO5_ADDR       	(EEPROM_SEC14_START_ADDR +EE_BLIST_NO5_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO6_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO6_ADDR       	(EEPROM_SEC15_START_ADDR +EE_BLIST_NO6_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO7_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO7_ADDR       	(EEPROM_SEC16_START_ADDR +EE_BLIST_NO7_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO8_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO8_ADDR       	(EEPROM_SEC17_START_ADDR +EE_BLIST_NO8_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO9_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO9_ADDR       	(EEPROM_SEC18_START_ADDR +EE_BLIST_NO9_OFFSET)//�������������ݶ���ʼ��ַ
#define EE_BLIST_NO10_OFFSET     	3U//������������Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_BLIST_NO10_ADDR       	(EEPROM_SEC19_START_ADDR +EE_BLIST_NO10_OFFSET)//�������������ݶ���ʼ��ַ


/***********�꺯��*********/

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint8  Space;/*���ʵĴ洢λ�ã�Ƭ�ڻ�Ƭ��*/
	uint32 SecAddr;/*�����׵�ַ*/	
	uint32 DtAddr;/*��Ч�����׵�ַ*/	
	char*  Data;/*����ָ��*/
	uint8  Lng;/*��Ч���ݳ���*/
}EepromCfg_ConfStruct;

/******enum definitions******/
typedef enum 
{
	EepromCfg_CardInfo = 0U,/*ĸ��������Ϣ*/
	//EepromCfg_MacInfo, /*mac��ַ��Ϣ*/
	//EepromCfg_tokenInfo, /*��ʼ���豸��token��Ϣ*/
	//EepromCfg_Blist_one, /*��ע��������Ϣ:������1*/
	//EepromCfg_Blist_two, /*��ע��������Ϣ:������2*/
	//EepromCfg_Blist_three, /*��ע��������Ϣ:������3*/
	//EepromCfg_Blist_four, /*��ע��������Ϣ:������4*/
	//EepromCfg_Blist_five, /*��ע��������Ϣ:������5*/
	//EepromCfg_Blist_six, /*��ע��������Ϣ:������6*/
	//EepromCfg_Blist_seven, /*��ע��������Ϣ:������7*/
	//EepromCfg_Blist_eight, /*��ע��������Ϣ:������8*/
	//EepromCfg_Blist_nine, /*��ע��������Ϣ:������9*/
	//EepromCfg_Blist_ten, /*��ע��������Ϣ:������10*/
	//EepromCfg_timestamp_page, /*������ʱ�����ҳ��*/
	//EepromCfg_CardInfoBkUpOne,/*ĸ��������Ϣ,����*/
	//EepromCfg_CardInfoBkUpTwo,/*ĸ��������Ϣ,����*/
	//EepromCfg_MacInfoBkUpOne, /*mac��ַ��Ϣ,����*/
	//EepromCfg_MacInfoBkUpTwo, /*mac��ַ��Ϣ,����*/
	EE_OBJECT_NUM
}EepromCfg_StrgObject;

typedef enum 
{
	EepromCfg_IntEE = 0U,/*Ƭ��EE*/
	EepromCfg_ExtEE = 1U/*Ƭ��EE*/
}EepromCfg_EE_Index;
/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/
extern EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM];
extern const uint8  CaEepromCfg_CheckSumObj[EE_CHECKSUM_OBJ_NUM];/*��Ҫ������У��flash�е�������ȷ�Ե����ݶ���*/
extern const uint8  CaEepromCfg_ObjDtLng[EE_CHECKSUM_OBJ_NUM];/*��Ҫ������У��flash�е�������ȷ�Ե����ݶ������ݳ���*/
/*******************************************************
description?unction External declaration
*******************************************************/
extern uint8  MemIfCfg_WriteEE(uint32 Le_u_Addr,uint8* Le_u_Data,uint8 Le_u_Lng);
extern uint8  MemIfCfg_ReadEE(uint32 Le_u_Addr,uint8* Le_u_Data,uint8 Le_u_Lng);
extern void  MemIfCfg_FLASH_Unlock(void);
extern void  MemIfCfg_FLASH_Lock(void);
extern void  MemIfCfg_FLASH_ProgramByte(uint32 Address, uint8 Data);
extern uint8 MemIfCfg_FLASH_ReadByte(uint32 Le_u_Addr);
extern void MemIfCfg_FLASH_SectorErase(uint32 Le_u_Addr);
extern uint8 MemIfCfg_FLASH_IDLE(void);
#endif
