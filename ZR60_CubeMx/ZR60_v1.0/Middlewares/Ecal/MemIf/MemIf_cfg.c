/******************************************************
文件名：	MemIf_cfg.c

描述：		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "MemIf_cfg.h"

//uint8 CeMemIfCfg_u_CardSetLng = EE_CARD_SET_LNG;
/*******************************************************
description： global variable definitions
*******************************************************/
EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM] = 
{
	{EepromCfg_IntEE,EEPROM_SEC5_START_ADDR,EE_CARD_SET_ADDR,STD_NULL_PTR,0,"the configuration information of mother card"},//卡片配置信息
	{EepromCfg_IntEE,EEPROM_SEC1_START_ADDR,EE_MAC_INFO_ADDR,STD_NULL_PTR,0,"MAC address information"}//mac信息
	//{EepromCfg_IntEE,EEPROM_SEC6_START_ADDR,EE_TOKEN_INFO_ADDR,(char*)Vetcp_client_u_token,sizeof(Vetcp_client_u_token)}//token信息
};

const uint8  CaEepromCfg_CheckSumObj[EE_CHECKSUM_OBJ_NUM] = /*需要周期性校验flash中的数据正确性的数据对象*/
{
	EepromCfg_CardInfo,/*母卡配置信息*/
	//EepromCfg_MacInfo, /*mac地址信息*/
	//EepromCfg_CardInfoBkUpOne,/*母卡配置信息,备份*/
	//EepromCfg_CardInfoBkUpTwo,/*母卡配置信息,备份*/
	//EepromCfg_MacInfoBkUpOne, /*mac地址信息,备份*/
	//EepromCfg_MacInfoBkUpTwo /*mac地址信息,备份*/
};

const uint8  CaEepromCfg_ObjDtLng[EE_CHECKSUM_OBJ_NUM] = /*需要周期性校验flash中的数据正确性的数据对象数据长度*/
{
	82U,/*母卡配置信息*/
	//sizeof(Se_u_MacAddr), /*mac地址信息*/
	82U,/*母卡配置信息,备份*/
	82U,/*母卡配置信息,备份*/
	//sizeof(Se_u_MacAddr), /*mac地址信息,备份*/
	//sizeof(Se_u_MacAddr) /*mac地址信息,备份*/
};


/*******************************************************
description： static variable definitions
*******************************************************/


/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：eeprom解锁
 
*备  注：
******************************************************/
void MemIfCfg_FLASH_Unlock(void)
{
#ifndef MemIf_IntEE
	W25QXX_Write_SR(0x00);//CMP=0时，解除W25Q128 16M memory写保护
	W25QXX_Write_Enable(); //EEPROM解除保护
#endif
}

/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：eeprom加锁保护
 
*备  注：
******************************************************/
void MemIfCfg_FLASH_Lock(void)
{
#ifndef MemIf_IntEE
	W25QXX_Write_SR(0x1c);//CMP=0时，开启W25Q128 16M memory写保护
	W25QXX_Write_Disable();//EEPROM加上保护
#endif
}

/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：写eeprom
 
*备  注：
******************************************************/
void MemIfCfg_FLASH_ProgramByte(uint32 Address, uint8 Data)
{
#ifndef MemIf_IntEE
	SPI_FLASH_BufferWrite(&Data,Address,1);
#endif
}

/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：读eeprom
 
*备  注：
******************************************************/
uint8 MemIfCfg_FLASH_ReadByte(uint32 Le_u_Addr)
{
	uint8 ReadByte;
	
	//SPI_FLASH_BufferRead(&ReadByte,Le_u_Addr,1);
	__disable_irq() ; //关闭总中断
	ReadByte = *(__IO uint8*)Le_u_Addr;//读FLASH中的数据，直接给出地址就行了。跟从内存中读数据一样
	__enable_irq() ; //打开总中断
	return ReadByte;
}


/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：eeprom扇区擦除
 
*备  注：
******************************************************/
void MemIfCfg_FLASH_SectorErase(uint32 Le_u_Addr)
{
#ifndef MemIf_IntEE
	SPI_FLASH_SectorErase(Le_u_Addr);
#endif
}


/******************************************************
*函数名：
 
*形  参：
 
*返回值：void
 
*描  述：eeprom空闲状态
 
*备  注：
******************************************************/
uint8 MemIfCfg_FLASH_IDLE(void)
{
#ifndef MemIf_IntEE
	uint8 ret = 0U;
	if((0U == GetAudioIO_u_PlaySt()) && (0U == SPI_FLASH_WrInProgStatus()))
	{
		ret = 1U;
	}
	return ret;
#endif
	return 1;
}
