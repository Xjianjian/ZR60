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

//uint8 CeMemIfCfg_u_CardSetLng = EE_CARD_SET_LNG;
/*******************************************************
description�� global variable definitions
*******************************************************/
EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM] = 
{
	{EepromCfg_IntEE,EEPROM_SEC5_START_ADDR,EE_CARD_SET_ADDR,STD_NULL_PTR,0,"the configuration information of mother card"},//��Ƭ������Ϣ
	{EepromCfg_IntEE,EEPROM_SEC1_START_ADDR,EE_MAC_INFO_ADDR,STD_NULL_PTR,0,"MAC address information"}//mac��Ϣ
	//{EepromCfg_IntEE,EEPROM_SEC6_START_ADDR,EE_TOKEN_INFO_ADDR,(char*)Vetcp_client_u_token,sizeof(Vetcp_client_u_token)}//token��Ϣ
};

const uint8  CaEepromCfg_CheckSumObj[EE_CHECKSUM_OBJ_NUM] = /*��Ҫ������У��flash�е�������ȷ�Ե����ݶ���*/
{
	EepromCfg_CardInfo,/*ĸ��������Ϣ*/
	//EepromCfg_MacInfo, /*mac��ַ��Ϣ*/
	//EepromCfg_CardInfoBkUpOne,/*ĸ��������Ϣ,����*/
	//EepromCfg_CardInfoBkUpTwo,/*ĸ��������Ϣ,����*/
	//EepromCfg_MacInfoBkUpOne, /*mac��ַ��Ϣ,����*/
	//EepromCfg_MacInfoBkUpTwo /*mac��ַ��Ϣ,����*/
};

const uint8  CaEepromCfg_ObjDtLng[EE_CHECKSUM_OBJ_NUM] = /*��Ҫ������У��flash�е�������ȷ�Ե����ݶ������ݳ���*/
{
	82U,/*ĸ��������Ϣ*/
	//sizeof(Se_u_MacAddr), /*mac��ַ��Ϣ*/
	82U,/*ĸ��������Ϣ,����*/
	82U,/*ĸ��������Ϣ,����*/
	//sizeof(Se_u_MacAddr), /*mac��ַ��Ϣ,����*/
	//sizeof(Se_u_MacAddr) /*mac��ַ��Ϣ,����*/
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
#ifndef MemIf_IntEE
	W25QXX_Write_SR(0x00);//CMP=0ʱ�����W25Q128 16M memoryд����
	W25QXX_Write_Enable(); //EEPROM�������
#endif
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
#ifndef MemIf_IntEE
	W25QXX_Write_SR(0x1c);//CMP=0ʱ������W25Q128 16M memoryд����
	W25QXX_Write_Disable();//EEPROM���ϱ���
#endif
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
#ifndef MemIf_IntEE
	SPI_FLASH_BufferWrite(&Data,Address,1);
#endif
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
	__disable_irq() ; //�ر����ж�
	ReadByte = *(__IO uint8*)Le_u_Addr;//��FLASH�е����ݣ�ֱ�Ӹ�����ַ�����ˡ������ڴ��ж�����һ��
	__enable_irq() ; //�����ж�
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
#ifndef MemIf_IntEE
	SPI_FLASH_SectorErase(Le_u_Addr);
#endif
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
