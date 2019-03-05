/******************************************************
文件名：	MemIf.c

描述：		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "MemIf.h"


/*******************************************************
description： global variable definitions
*******************************************************/


/*******************************************************
description： static variable definitions
*******************************************************/
static uint8  SeMemIf_u_EESt[EE_OBJECT_NUM];//非易失性存储器当前状态
static uint8  SeMemIf_u_WrEEFlag[EE_OBJECT_NUM];//数据更新标志
static uint16 SeMemIf_w_Lng[EE_OBJECT_NUM];//数据长度
static uint8  SeMemIf_u_VildFlag[EE_OBJECT_NUM];//数据有效性标志
#ifdef EE_DEBUG
static uint8 SeMemIf_u_RdFlag = 0U;
#endif
/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：MemIf_Init

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void MemIf_Init(void)
{
	
}

/******************************************************
*函数名：TskMemIf_MainFunction

*形  参：void

*返回值：void

*描  述：

*备  注：
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
				if(1U == SeMemIf_u_WrEEFlag[Le_u_Obj])//数据更新
				{
					SeMemIf_u_EESt[Le_u_Obj] = EE_ERASE;
				}			
			}
			break;
			case EE_ERASE:
			{
				if(1U == MemIfCfg_FLASH_IDLE())//空闲，可擦除
				{
					MemIfCfg_FLASH_Unlock();//解除写保护,20180619 by liujian
					MemIfCfg_FLASH_SectorErase((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr));
					SeMemIf_u_EESt[Le_u_Obj] = EE_WR;
				}
			}
			break;
			case EE_WR:
			{
				if(1U == MemIfCfg_FLASH_IDLE())//空闲，可写
				{
					MemIfCfg_FLASH_Unlock();//解除写保护,20180619 by liujian
					Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
					for(Le_w_Lng =0U; Le_w_Lng < SeMemIf_w_Lng[Le_u_Obj]; Le_w_Lng++)//将数据写入EEPROM中
					{
						MemIfCfg_FLASH_ProgramByte((CaEepromCfg_Conf[Le_u_Obj].DtAddr + Le_w_Lng), Le_u_ptr[Le_w_Lng]);			
					}
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1),(uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*有效数据长度信息高Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2),(uint8)SeMemIf_w_Lng[Le_u_Obj]);/*有效数据长度信息低Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),EE_VALID_FIELD_VALUE);/*置位数据有效性标志位*/
					MemIfCfg_FLASH_Lock();//开启写保护,20180619 by liujian
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
*函数名：MemIf_WriteEE

*形  参：

*返回值：

*描  述：写eeprom或flash

*备  注：
******************************************************/
uint8 MemIf_WriteEE(uint8 Le_u_Object,uint32 Le_dw_Address,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	//uint8 Le_u_ValidFlag = 0U;
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{
		case EepromCfg_IntEE:/*写片上eeprom*/
		{
#if 1
			MemIfCfg_FLASH_Unlock();/*eeprom解锁保护*/
			for(Le_w_i=0; Le_w_i < Le_w_Lng; Le_w_i++)//将数据写入EEPROM中
			{
				FLASH_ProgramByte(Le_dw_Address, Le_u_Data[Le_w_i]);
				Le_dw_Address++;
			}
			Le_u_ok = 1U;
			MemIfCfg_FLASH_Lock();/*eeprom加锁保护*/
#endif
		}
		break;
		case EepromCfg_ExtEE:/*写片外eeprom*/
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
*函数名：MemIf_ReadEE

*形  参：

*返回值：

*描  述：读eeprom或flash

*备  注：
******************************************************/
uint8 MemIf_ReadEE(uint8 Le_u_Object,uint32 Le_dw_Address,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	//uint8 Le_u_ValidFlag = 0U;
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{

		case EepromCfg_IntEE:/*读片上eeprom*/
		{
#if 1			
			for(Le_w_i=0; Le_w_i < Le_w_Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = *(__IO uint8*)Le_dw_Address;//读FLASH中的数据，直接给出地址就行了。跟从内存中读数据一样
				Le_dw_Address++;			
				Le_u_ok = 1U;
			}
#endif
		}
		break;
		case EepromCfg_ExtEE:/*读片外eeprom*/
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
*函数名：MemIf_ReadEE

*形  参：

*返回值：

*描  述：读eeprom

*备  注：
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
*函数名：GetMemIf_u_EEVild

*形  参：

*返回值：

*描  述：获取数据有效性

*备  注：
******************************************************/
uint8 GetMemIf_u_EEVild(uint8 Object)
{
	return SeMemIf_u_VildFlag[Object];
}


/******************************************************
*函数名：SetMemIf_EEVild

*形  参：

*返回值：

*描  述：设置数据有效性

*备  注：
******************************************************/
void SetMemIf_EEVild(uint8 Object)
{
	SeMemIf_u_VildFlag[Object] = 1U;
}

/******************************************************
*函数名：GetMemIf_u_Idle

*形  参：

*返回值：

*描  述：获取flash操作空闲状态:0--忙；1--空闲

*备  注：
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
*函数名：GetMemIf_u_DtVild

*形  参：

*返回值：

*描  述：获取扇区数据有效性

*备  注：
******************************************************/
uint8 GetMemIf_u_DtVild(uint8 Obj)
{
	uint8 VildFlag = 0U;
	VildFlag = MemIfCfg_FLASH_ReadByte((uint32)CaEepromCfg_Conf[Obj].SecAddr);//读取扇区100数据有效性标志（存放已注销卡卡号信息）
	if(EE_VALID_FIELD_VALUE == VildFlag)//flash数据有效
	{
		return 1U;
	}
	return 0U;
}

/******************************************************
*函数名：GetMemIf_w_DtLng

*形  参：

*返回值：

*描  述：获取扇区有效数据长度

*备  注：
******************************************************/
uint16 GetMemIf_w_DtLng(uint8 Obj)
{
	uint16 LeMemIf_w_Lng = 0U;
	LeMemIf_w_Lng = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr + 1));
	LeMemIf_w_Lng = LeMemIf_w_Lng <<8U;
	LeMemIf_w_Lng = (LeMemIf_w_Lng | MemIfCfg_FLASH_ReadByte((uint32)CaEepromCfg_Conf[Obj].SecAddr +2));
	return LeMemIf_w_Lng;
}


