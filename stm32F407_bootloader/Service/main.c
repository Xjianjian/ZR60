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
  * ƽ̨:����  STM32 F407 ������  
  *
  *
  *
  ************************************************
*/

#include "Include.h"


/*************************************************
  *        				�������
**************************************************/
/* Private typedef */
typedef  void (*pFunction)(void);


//uint16 Vemain_w_5msTskTimer = 0U;
static Include_UpdataInfoStruct Se_h_UpdataInfo;
pFunction Jump_To_Application;
uint32_t JumpAddress;

/*************************************************
  *        				���徲̬����
**************************************************/
static void EraseFlashSector(uint16_t Le_w_sector);
static uint32 GetCheckSum_dw_XOR(uint8* Le_u_Dt,uint16 Le_w_Lng);
static void JumpToApp(void);

/*************************************************
  *        				main����
**************************************************/
int main(void)
{
	uint32 Le_dw_i;
	uint8 Le_u_dt;
	uint32 Le_dw_CheckSum;/*����������У���*/
	uint32 Le_dw_InitDlyTime = 0xffffff;//��ʱʱ��
	/*******************
	 *	��ʼ��...	
	********************/
	while(Le_dw_InitDlyTime--);
#ifdef UART_DEBUG
	Debug_USART_Config();
#endif
	MemIf_Init();
	printf("\n����bootloader...\n");
	if(1U == MemIf_ReadEE(EepromCfg_UpDateInfo,ADDR_FLASH_SECTOR_2,	\
						  (uint8*)(&Se_h_UpdataInfo),sizeof(Se_h_UpdataInfo)))
	{
#ifdef UART_DEBUG
		printf("\n%x,%x,%d,%x\n",Se_h_UpdataInfo.DtFirstAddr,Se_h_UpdataInfo.DtendAddr \
			  ,Se_h_UpdataInfo.Lng,Se_h_UpdataInfo.CheckSum);
#endif
	}
	/******��ʼ�� end****/	
	while (1)
	{ 	
		if(Se_h_UpdataInfo.Xor == GetCheckSum_dw_XOR((uint8*)(&(Se_h_UpdataInfo.DtFirstAddr)), \
				(sizeof(Se_h_UpdataInfo)-4U)))
		{//����������У���ͨ��	
			Se_h_UpdataInfo.CheckSum = ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -4)) << 24) & 0xff000000;/*����������У���,���صĶ������ļ������4�ֽ�ΪУ���*/
			Se_h_UpdataInfo.CheckSum = Se_h_UpdataInfo.CheckSum | ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -3)) << 16);/*����������У���*/
			Se_h_UpdataInfo.CheckSum = Se_h_UpdataInfo.CheckSum | ((*((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -2)) << 8);/*����������У���*/
			Se_h_UpdataInfo.CheckSum |= *((__IO uint8*)Se_h_UpdataInfo.DtFirstAddr + Se_h_UpdataInfo.Lng -1);/*����������У���*/
			Le_dw_CheckSum = crc32(0xFFFFFFFF,(__IO uint8*)Se_h_UpdataInfo.DtFirstAddr,(Se_h_UpdataInfo.Lng -4));//�������������ݼ���õ���У���
			printf("\nSe_h_UpdataInfo.CheckSum = %x\n",Se_h_UpdataInfo.CheckSum);
			printf("\nLe_dw_CheckSum = %x\n",Le_dw_CheckSum);
			if(Se_h_UpdataInfo.CheckSum == Le_dw_CheckSum)
			{/*��������������crc32У���ͨ��*/
				/* FLASH_Sector_6��FLASH_Sector_7��FLASH_Sector_8Ϊ�û����������� */
				EraseFlashSector(FLASH_Sector_6);
				EraseFlashSector(FLASH_Sector_7);
				EraseFlashSector(FLASH_Sector_8);
				
				/* �����������������ݵ��û����������� */
				for(Le_dw_i = 0U;Le_dw_i < (Se_h_UpdataInfo.Lng - 4U);Le_dw_i++)
				{
					Le_u_dt = *(__IO uint8*)Se_h_UpdataInfo.DtFirstAddr;
					FLASH_Unlock();
					FLASH_ProgramByte((ADDR_FLASH_SECTOR_6+Le_dw_i),Le_u_dt);
					FLASH_Lock();
					Se_h_UpdataInfo.DtFirstAddr++;
				}
				
				/* ������ɣ��������������û��������������� */
				EraseFlashSector(FLASH_Sector_2);
				EraseFlashSector(FLASH_Sector_9);
				EraseFlashSector(FLASH_Sector_10);
				EraseFlashSector(FLASH_Sector_11);
				
				printf("\n�������\n");
				JumpToApp();//��ת��App
			}
			else
			{
				printf("\ncrc32У��NOK\n");
				/* crc32У��NOK */
				EraseFlashSector(FLASH_Sector_2);
				
				JumpToApp();//��ת��App
			}
		}
		else
		{
			printf("\n������Ϣ������У��NOK\n");
			JumpToApp();//��ת��App
		}			
	}
}

/*
	��ת���û�App
*/
static void JumpToApp(void)
{
	if (((*(__IO uint32_t*)USER_FLASH_FIRST_PAGE_ADDRESS) & 0x2FFE0000 ) == 0x20000000)//�ж�ջ����ֵַ�Ƿ���0x2000 0000 - 0x 2000 2000֮��
    {
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);//����������ջָ��
		Jump_To_Application();
	}
}

#if 1
/******************************************************
*��������

*��  �Σ�

*����ֵ��

*��  �������ȡ��У��

*��  ע��
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
*��������

*��  �Σ�

*����ֵ��

*��  ��������flash����

*��  ע��
******************************************************/
static void EraseFlashSector(uint16_t Le_w_sector)
{
	FLASH_Unlock();
	//���������flash��״̬��־����Ҫ�����û������ܻᵼ�²���flashʧ�ܻ��߲��ܲ���
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	FLASH_EraseSector(Le_w_sector,VoltageRange_3);
	FLASH_Lock();
}
/*******************END OF FILE******************/

