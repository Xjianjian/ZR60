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


/**********�곣�����e*******/
/********************************************************/
/* Base address of the Flash sectors */
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


#define EEPROM_SEC0_START_ADDR       	0x000000//EEPROM����ʼ��ַ:W25Q128 sector0��ʼ��ַ
#define EEPROM_SEC1_START_ADDR       	0x001000//EEPROM����ʼ��ַ:W25Q128 sector1��ʼ��ַ
#define EEPROM_SEC2_START_ADDR       	0x002000//EEPROM����ʼ��ַ:W25Q128 sector2��ʼ��ַ
#define EEPROM_SEC3_START_ADDR       	0x003000//EEPROM����ʼ��ַ:W25Q128 sector3��ʼ��ַ
#define EEPROM_SEC4_START_ADDR       	0x004000//EEPROM����ʼ��ַ:W25Q128 sector4��ʼ��ַ
#define EEPROM_SEC5_START_ADDR       	0x005000//EEPROM����ʼ��ַ:W25Q128 sector5��ʼ��ַ
#define EEPROM_SEC6_START_ADDR       	0x006000//EEPROM����ʼ��ַ:W25Q128 sector6��ʼ��ַ
#define EEPROM_SEC7_START_ADDR       	0x007000//EEPROM����ʼ��ַ:W25Q128 sector7��ʼ��ַ

/********************************************************/
//#define EE_DEBUG

#define EE_IDLE          	0U//����
#define EE_ERASE          	1U//����
#define EE_WR          		2U//д

#define EE_VALID_FIELD_VALUE    0x55//������Чʱд����Ч�ֶε�ֵ

#define EE_MAC_INFO_OFFSET     	3U//mac��ַ��Ϣ��ַ�����EE��ʼ��ַƫ����(addr[0]д������Ч�Ա�־��addr[1]addr[2]���ݳ�����Ϣ)
#define EE_MAC_INFO_ADDR       (EEPROM_SEC1_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ
#define EE_MAC_INFO_BACKUPONE  (EEPROM_SEC3_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ
#define EE_MAC_INFO_BACKUPTWO  (EEPROM_SEC4_START_ADDR +EE_MAC_INFO_OFFSET)//mac��ַ��Ϣ���ݶ���ʼ��ַ



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
	//void*  Lng;/*��������ݶγ���*/
	void*  Data;/*����ָ��*/
}EepromCfg_ConfStruct;

/******enum definitions******/
typedef enum 
{
	EepromCfg_UpDateInfo = 0U, /*IAP������Ϣ*/
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
extern const EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM];


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
