/******************************************************
�ļ�����	MemIf.c

������		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "MemIf.h"


/*******************************************************
description�� global variable definitions
*******************************************************/


/*******************************************************
description�� static variable definitions
*******************************************************/
#ifndef MemIf_IntEE
static uint8  SeMemIf_u_EESt[EE_OBJECT_NUM];//����ʧ�Դ洢����ǰ״̬
static uint8  SeMemIf_u_WrEEFlag[EE_OBJECT_NUM];//���ݸ��±�־
#endif
static uint8  SeMemIf_u_WrFlashBusy = 0U;//дƬ��flashæ��־
static uint16 SeMemIf_w_Lng[EE_OBJECT_NUM];//���ݳ���
static uint8  SeMemIf_u_VildFlag[EE_OBJECT_NUM];//������Ч�Ա�־
#ifdef EE_DEBUG
static uint8 SeMemIf_u_RdFlag = 0U;
#endif

static uint32  SeMemIf_dw_Timer = 0U;
#ifndef MemIf_IntEE
static uint8  SeMemIf_u_CheckSumObj = 0U;//����
static uint8  SeMemIf_u_DataCache[100U] = {0U};//���ݻ���
#endif
/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void MemIf_EraseFlash(uint32_t FLASH_Sector);
static uint8 MemIf_u_wrFlash(void);

/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������MemIf_Init

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void MemIf_Init(void)
{
	uint8 Le_u_i;
	for(Le_u_i = 0U;Le_u_i < EE_OBJECT_NUM;Le_u_i++)
	{
#ifndef MemIf_IntEE
		SeMemIf_u_EESt[Le_u_i] = EE_IDLE;
		SeMemIf_u_WrEEFlag[Le_u_i] = 0U;
#endif
		SeMemIf_u_VildFlag[Le_u_i] = 0U;//������Ч
#if 0
		MemIfCfg_FLASH_SectorErase((uint32)(CaEepromCfg_Conf[Le_u_i].SecAddr));
		while(0U != SPI_FLASH_WrInProgStatus());
		
		uint8 byte[2U];
		byte[0]  = W25QXX_ReadSR(W25X_ReadStatusReg);
		printf("W25X_ReadStatusReg:%d\n",byte[0]);
		byte[1]  = W25QXX_ReadSR(W25X_ReadStatusRegTwo);
		printf("W25X_ReadStatusRegTwo:%d\n",byte[1]);
		byte[1]  = W25QXX_ReadSR(W25X_ReadStatusRegThr);
		printf("W25X_ReadStatusRegThr:%d\n",byte[1]);
#endif
	}
}

/******************************************************
*��������TskMemIf_MainFunction

*��  �Σ�void

*����ֵ��void

*��  ����

*��  ע��
******************************************************/
void TskMemIf_MainFunction(void)
{
#ifndef MemIf_IntEE
	uint16 Le_w_Lng;
	uint8 Le_u_Obj;
	//uint8 Le_u_Object;
	uint8* Le_u_ptr;
	uint8 LeMemIfCfg_u_Xor = 0U;
	uint8 Le_u_Xor = 0U;
	
	for(Le_u_Obj = 0U;Le_u_Obj < EE_OBJECT_NUM;Le_u_Obj++)
	{
		switch(SeMemIf_u_EESt[Le_u_Obj])
		{
			case EE_IDLE:
			{
				if(1U == SeMemIf_u_WrEEFlag[Le_u_Obj])//���ݸ���
				{
					SeMemIf_u_EESt[Le_u_Obj] = EE_ERASE;
				}			
			}
			break;
			case EE_ERASE:
			{
				if(1U == MemIfCfg_FLASH_IDLE())//���У��ɲ���
				{
					MemIfCfg_FLASH_Unlock();//���д����,20180619 by liujian
					MemIfCfg_FLASH_SectorErase((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr));
					SeMemIf_u_EESt[Le_u_Obj] = EE_WR;
				}
			}
			break;
			case EE_WR:
			{
				if(1U == MemIfCfg_FLASH_IDLE())//���У���д
				{
					MemIfCfg_FLASH_Unlock();//���д����,20180619 by liujian
					Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
					for(Le_w_Lng =0U; Le_w_Lng < SeMemIf_w_Lng[Le_u_Obj]; Le_w_Lng++)//������д��EEPROM��
					{
						MemIfCfg_FLASH_ProgramByte((CaEepromCfg_Conf[Le_u_Obj].DtAddr + Le_w_Lng), Le_u_ptr[Le_w_Lng]);	
						LeMemIfCfg_u_Xor ^= Le_u_ptr[Le_w_Lng];
					}
					LeMemIfCfg_u_Xor = (~LeMemIfCfg_u_Xor);
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1),(uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*��Ч���ݳ�����Ϣ��Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2),(uint8)SeMemIf_w_Lng[Le_u_Obj]);/*��Ч���ݳ�����Ϣ��Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),LeMemIfCfg_u_Xor);/*дУ���*/
					MemIfCfg_FLASH_Lock();//����д����,20180619 by liujian
					SeMemIf_u_WrEEFlag[Le_u_Obj] = 0U;
					SeMemIf_u_EESt[Le_u_Obj] = EE_IDLE;
	#ifdef EE_DEBUG
					SeMemIf_u_RdFlag = 1U;
	#endif
				}			
			}
			break;
			default:
			break;
		}
	}
	
	if(SeMemIf_dw_Timer >= (60000U/(MemIf_SCHEDULING_CYCLE/5U)))//���5min
	{
		if(1U == MemIfCfg_FLASH_IDLE())//����
		{
			SeMemIf_dw_Timer = 0U;
			Le_u_Xor = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]].SecAddr));//��ȡ��������У���
			Le_w_Lng = CaEepromCfg_ObjDtLng[SeMemIf_u_CheckSumObj];
			if(Le_w_Lng <= 100U)
			{
				(void)MemIf_ReadEE(CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj],SeMemIf_u_DataCache, \
								   Le_w_Lng);//��ȡflash����
				if(Le_u_Xor != GetMemIf_u_XOR(SeMemIf_u_DataCache,Le_w_Lng))
				{//У��δͨ����˵��������,����д���ݵ���Ӧflash
					MEMIF_PRINTF_D("���ݶ���%d������\n",CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]);
					SeMemIf_u_WrEEFlag[CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]] = 1U;
					SeMemIf_w_Lng[CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]] = Le_w_Lng;
				}
				
				SeMemIf_u_CheckSumObj++;
				if(SeMemIf_u_CheckSumObj == EE_CHECKSUM_OBJ_NUM)
				{
					SeMemIf_u_CheckSumObj = 0U;
				}
			}
			else
			{
				MEMIF_PRINTF_S("���ݳ��ȳ����������󳤶�");
			}
		}
	}
	else
	{
		SeMemIf_dw_Timer++;
	}
