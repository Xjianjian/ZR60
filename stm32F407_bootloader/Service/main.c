/**
  ************************************************
  * @file    main.c
  * @author  liujian
  * @version V1.0
  * @date    2018-3-28
  * @brief
  ************************************************
  * @attention
  *
  * 平台:秉火  STM32 F407 开发板  
  *
  *
  *
  ************************************************
*/

#include "Include.h"


/*************************************************
  *        				定义变量
**************************************************/
/* Private typedef */
typedef  void (*pFunction)(void);


//uint16 Vemain_w_5msTskTimer = 0U;
static Include_UpdataInfoStruct Se_h_UpdataInfo;
pFunction Jump_To_Application;
uint32_t JumpAddress;

/*************************************************
  *        				定义静态函数
**************************************************/
static void EraseFlashSector(uint16_t Le_w_sector);
static uint32 GetCheckSum_dw_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
static void JumpToApp(void);

/*************************************************
  *        				main函数
**************************************************/
int main(void)
{
	uint32 Le_dw_i;
	uint8 Le_u_dt;
	uint32 Le_dw_CheckSum;/*升级区数据校验和*/
	uint32 Le_dw_InitDlyTime = 0xffffff;//延时时间
	/*******************
	 *	初始化...	
	********************/
	while(Le_dw_InitDlyTime--);
#ifdef UART_DEBUG
	Debug_USART_Config();
#endif
	MemIf_Init();
	printf("\n运行bootloader...\n");
	if(1U == MemIf_ReadEE(EepromCfg_UpDateInfo,ADDR_FLASH_SECTOR_2,	\
						  (uint8*)(&Se_h_UpdataInfo),sizeof(Se_h_UpdataInfo)))
	{
#ifdef UART_DEBUG
		printf("\n%x,%x,%d,%x\n",Se_h_UpdataInfo.DtFirstAddr,Se_h_UpdataInfo.DtendAddr \
			  ,Se_h_UpdataInfo.Lng,Se_h_UpdataInfo.CheckSum);
#endif
	}
	/******初始化 end****/	
	while (1)
	{ 	
		if(Se_h_UpdataInfo.Xor == GetCheckSum_dw_XOR((uint8*)(&(Se_h_UpdataInfo.DtFirstAddr)), \
				(sizeof(Se_h_UpdataInfo)-4U)))
		{//升级数据区校验和通过	
			Se_h_UpdataInfo.CheckSum = ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -4)) << 24) & 0xff000000;/*下载区数据校验和,下载的二进制文件的最后4字节为校验和*/
			Se_h_UpdataInfo.CheckSum = Se_h_UpdataInfo.CheckSum | ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -3)) << 16);/*下载区数据校验和*/
			Se_h_UpdataInfo.CheckSum = Se_h_UpdataInfo.CheckSum | ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -2)) << 8);/*下载区数据校验和*/
			Se_h_UpdataInfo.CheckSum |= *((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -1);/*下载区数据校验和*/
			Le_dw_CheckSum = crc32(0xFFFFFFFF,(__IO uint8*)Se_h_UpdataInfo.DtFirstAddr,(Se_h_UpdataInfo.Lng -4));//根据下载区数据计算得到的校验和
			printf("\nSe_h_UpdataInfo.CheckSum = %x\n",Se_h_UpdataInfo.CheckSum);
			printf("\nLe_dw_CheckSum = %x\n",Le_dw_CheckSum);
			if(Se_h_UpdataInfo.CheckSum == Le_dw_CheckSum)
			{/*计算下载区数据crc32校验和通过*/
				/* FLASH_Sector_6、FLASH_Sector_7、FLASH_Sector_8为用户代码运行区 */
				EraseFlashSector(FLASH_Sector_6);
				EraseFlashSector(FLASH_Sector_7);
				EraseFlashSector(FLASH_Sector_8);
				
				/* 拷贝下载区代码数据到用户代码运行区 */
				for(Le_dw_i = 0U;Le_dw_i < (Se_h_UpdataInfo.Lng - 4U);Le_dw_i++)
				{
					Le_u_dt = *(__IO uint8*)Se_h_UpdataInfo.DtFirstAddr;
					FLASH_Unlock();
					FLASH_ProgramByte((ADDR_FLASH_SECTOR_6+Le_dw_i),Le_u_dt);
					FLASH_Lock();
					Se_h_UpdataInfo.DtFirstAddr++;
				}
				
				/* 升级完成，擦除升级区和用户代码下载区数据 */
				EraseFlashSector(FLASH_Sector_2);
				EraseFlashSector(FLASH_Sector_9);
				EraseFlashSector(FLASH_Sector_10);
				EraseFlashSector(FLASH_Sector_11);
				
				printf("\n升级完成\n");
				JumpToApp();//跳转到App
			}
			else
			{
				printf("\ncrc32校验NOK\n");
				/* crc32校验NOK */
				EraseFlashSector(FLASH_Sector_2);
				
				JumpToApp();//跳转到App
			}
		}
		else
		{
			printf("\n升级信息区数据校验NOK\n");
			JumpToApp();//跳转到App
		}			
	}
}

/*
	跳转到用户App
*/
static void JumpToApp(void)
{
	if (((*(__IO uint32_t*)USER_FLASH_FIRST_PAGE_ADDRESS) & 0x2FFE0000 ) == 0x20000000)//判断栈定地址值是否在0x2000 0000 - 0x 2000 2000之间
    {
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);//设置主函数栈指针
		Jump_To_Application();
	}
}

#if 1
/******************************************************
*函数名：

*形  参：

*返回值：

*描  述：异或取反校验

*备  注：
******************************************************/
static uint32 GetCheckSum_dw_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng)
{
	uint16 Le_w_i;
	uint8 Le_u_Xor = 0U;
	for(Le_w_i = 0U; Le_w_i < Le_w_Lng;Le_w_i++)
	{
		Le_u_Xor ^= Le_u_Dt[Le_w_i];
	}
	Le_u_Xor = (~Le_u_Xor);
	return (uint32)Le_u_Xor;
}
#endif


/******************************************************
*函数名：

*形  参：

*返回值：

*描  述：擦除flash扇区

*备  注：
******************************************************/
static void EraseFlashSector(uint16_t Le_w_sector)
{
	FLASH_Unlock();
	//下面这个清flash的状态标志很重要，如果没有清可能会导致擦除flash失败或者不能擦除
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	FLASH_EraseSector(Le_w_sector,VoltageRange_3);
	FLASH_Lock();
}
/*******************END OF FILE******************/

