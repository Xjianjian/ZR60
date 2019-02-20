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
const EepromCfg_ConfStruct    CaEepromCfg_Conf[EE_OBJECT_NUM] = 
{
	{EepromCfg_IntEE,EEPROM_SEC5_START_ADDR,EE_CARD_SET_ADDR,(uint8*)sminfo.community_id,sizeof(sminfo)},//��Ƭ������Ϣ
	{EepromCfg_IntEE,EEPROM_SEC1_START_ADDR,EE_MAC_INFO_ADDR,(uint8*)Se_u_MacAddr,sizeof(Se_u_MacAddr)},//mac��Ϣ
	{EepromCfg_IntEE,EEPROM_SEC6_START_ADDR,EE_TOKEN_INFO_ADDR,(uint8*)Vetcp_client_u_token,sizeof(Vetcp_client_u_token)}//token��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC10_START_ADDR,EE_BLIST_NO1_ADDR,(uint8*)CardNumCache[0].CardNumArray[0].CardNum},//������1��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC11_START_ADDR,EE_BLIST_NO2_ADDR,(uint8*)CardNumCache[1].CardNumArray[0].CardNum},//������2��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC12_START_ADDR,EE_BLIST_NO3_ADDR,(uint8*)CardNumCache[2].CardNumArray[0].CardNum},//������3��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC13_START_ADDR,EE_BLIST_NO4_ADDR,(uint8*)CardNumCache[3].CardNumArray[0].CardNum},//������4��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC14_START_ADDR,EE_BLIST_NO5_ADDR,(uint8*)CardNumCache[4].CardNumArray[0].CardNum},//������5��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC15_START_ADDR,EE_BLIST_NO6_ADDR,(uint8*)CardNumCache[5].CardNumArray[0].CardNum},//������6��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC16_START_ADDR,EE_BLIST_NO7_ADDR,(uint8*)CardNumCache[6].CardNumArray[0].CardNum},//������7��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC17_START_ADDR,EE_BLIST_NO8_ADDR,(uint8*)CardNumCache[7].CardNumArray[0].CardNum},//������8��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC18_START_ADDR,EE_BLIST_NO9_ADDR,(uint8*)CardNumCache[8].CardNumArray[0].CardNum},//������9��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC19_START_ADDR,EE_BLIST_NO10_ADDR,(uint8*)CardNumCache[9].CardNumArray[0].CardNum},//������10��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC2_START_ADDR,EE_BLIST_TIMESTAMP_PAGE_ADDR,(uint8*)BListPull.token},//������ʱ�����ҳ����Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC6_START_ADDR,EE_CARD_SET_BACKUPONE,(uint8*)sminfo.community_id},//��Ƭ������Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC7_START_ADDR,EE_CARD_SET_BACKUPTWO,(uint8*)sminfo.community_id},//��Ƭ������Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC3_START_ADDR,EE_MAC_INFO_BACKUPONE,(uint8*)Se_u_MacAddr},//mac��Ϣ
	//{EepromCfg_ExtEE,EEPROM_SEC4_START_ADDR,EE_MAC_INFO_BACKUPTWO,(uint8*)Se_u_MacAddr},//mac��Ϣ
};

const uint8  CaEepromCfg_CheckSumObj[EE_CHECKSUM_OBJ_NUM] = /*��Ҫ������У��flash�е�������ȷ�Ե����ݶ���*/
{
	EepromCfg_CardInfo,/*ĸ��������Ϣ*/
	EepromCfg_MacInfo, /*mac��ַ��Ϣ*/
	//EepromCfg_CardInfoBkUpOne,/*ĸ��������Ϣ,����*/
	//EepromCfg_CardInfoBkUpTwo,/*ĸ��������Ϣ,����*/
	//EepromCfg_MacInfoBkUpOne, /*mac��ַ��Ϣ,����*/
	//EepromCfg_MacInfoBkUpTwo /*mac��ַ��Ϣ,����*/
};

const uint8  CaEepromCfg_ObjDtLng[EE_CHECKSUM_OBJ_NUM] = /*��Ҫ������У��flash�е�������ȷ�Ե����ݶ������ݳ���*/
{
	sizeof(sminfo),/*ĸ��������Ϣ*/
	sizeof(Se_u_MacAddr), /*mac��ַ��Ϣ*/
	sizeof(sminfo),/*ĸ��������Ϣ,����*/
	sizeof(sminfo),/*ĸ��������Ϣ,����*/
	sizeof(Se_u_MacAddr), /*mac��ַ��Ϣ,����*/
	sizeof(Se_u_MacAddr) /*mac��ַ��Ϣ,����*/
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
	W25QXX_Write_SR(0x00);//CMP=0ʱ�����W25Q128 16M memoryд����
	W25QXX_Write_Enable(); //EEPROM�������
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
	W25QXX_Write_SR(0x1c);//CMP=0ʱ������W25Q128 16M memoryд����
	W25QXX_Write_Disable();//EEPROM���ϱ���
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
	SPI_FLASH_BufferWrite(&Data,Address,1);
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
	SPI_FLASH_SectorErase(Le_u_Addr);
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
	if((0U == GetAudioIO_u_PlaySt()) && (0U == SPI_FLASH_WrInProgStatus()))
	{
		ret = 1U;
	}
	return ret;
}
