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

uint8 Se_u_MacAddr = 0;
/*******************************************************
description： global variable definitions
*******************************************************/
const EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM] = 
{
	{EepromCfg_IntEE,ADDR_FLASH_SECTOR_2,ADDR_FLASH_SECTOR_2,STD_NULL_PTR}//升级区信息
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
	//W25QXX_Write_SR(0x00);//CMP=0时，解除W25Q128 16M memory写保护
	FLASH_Unlock(); //EEPROM解除保护
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
	//W25QXX_Write_SR(0x1c);//CMP=0时，开启W25Q128 16M memory写保护
	FLASH_Lock();//EEPROM加上保护
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
	//SPI_FLASH_BufferWrite(&Data,Address,1);
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
	//SPI_FLASH_SectorErase(Le_u_Addr);
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
	uint8 ret = 0U;
	//if(0U == SPI_FLASH_WrInProgStatus())
	{
		ret = 1U;
	}
	return ret;
}
