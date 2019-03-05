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
static uint8  SeMemIf_u_EESt[EE_OBJECT_NUM];//����ʧ�Դ洢����ǰ״̬
static uint8  SeMemIf_u_WrEEFlag[EE_OBJECT_NUM];//���ݸ��±�־
static uint16 SeMemIf_w_Lng[EE_OBJECT_NUM];//���ݳ���
static uint8  SeMemIf_u_VildFlag[EE_OBJECT_NUM];//������Ч�Ա�־
#ifdef EE_DEBUG
static uint8 SeMemIf_u_RdFlag = 0U;
#endif
/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


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
	uint16 Le_w_Lng;
	uint8 Le_u_Obj;
	//uint8 Le_u_Object;
	uint8* Le_u_ptr;
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
					}
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1),(uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*��Ч���ݳ�����Ϣ��Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2),(uint8)SeMemIf_w_Lng[Le_u_Obj]);/*��Ч���ݳ�����Ϣ��Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),EE_VALID_FIELD_VALUE);/*��λ������Ч�Ա�־λ*/
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
}


/******************************************************
*��������MemIf_WriteEE

*��  �Σ�

*����ֵ��

*��  ����дeeprom��flash

*��  ע��
******************************************************/
uint8 MemIf_WriteEE(uint8 Le_u_Object,uint32 Le_dw_Address,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	//uint8 Le_u_ValidFlag = 0U;
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{
		case EepromCfg_IntEE:/*дƬ��eeprom*/
		{
#if 1
			MemIfCfg_FLASH_Unlock();/*eeprom��������*/
			for(Le_w_i=0; Le_w_i < Le_w_Lng; Le_w_i++)//������д��EEPROM��
			{
				FLASH_ProgramByte(Le_dw_Address, Le_u_Data[Le_w_i]);
				Le_dw_Address++;
			}
			Le_u_ok = 1U;
			MemIfCfg_FLASH_Lock();/*eeprom��������*/
#endif
		}
		break;
		case EepromCfg_ExtEE:/*дƬ��eeprom*/
		{
			SeMemIf_u_WrEEFlag[Le_u_Object] = 1U;
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

*����ֵ��

*��  ������eeprom��flash

*��  ע��
******************************************************/
uint8 MemIf_ReadEE(uint8 Le_u_Object,uint32 Le_dw_Address,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	//uint8 Le_u_ValidFlag = 0U;
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{

		case EepromCfg_IntEE:/*��Ƭ��eeprom*/
		{
#if 1			
			for(Le_w_i=0; Le_w_i < Le_w_Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = *(__IO uint8*)Le_dw_Address;//��FLASH�е����ݣ�ֱ�Ӹ�����ַ�����ˡ������ڴ��ж�����һ��
				Le_dw_Address++;			
				Le_u_ok = 1U;
			}
#endif
		}
		break;
		case EepromCfg_ExtEE:/*��Ƭ��eeprom*/
		{
			for(Le_w_i=0; Le_w_i < Le_w_Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = MemIfCfg_FLASH_ReadByte(CaEepromCfg_Conf[Le_u_Object].DtAddr + Le_w_i);			
			}
			Le_u_ok = 1U;
		}
		break;
		default:
		break;
	}
	return Le_u_ok;
}

#ifdef EE_DEBUG
/******************************************************
*��������MemIf_ReadEE

*��  �Σ�

*����ֵ��

*��  ������eeprom

*��  ע��
******************************************************/
void MemIf_TestEE(void)
{
	static struct comm_info sminfo_test;
	static uint8 VildFlag;
	if((0U == SeMemIf_u_RdFlag)&& (0U == SeMemIf_u_WrEEFlag))
	{
		sminfo.community_id[0] = 0x01;
		sminfo.community_id[15] = 0x16;
		sminfo.build_num[0] = 0x01;
		sminfo.build_num[15] = 0x16;
		sminfo.cell_num[0] = 0x01;
		sminfo.cell_num[15] = 0x16;
		SeMemIf_u_WrEEFlag = 1U;
	}
	else
	{
		if(1U == SeMemIf_u_RdFlag)
		{
			MemIf_ReadEE(EepromCfg_CardInfo,sminfo_test.community_id,sizeof(sminfo_test));
			VildFlag = MemIfCfg_FLASH_ReadByte(EEPROM_START_ADDR);	
			SeMemIf_u_RdFlag = 2U;
		}
		 
	}
}
#endif

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
	VildFlag = MemIfCfg_FLASH_ReadByte((uint32)CaEepromCfg_Conf[Obj].SecAddr);//��ȡ����100������Ч�Ա�־�������ע����������Ϣ��
	if(EE_VALID_FIELD_VALUE == VildFlag)//flash������Ч
	{
		return 1U;
	}
	return 0U;
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