#endif
}


/******************************************************
*��������MemIf_WriteEE

*��  �Σ�

*����ֵ��

*��  ����дeeprom

*��  ע��
******************************************************/
uint8 MemIf_WriteEE(uint8 Le_u_Object,void* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{
		case EepromCfg_IntEE:/*дƬ��eeprom*/
		{
			SeMemIf_u_WrFlashBusy = 1U;
			CaEepromCfg_Conf[Le_u_Object].Data = (char*)Le_u_Data;
			CaEepromCfg_Conf[Le_u_Object].Lng = (uint8)Le_w_Lng;
			SeMemIf_u_VildFlag[Le_u_Object] = 1U;
			/* ����flash */
			MemIf_EraseFlash(MEMIF_ERASE_SECTOR);//д֮ǰ�Ȳ�����Ƭ��flash�����ڴ洢ĸ�����ã�mac��ַ����Ϣ
			/* дflash */
			MEMIF_PRINTF_S("\r\n����Ƭ��flash�������,��ʼд���ݵ�Ƭ��flash����\r\n");
			(void)MemIf_u_wrFlash();
			SeMemIf_u_WrFlashBusy = 0U;
			
			Le_u_ok = 1U;	
		}
		break;
		case EepromCfg_ExtEE:/*дƬ��eeprom*/
		{
			#ifndef MemIf_IntEE
			SeMemIf_u_WrEEFlag[Le_u_Object] = 1U;
			#endif
			SeMemIf_w_Lng[Le_u_Object] = Le_w_Lng;
			Le_u_ok = 1U;
		}
		break;
		default:
		break;
	}
	
	return  Le_u_ok;	
}


/******************************************************
*��������MemIf_ReadEE

*��  �Σ�

*����ֵ������У���

*��  ������eeprom

*��  ע��
******************************************************/
uint8 MemIf_ReadEE(uint8 Le_u_Object,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	//uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	uint32_t FLASH_addr;
	uint8 Le_u_Xor = 0U;	
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{

		case EepromCfg_IntEE:/*��Ƭ��eeprom*/
		{
			CaEepromCfg_Conf[Le_u_Object].Data = (char*)Le_u_Data;
			CaEepromCfg_Conf[Le_u_Object].Lng = (uint8)Le_w_Lng;
			FLASH_addr = CaEepromCfg_Conf[Le_u_Object].DtAddr;
			__disable_irq() ; //�ر����ж�
			for(Le_w_i=0; Le_w_i < CaEepromCfg_Conf[Le_u_Object].Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = *(__IO uint8*)FLASH_addr;//��FLASH�е����ݣ�ֱ�Ӹ�����ַ�����ˡ������ڴ��ж�����һ��
				Le_u_Xor ^= Le_u_Data[Le_w_i];
				FLASH_addr++;
			}
			Le_u_Xor = (uint8)((~Le_u_Xor) + 0xAA);
			__enable_irq() ; //�����ж�
			//Le_u_ok = 1U;
		}
		break;
		case EepromCfg_ExtEE:/*��Ƭ��eeprom*/
		{
			for(Le_w_i=0; Le_w_i < CaEepromCfg_Conf[Le_u_Object].Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = MemIfCfg_FLASH_ReadByte(CaEepromCfg_Conf[Le_u_Object].DtAddr + Le_w_i);	
				Le_u_Xor ^= Le_u_Data[Le_w_i];
			}
			Le_u_Xor = (uint8)((~Le_u_Xor) + 0xAA);
			//Le_u_ok = 1U;
		}
		break;
		default:
		break;
	}
	return Le_u_Xor;
}


