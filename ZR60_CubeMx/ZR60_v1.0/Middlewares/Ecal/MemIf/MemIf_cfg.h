/******************************************************
文件名：	MemIf_cfg.h

描述）

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
/**********宏开关定乪*******/
#define   MEMIF_HAL     //定义时表示使用HAL库

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

/**********宏常量定乪*******/
/********************************************************/
#define EEPROM_SEC0_START_ADDR       	0x000000//EEPROM的起始地址:W25Q128 sector0起始地址
#define EEPROM_SEC1_START_ADDR       	((uint32_t)0x08010400) /* 片上flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC2_START_ADDR       	0x002000//EEPROM的起始地址:W25Q128 sector2起始地址
#define EEPROM_SEC3_START_ADDR       	0x015000//EEPROM的起始地址:W25Q128 sector3起始地址
#define EEPROM_SEC4_START_ADDR       	0x024000//EEPROM的起始地址:W25Q128 sector4起始地址
#define EEPROM_SEC5_START_ADDR       	((uint32_t)0x08010000) /* 片上flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC6_START_ADDR       	((uint32_t)0x08010800) /* 片上flash Base @ of Sector 4, 64 Kbytes */
#define EEPROM_SEC7_START_ADDR       	0x023000//EEPROM的起始地址:W25Q128 sector7起始地址

#define EEPROM_SEC10_START_ADDR       	0x00A000//EEPROM的起始地址:W25Q128 sector10起始地址
#define EEPROM_SEC11_START_ADDR       	0x00B000//EEPROM的起始地址:W25Q128 sector11起始地址
#define EEPROM_SEC12_START_ADDR       	0x00C000//EEPROM的起始地址:W25Q128 sector12起始地址
#define EEPROM_SEC13_START_ADDR       	0x00D000//EEPROM的起始地址:W25Q128 sector13起始地址
#define EEPROM_SEC14_START_ADDR       	0x00E000//EEPROM的起始地址:W25Q128 sector14起始地址
#define EEPROM_SEC15_START_ADDR       	0x00F000//EEPROM的起始地址:W25Q128 sector15起始地址
#define EEPROM_SEC16_START_ADDR       	0x010000//EEPROM的起始地址:W25Q128 sector16起始地址
#define EEPROM_SEC17_START_ADDR       	0x011000//EEPROM的起始地址:W25Q128 sector17起始地址
#define EEPROM_SEC18_START_ADDR       	0x012000//EEPROM的起始地址:W25Q128 sector18起始地址
#define EEPROM_SEC19_START_ADDR       	0x013000//EEPROM的起始地址:W25Q128 sector19起始地址
//#define EEPROM_SEC2_START_ADDR       	0x002000//EEPROM的起始地址:W25Q128 sector2起始地址
/********************************************************/
//#define EE_DEBUG
#define MemIf_SCHEDULING_CYCLE     10U//调度周期
#define MemIf_IntEE          //操作片上flash

#define EE_IDLE          	0U//空闲
#define EE_ERASE          	1U//擦除
#define EE_WR          		2U//写

#define EE_VALID_FIELD_VALUE    0x55//数据有效时写入有效字段的值
#define EE_CHECKSUM_OBJ_NUM    	3U//校验和对象数

#define EE_CARD_SET_OFFSET     	4U//卡片配置信息地址相对于EE起始地址偏移量(addr[0]写数据校验和；addr[1]addr[2]数据长度信息；addr[3]有效性标志)
#define EE_CARD_SET_ADDR       (EEPROM_SEC5_START_ADDR +EE_CARD_SET_OFFSET)//卡片配置信息数据段起始地址
//#define EE_CARD_SET_BACKUPONE  (EEPROM_SEC6_START_ADDR +EE_CARD_SET_OFFSET)//卡片配置信息数据段起始地址
//#define EE_CARD_SET_BACKUPTWO  (EEPROM_SEC7_START_ADDR +EE_CARD_SET_OFFSET)//卡片配置信息数据段起始地址

#define EE_MAC_INFO_OFFSET     	4U//mac地址信息地址相对于EE起始地址偏移量(addr[0]写数据校验和；addr[1]addr[2]数据长度信息；addr[3]有效性标志)
#define EE_MAC_INFO_ADDR       (EEPROM_SEC1_START_ADDR +EE_MAC_INFO_OFFSET)//mac地址信息数据段起始地址
//#define EE_MAC_INFO_BACKUPONE  (EEPROM_SEC3_START_ADDR +EE_MAC_INFO_OFFSET)//mac地址信息数据段起始地址
//#define EE_MAC_INFO_BACKUPTWO  (EEPROM_SEC4_START_ADDR +EE_MAC_INFO_OFFSET)//mac地址信息数据段起始地址

