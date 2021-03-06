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
static uint8  SeMemIf_u_WrFlashFlag = 0U;//片上flash数据更新标志
static uint8  SeMemIf_u_WrFlashBusy = 0U;//写片上flash忙标志
static uint16 SeMemIf_w_Lng[EE_OBJECT_NUM];//数据长度
static uint8  SeMemIf_u_VildFlag[EE_OBJECT_NUM];//数据有效性标志
#ifdef EE_DEBUG
static uint8 SeMemIf_u_RdFlag = 0U;
#endif

static uint32  SeMemIf_dw_Timer = 0U;
static uint8  SeMemIf_u_CheckSumObj = 0U;//索引
static uint8  SeMemIf_u_DataCache[100U] = {0U};//数据缓存
/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static uint8 GetMemIf_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
static void MemIf_EraseFlash(uint32_t FLASH_Sector);
static uint8 MemIf_u_wrFlash(void);

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
	uint8 Le_u_i;
	for(Le_u_i = 0U;Le_u_i < EE_OBJECT_NUM;Le_u_i++)
	{
		SeMemIf_u_EESt[Le_u_i] = EE_IDLE;
		SeMemIf_u_WrEEFlag[Le_u_i] = 0U;
		SeMemIf_u_VildFlag[Le_u_i] = 0U;//数据无效
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
*函数名：TskMemIf_MainFunction

*形  参：void

*返回值：void

*描  述：

*备  注：
******************************************************/
void TskMemIf_MainFunction(void)
{
#ifdef MemIf_IntEE
	if((1U == SeMemIf_u_WrFlashFlag) && (0U == GetAudioIO_u_PlaySt()))
	{
		SeMemIf_u_WrFlashBusy = 1U;
		/* 擦除flash */
		MemIf_EraseFlash(FLASH_Sector_4);//写之前先擦除。片上flash，用于存储母卡配置，mac地址等信息
		USART_PRINTF_S("\r\n擦除片上flash扇区完成\r\n");
		/* 写flash */
		USART_PRINTF_S("\r\n写数据到片上flash扇区\r\n");
		(void)MemIf_u_wrFlash();
		SeMemIf_u_WrFlashFlag = 0U;
		SeMemIf_u_WrFlashBusy = 0U;
	}
#else
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
						LeMemIfCfg_u_Xor ^= Le_u_ptr[Le_w_Lng];
					}
					LeMemIfCfg_u_Xor = (~LeMemIfCfg_u_Xor);
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1),(uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*有效数据长度信息高Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2),(uint8)SeMemIf_w_Lng[Le_u_Obj]);/*有效数据长度信息低Byte*/
					MemIfCfg_FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),LeMemIfCfg_u_Xor);/*写校验和*/
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
	
	if(SeMemIf_dw_Timer >= (60000U/(MemIf_SCHEDULING_CYCLE/5U)))//大概5min
	{
		if(1U == MemIfCfg_FLASH_IDLE())//空闲
		{
			SeMemIf_dw_Timer = 0U;
			Le_u_Xor = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]].SecAddr));//读取扇区数据校验和
			Le_w_Lng = CaEepromCfg_ObjDtLng[SeMemIf_u_CheckSumObj];
			if(Le_w_Lng <= 100U)
			{
				(void)MemIf_ReadEE(CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj],SeMemIf_u_DataCache, \
								   Le_w_Lng);//读取flash数据
				if(Le_u_Xor != GetMemIf_u_XOR(SeMemIf_u_DataCache,Le_w_Lng))
				{//校验未通过，说明数据损坏,重新写数据到对应flash
					USART_PRINTF_D("数据对象%d数据损坏\n",CaEepromCfg_CheckSumObj[SeMemIf_u_CheckSumObj]);
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
				USART_PRINTF_S("数据长度超出定义的最大长度");
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
*函数名：MemIf_WriteEE

*形  参：

*返回值：

*描  述：写eeprom

*备  注：
******************************************************/
uint8 MemIf_WriteEE(uint8 Le_u_Object,void* Le_u_Data,uint16 Le_w_Lng)
{
	uint8 Le_u_ok = 0U;
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{
		case EepromCfg_IntEE:/*写片上eeprom*/
		{
			SeMemIf_u_WrFlashFlag = 1U;
			SeMemIf_u_VildFlag[Le_u_Object] = 1U;
			Le_u_ok = 1U;	
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

*返回值：数据校验和

*描  述：读eeprom

*备  注：
******************************************************/
uint8 MemIf_ReadEE(uint8 Le_u_Object,uint8* Le_u_Data,uint16 Le_w_Lng)
{
	//uint8 Le_u_ok = 0U;
	uint16 Le_w_i = 0U;
	uint32_t FLASH_addr;
	uint8 Le_u_Xor = 0U;	
	
	switch(CaEepromCfg_Conf[Le_u_Object].Space)
	{

		case EepromCfg_IntEE:/*读片上eeprom*/
		{
			FLASH_addr = CaEepromCfg_Conf[Le_u_Object].DtAddr;
			__disable_irq() ; //关闭总中断
			for(Le_w_i=0; Le_w_i < CaEepromCfg_Conf[Le_u_Object].Lng; Le_w_i++)
			{
				Le_u_Data[Le_w_i] = *(__IO uint8*)FLASH_addr;//读FLASH中的数据，直接给出地址就行了。跟从内存中读数据一样
				Le_u_Xor ^= Le_u_Data[Le_w_i];
				FLASH_addr++;
			}
			Le_u_Xor = (uint8)((~Le_u_Xor) + 0xAA);
			__enable_irq() ; //打开总中断
			//Le_u_ok = 1U;
		}
		break;
		case EepromCfg_ExtEE:/*读片外eeprom*/
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
*函数名：GetMemIf_u_DtVild

*形  参：

*返回值：

*描  述：获取扇区数据有效性

*备  注：
******************************************************/
uint8 GetMemIf_u_DtVild(uint8 Obj)
{
	uint8 VildFlag = 0U;
	VildFlag = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr + 3));//读取扇区100数据有效性标志（存放已注销卡卡号信息）
	return VildFlag;
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

/******************************************************
*函数名：GetMemIf_u_CheckSum

*形  参：

*返回值：

*描  述：获取扇区数据校验和

*备  注：
******************************************************/
uint8 GetMemIf_u_CheckSum(uint8 Obj)
{
	uint8 Le_u_Xor = 0U;
	Le_u_Xor = MemIfCfg_FLASH_ReadByte((uint32)(CaEepromCfg_Conf[Obj].SecAddr));
	return Le_u_Xor;
}


/*
	计时器
*/
void MemIf_Timer(void)
{
	SeMemIf_dw_Timer++;
}


/******************************************************
*函数名：GetMemIf_u_XOR

*形  参：

*返回值：

*描  述：异或取反校验

*备  注：
******************************************************/
static uint8 GetMemIf_u_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng)
{
	uint16 Le_w_i;
	uint8 Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return Le_u_Xor;
}



/*
//擦除flash扇区
*/
static void MemIf_EraseFlash(uint32_t FLASH_Sector)
{
	__disable_irq() ; //关闭总中断
	//IWDG_Feed();//喂狗
	FLASH_Unlock();
	//下面这个清flash的状态标志很重要，如果没有清可能会导致擦除flash失败或者不能擦除
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	(void)FLASH_EraseSector(FLASH_Sector,VoltageRange_3);
	FLASH_Lock();
	//IWDG_Feed();//喂狗
	__enable_irq() ; //打开总中断
}


/*
//写flash
*/
static uint8 MemIf_u_wrFlash(void)
{
	uint16_t Le_w_i;
	uint8 Le_u_Obj;
	uint8* Le_u_ptr;
	uint32 FLASH_addr;
	uint8 LeMemIfCfg_u_Xor = 0U;
	__disable_irq() ; //关闭总中断
	FLASH_Unlock();
	for(Le_u_Obj = 0U;Le_u_Obj < EE_OBJECT_NUM;Le_u_Obj++)
	{
		//USART_PRINTF_S("\r\n");
		Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
		for(Le_w_i =0U;Le_w_i < CaEepromCfg_Conf[Le_u_Obj].Lng;Le_w_i++)//将数据写入EEPROM中
		{
			FLASH_ProgramByte((CaEepromCfg_Conf[Le_u_Obj].DtAddr + Le_w_i), Le_u_ptr[Le_w_i]);	
			//USART_PRINTF_D("%x ",Le_u_ptr[Le_w_i]);
			LeMemIfCfg_u_Xor ^= Le_u_ptr[Le_w_i];
		}
		LeMemIfCfg_u_Xor = (~LeMemIfCfg_u_Xor);
		LeMemIfCfg_u_Xor = (uint8)(LeMemIfCfg_u_Xor + 0xAA);
		//USART_PRINTF_D("%x ",LeMemIfCfg_u_Xor);
		FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 1),(uint8)(SeMemIf_w_Lng[Le_u_Obj] >> 8U));/*有效数据长度信息高Byte*/
		FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 2),(uint8)SeMemIf_w_Lng[Le_u_Obj]);/*有效数据长度信息低Byte*/
		if(SeMemIf_u_VildFlag[Le_u_Obj] == 1U)
		{
			//USART_PRINTF_D("\r\nLe_u_Obj == %d\r\n",Le_u_Obj);
			FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr + 3),STD_ACTIVE);/*有效性标志Byte*/
		}
		//USART_PRINTF_D("\r\nLeMemIfCfg_u_Xor == %d\r\n",LeMemIfCfg_u_Xor);
		FLASH_ProgramByte((uint32)(CaEepromCfg_Conf[Le_u_Obj].SecAddr),LeMemIfCfg_u_Xor);/*写校验和*/
		
		/* 校验数据是否正确写入 */
		FLASH_addr = CaEepromCfg_Conf[Le_u_Obj].DtAddr;
		Le_u_ptr = CaEepromCfg_Conf[Le_u_Obj].Data;
		//__disable_irq() ; //关闭总中断
		for(Le_w_i=0;Le_w_i < CaEepromCfg_Conf[Le_u_Obj].Lng;Le_w_i++)
		{
			if(Le_u_ptr[Le_w_i] != *(__IO uint8*)FLASH_addr)//读FLASH中的数据，直接给出地址就行了。跟从内存中读数据一样
			{
				USART_PRINTF_S("\r\n数据写入出错\r\n");
				__enable_irq() ; //打开总中断
				FLASH_Lock();
				return 0;
			}
			FLASH_addr++;
		}
		
		if(LeMemIfCfg_u_Xor != *(__IO uint8*)CaEepromCfg_Conf[Le_u_Obj].SecAddr)
		{
			USART_PRINTF_S("\r\n校验和写入出错\r\n");
			__enable_irq() ; //打开总中断
			FLASH_Lock();
			return 0;
		}
		LeMemIfCfg_u_Xor = 0u;
	}
	USART_PRINTF_S("\r\n写片上flash完成\r\n");
	__enable_irq() ; //打开总中断
	FLASH_Lock();
	return 1;
	//__enable_irq() ; //打开总中断
}