/******************************************************
*��������GetMemIf_u_EEVild

*��  �Σ�

*����ֵ��

*��  ������ȡ������Ч��

*��  ע��
******************************************************/
uint8 GetMemIf_u_EEVild(uint8 Object)
{
	return SeMemIf_u_VildFlag[Object];
}


/******************************************************
*��������SetMemIf_EEVild

*��  �Σ�

*����ֵ��

*��  ��������������Ч��

*��  ע��
******************************************************/
void SetMemIf_EEVild(uint8 Object)
{
	SeMemIf_u_VildFlag[Object] = 1U;
}

/******************************************************
*��������GetMemIf_u_Idle

*��  �Σ�

*����ֵ��

*��  ������ȡflash��������״̬:0--æ��1--����

*��  ע��
******************************************************/
uint8 GetMemIf_u_Idle(void)
{
#ifdef MemIf_IntEE
	if(1U == SeMemIf_u_WrFlashBusy)
	{
		return 0U;
	}
	else
	{
		return 1U;
	}
#else
	uint8 ret = 1U;
	uint8 Le_u_i;
	for(Le_u_i = 0U;Le_u_i < EE_OBJECT_NUM;Le_u_i++)
	{
		if(EE_IDLE != SeMemIf_u_EESt[Le_u_i])
		{
			 ret = 0U;
			 break;
		}		
	}
	return ret;
#endif
}

/******************************************************
*��������GetMemIf_u_DtVild

*��  �Σ�

*����ֵ��

*��  ������ȡ����������Ч��

*��  ע��
******************************************************/
uint8 GetMemIf_u_DtVild(uint8 Obj)
{
	uint8 VildFlag = 0U;
	VildFlag = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr + 3));//��ȡ����100������Ч�Ա�־�������ע����������Ϣ��
	return VildFlag;
}

/******************************************************
*��������GetMemIf_w_DtLng

*��  �Σ�

*����ֵ��

*��  ������ȡ������Ч���ݳ���

*��  ע��
******************************************************/
uint16 GetMemIf_w_DtLng(uint8 Obj)
{
	uint16 LeMemIf_w_Lng = 0U;
	LeMemIf_w_Lng = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr + 1));
	LeMemIf_w_Lng = LeMemIf_w_Lng <<8U;
	LeMemIf_w_Lng = (LeMemIf_w_Lng | MemIfCfg_FLASH_ReadByte((uint32)CaEepromCfg_Conf[Obj].SecAddr +2));
	return LeMemIf_w_Lng;
}

/******************************************************
*��������GetMemIf_u_CheckSum

*��  �Σ�

*����ֵ��

*��  ������ȡ��������У���

*��  ע��
******************************************************/
uint8 GetMemIf_u_CheckSum(uint8 Obj)
{
	uint8 Le_u_Xor = 0U;
	Le_u_Xor = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr));
	return Le_u_Xor;
}


/*
	��ʱ��
*/
void MemIf_Timer(void)
{
	SeMemIf_dw_Timer++;
}



/*
//����flash����
*/
static void MemIf_EraseFlash(uint32_t FLASH_Sector)
{
#ifdef MEMIF_HAL
    uint32_t SectorError;
    FLASH_EraseInitTypeDef pEraseInit;
	__disable_irq() ; //�ر����ж�
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock(); 
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
						   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	
    pEraseInit.TypeErase = TYPEERASE_SECTORS;
    pEraseInit.Sector = FLASH_Sector;
    pEraseInit.NbSectors = 1 ;
    pEraseInit.VoltageRange = VOLTAGE_RANGE_3;
	
    if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
    {
        /* Error occurred while page erase */
        MEMIF_PRINTF_S("\r\nError occurred while page erase\r\n");
    }
	HAL_FLASH_Lock();
	__enable_irq() ; //�����ж�
#else
	__disable_irq() ; //�ر����ж�
	//IWDG_Feed();//ι��
	FLASH_Unlock();
	//���������flash��״̬��־����Ҫ�����û������ܻᵼ�²���flashʧ�ܻ��߲��ܲ���
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	(void)FLASH_EraseSector(FLASH_Sector,VoltageRange_3);
	FLASH_Lock();
	//IWDG_Feed();//ι��
	__enable_irq() ; //�����ж�
#endif
}