#define EE_TOKEN_INFO_OFFSET     	4U//token信息地址相对于EE起始地址偏移量(addr[0]写数据校验和；addr[1]addr[2]数据长度信息；addr[3]有效性标志)
#define EE_TOKEN_INFO_ADDR       (EEPROM_SEC6_START_ADDR +EE_TOKEN_INFO_OFFSET)//mac地址信息数据段起始地址

#define EE_BLIST_TIMESTAMP_PAGE_OFFSET     	4U//黑名单时间戳，页码信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息；addr[3]有效性标志)
#define EE_BLIST_TIMESTAMP_PAGE_ADDR       (EEPROM_SEC2_START_ADDR +EE_BLIST_TIMESTAMP_PAGE_OFFSET)//黑名单时间戳，页码信息数据段起始地址

#define EE_BLIST_NO1_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO1_ADDR       	(EEPROM_SEC10_START_ADDR +EE_BLIST_NO1_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO2_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO2_ADDR       	(EEPROM_SEC11_START_ADDR +EE_BLIST_NO2_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO3_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO3_ADDR       	(EEPROM_SEC12_START_ADDR +EE_BLIST_NO3_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO4_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO4_ADDR       	(EEPROM_SEC13_START_ADDR +EE_BLIST_NO4_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO5_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO5_ADDR       	(EEPROM_SEC14_START_ADDR +EE_BLIST_NO5_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO6_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO6_ADDR       	(EEPROM_SEC15_START_ADDR +EE_BLIST_NO6_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO7_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO7_ADDR       	(EEPROM_SEC16_START_ADDR +EE_BLIST_NO7_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO8_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO8_ADDR       	(EEPROM_SEC17_START_ADDR +EE_BLIST_NO8_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO9_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO9_ADDR       	(EEPROM_SEC18_START_ADDR +EE_BLIST_NO9_OFFSET)//黑名单卡号数据段起始地址
#define EE_BLIST_NO10_OFFSET     	3U//黑名单卡号信息地址相对于EE起始地址偏移量(addr[0]写数据有效性标志；addr[1]addr[2]数据长度信息)
#define EE_BLIST_NO10_ADDR       	(EEPROM_SEC19_START_ADDR +EE_BLIST_NO10_OFFSET)//黑名单卡号数据段起始地址


/***********宏函敪*********/

/*******************************************************
description?truct definitions
*******************************************************/

/*******************************************************
description?ypedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint8  Space;/*访问的存储位置：片内或片外*/
	uint32 SecAddr;/*扇区首地址*/	
	uint32 DtAddr;/*有效数据首地址*/	
	char*  Data;/*数据指针*/
	uint8  Lng;/*有效数据长度*/
}EepromCfg_ConfStruct;

/******enum definitions******/
typedef enum 
{
	EepromCfg_CardInfo = 0U,/*母卡配置信息*/
	//EepromCfg_MacInfo, /*mac地址信息*/
	//EepromCfg_tokenInfo, /*初始化设备的token信息*/
	//EepromCfg_Blist_one, /*已注销卡号信息:黑名单1*/
	//EepromCfg_Blist_two, /*已注销卡号信息:黑名单2*/
	//EepromCfg_Blist_three, /*已注销卡号信息:黑名单3*/
	//EepromCfg_Blist_four, /*已注销卡号信息:黑名单4*/
	//EepromCfg_Blist_five, /*已注销卡号信息:黑名单5*/
	//EepromCfg_Blist_six, /*已注销卡号信息:黑名单6*/
	//EepromCfg_Blist_seven, /*已注销卡号信息:黑名单7*/
	//EepromCfg_Blist_eight, /*已注销卡号信息:黑名单8*/
	//EepromCfg_Blist_nine, /*已注销卡号信息:黑名单9*/
	//EepromCfg_Blist_ten, /*已注销卡号信息:黑名单10*/
	//EepromCfg_timestamp_page, /*黑名单时间戳、页码*/
	//EepromCfg_CardInfoBkUpOne,/*母卡配置信息,备份*/
	//EepromCfg_CardInfoBkUpTwo,/*母卡配置信息,备份*/
	//EepromCfg_MacInfoBkUpOne, /*mac地址信息,备份*/
	//EepromCfg_MacInfoBkUpTwo, /*mac地址信息,备份*/
	EE_OBJECT_NUM
}EepromCfg_StrgObject;

typedef enum 
{
	EepromCfg_IntEE = 0U,/*片内EE*/
	EepromCfg_ExtEE = 1U/*片外EE*/
}EepromCfg_EE_Index;
/******union definitions*****/




/*******************************************************
description?ariable External declaration
*******************************************************/
extern EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM];
extern const uint8  CaEepromCfg_CheckSumObj[EE_CHECKSUM_OBJ_NUM];/*需要周期性校验flash中的数据正确性的数据对象*/
extern const uint8  CaEepromCfg_ObjDtLng[EE_CHECKSUM_OBJ_NUM];/*需要周期性校验flash中的数据正确性的数据对象数据长度*/
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
