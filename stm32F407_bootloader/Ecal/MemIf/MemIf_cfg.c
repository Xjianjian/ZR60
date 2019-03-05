/******************************************************
�ļ�����	MemIf_cfg.c

������		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "MemIf_cfg.h"

uint8 Se_u_MacAddr = 0;
/*******************************************************
description�� global variable definitions
*******************************************************/
const EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM] = 
{
	{EepromCfg_IntEE,ADDR_FLASH_SECTOR_2,ADDR_FLASH_SECTOR_2,STD_NULL_PTR}//��������Ϣ
};

/*******************************************************
description�� static variable definitions
*******************************************************/


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/


/*Static function declaration*/



/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����eeprom����
 
*��  ע��
******************************************************/
void MemIfCfg_FLASH_Unlock(void)
{
	//W25QXX_Write_SR(0x00);//CMP=0ʱ�����W25Q128 16M memoryд����
	FLASH_Unlock(); //EEPROM�������
}

/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����eeprom��������
 
*��  ע��
******************************************************/
void MemIfCfg_FLASH_Lock(void)
{
	//W25QXX_Write_SR(0x1c);//CMP=0ʱ������W25Q128 16M memoryд����
	FLASH_Lock();//EEPROM���ϱ���
}

/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����дeeprom
 
*��  ע��
******************************************************/
void MemIfCfg_FLASH_ProgramByte(uint32 Address, uint8 Data)
{
	//SPI_FLASH_BufferWrite(&Data,Address,1);
}

/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ������eeprom
 
*��  ע��
******************************************************/
uint8 MemIfCfg_FLASH_ReadByte(uint32 Le_u_Addr)
{
	uint8 ReadByte;
	
	//SPI_FLASH_BufferRead(&ReadByte,Le_u_Addr,1);
	
	return ReadByte;
}


/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����eeprom��������
 
*��  ע��
******************************************************/
void MemIfCfg_FLASH_SectorErase(uint32 Le_u_Addr)
{
	//SPI_FLASH_SectorErase(Le_u_Addr);
}


/******************************************************
*��������
 
*��  �Σ�
 
*����ֵ��void
 
*��  ����eeprom����״̬
 
*��  ע��
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