/*
//дflash
*/
static uint8 MemIf_u_wrFlash()
{
	uint16_t Le_w_i;
	uint8 Le_u_Obj;
	char* Le_u_ptr = STD_NULL_PTR;
	uint32 FLASH_addr;
	uint8 LeMemIfCfg_u_Xor = 0U;
#ifdef MEMIF_HAL
#define FLASH_UNLOCK() 	HAL_FLASH_Unlock()
#define FLASH_LOCK() 	HAL_FLASH_Lock()
#define FLASH_PROGRAMBYTE(addr,data) 	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,addr,data)	
#else
#define FLASH_UNLOCK() 	HAL_FLASH_Unlock()
#define FLASH_LOCK() 	FLASH_Lock()	
#define FLASH_PROGRAMBYTE(addr,data) 	FLASH_ProgramByte(addr,data)	
#endif

	__disable_irq() ; //�ر����ж�
	FLASH_UNLOCK();
	for(Le_u_Obj = 0U;Le_u_Obj < EE_OBJECT_NUM;Le_u_Obj++)
	{
		MEMIF_PRINTF_D("\nNow is writing %s",CaEepromCfg_Conf[Le_u_Obj].description);
		Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
		if(STD_NULL_PTR == Le_u_ptr) 
		{
			MEMIF_PRINTF_D("\n%s is NULL",CaEepromCfg_Conf[Le_u_Obj].description);
			continue;
		}
		for(Le_w_i =0U;Le_w_i < CaEepromCfg_Conf[Le_u_Obj].Lng;Le_w_i++)//������д��EEPROM��
		{
			FLASH_PROGRAMBYTE((CaEepromCfg_Conf[Le_u_Obj].DtAddr + Le_w_i), Le_u_ptr[Le_w_i]);	
			LeMemIfCfg_u_Xor ^= Le_u_ptr[Le_w_i];
		}
		LeMemIfCfg_u_Xor = (~LeMemIfCfg_u_Xor);
		LeMemIfCfg_u_Xor = (uint8)(LeMemIfCfg_u_Xor + 0xAA);
		FLASH_PROGRAMBYTE((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1), \
						  (uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*��Ч���ݳ�����Ϣ��Byte*/
		FLASH_PROGRAMBYTE((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2), \
						  (uint8)SeMemIf_w_Lng[Le_u_Obj]);/*��Ч���ݳ�����Ϣ��Byte*/
		if(SeMemIf_u_VildFlag[Le_u_Obj] == 1U)
		{
			FLASH_PROGRAMBYTE((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 3),STD_ACTIVE);/*��Ч�Ա�־Byte*/
		}
		FLASH_PROGRAMBYTE((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),LeMemIfCfg_u_Xor);/*дУ���*/
		MEMIF_PRINTF_D("\nthe checksum of %s is ",CaEepromCfg_Conf[Le_u_Obj].description);
		MEMIF_PRINTF_D("%d\n",LeMemIfCfg_u_Xor);
		/* У�������Ƿ���ȷд�� */
		MEMIF_PRINTF_D("\ncheck %s",CaEepromCfg_Conf[Le_u_Obj].description);
		FLASH_addr = CaEepromCfg_Conf[Le_u_Obj].DtAddr;
		//Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
		for(Le_w_i=0;Le_w_i < CaEepromCfg_Conf[Le_u_Obj].Lng;Le_w_i++)
		{
			if(Le_u_ptr[Le_w_i] != *(__IO uint8*)FLASH_addr)//��FLASH�е����ݣ�ֱ�Ӹ�����ַ�����ˡ������ڴ��ж�����һ��
			{
				MEMIF_PRINTF_D("\nthe data of %s is wrong",CaEepromCfg_Conf[Le_u_Obj].description);
				__enable_irq() ; //�����ж�
				FLASH_LOCK();
				return 0;
			}
			FLASH_addr++;
		}
		
		if(LeMemIfCfg_u_Xor != *(__IO uint8*)CaEepromCfg_Conf[Le_u_Obj].SecAddr)
		{
			MEMIF_PRINTF_D("\nthe checksum of %s is wrong",CaEepromCfg_Conf[Le_u_Obj].description);
			__enable_irq() ; //�����ж�
			FLASH_LOCK();
			return 0;
		}
		LeMemIfCfg_u_Xor = 0u;
		MEMIF_PRINTF_D("\n%s was written successfully\n",CaEepromCfg_Conf[Le_u_Obj].description);
	}
	__enable_irq() ; //�����ж�
	FLASH_LOCK();
	return 1;
}
