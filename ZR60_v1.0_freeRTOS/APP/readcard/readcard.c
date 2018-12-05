/******************************************************
文件名：	readcard.c

描述：		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./usart3/usart3.h"
#include "./readcard/readcard.h"
#include "./wdata/wdata.h"
#include "string.h"
 
 
uint8_t volatile Sereadcard_SetKeyflag = 0U; //设置读卡密码标志
uint8_t volatile setcard = 0;//母卡设置完成标志
uint8_t volatile isSectedCard_flag = 1U;//自动寻卡设置标志：1--设置自动寻卡
struct comm_info sminfo;//缓存母卡配置信息
struct comm_info sminfo1;//缓存母卡配置信息
RebackInfo  rebackinfo;//缓存ic读卡接收数据
uint8_t volatile open_door = 0;//开门标志：1--开门

CardNumInfo SeReadcard_h_CardID;//当前开门使用的ic卡卡号

/*请求天线范围内的所有卡*/
static uint8_t Set_Card_ALL[7U] = {0x20,SetDetectCard_ALL, 0x25, 0x01, 0x01, 0x00,0x03};
static uint8_t Sereadcard_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xF5 ,0x79 ,0xED, 0xD8, 0x9F, 0x77 ,0x89, 0x03};		
//static uint8_t Sereadcard_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xFF ,0xFF ,0xFF, 0xFF, 0xFF, 0xFF ,0xD8, 0x03};	
/**
         * 读取模块的数据:
         * 0x20表示开始符
         * 0x00表示包名(其实就是主机写给模块, 模块原样返回)
         * 0x22表示是读卡操作命令
         * 0x01表示本次携带的数据只有一位
         * 0x02表示本次读取的是扇区02中的数据,
         * 0x00表示的是验证码, 需要计算
         * 0x03表示的是命令的结束符
*/
uint8_t Read_Block[7U] = {0x20,0x00, 0x22, 0x01, 0x02, 0x00,0x03};

/*定义静态函数*/
static void calcCommandsBBC(uint8_t *command,uint8_t length);
static void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length);	 
static void decodeLoadKeyResult(RebackInfo rebackInfo);
static void decodeSetDetectCard(RebackInfo rebackInfo);
static void decodeReadBlock(RebackInfo rebackInfo);



/**
     * 
     * 处理串口接收到的数据
     *
     * @param rebackInfo
*/
void handleMsg(RebackInfo rebackInfo) 
{
	//uint8 Le_u_i;
	switch (rebackInfo.Dt.DtSrt.Seqnr) 
	{
		case On_Card :  
		{
			//标签位为0表示读到卡了
			if(OK == rebackInfo.Dt.DtSrt.St)//获取卡号信息
			{
				if(0U == GetZR60Ctrl_u_CardSetSt())//非卡片设置状态
				{
					SeReadcard_h_CardID.lng = rebackInfo.Dt.DtSrt.ValidDt[3];//获取卡号长度信息
					if((SeReadcard_h_CardID.lng == 4U) && (0U == ReadCard_VrfyCardNumber(*((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4]))))))//卡号验证通过,卡号不在黑名单中
					{
						*((uint32*)SeReadcard_h_CardID.CardID) = *((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4])));
						readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));//读取扇区0x14：小区id
						USART_PRINTF_CARD_NUM("卡号 %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5],rebackInfo.Dt.DtSrt.ValidDt[6], \
												  rebackInfo.Dt.DtSrt.ValidDt[7]);
						USART_PRINTF_S("验证通过  √");
					}
					else
					{
						USART_PRINTF_CARD_NUM("卡号 %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5],rebackInfo.Dt.DtSrt.ValidDt[6], \
											  rebackInfo.Dt.DtSrt.ValidDt[7]);
						USART_PRINTF_S("验证不通过  X");
						isSectedCard_flag = 1U;
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
					}
				}
				else
				{
					readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));//读取扇区0x14
				}
			}
		}
		break;
		case Load_Key://标签位为Load_Key表示这是从机给主机的装载秘钥的反馈
		{
			USART_PRINTF_S("key密码设置成功");
			//decodeLoadKeyResult(rebackInfo);
		}
		break;
		case SetDetectCard_ALL://表示这是从机给主机的设置自动读卡的反馈
		{
			USART_PRINTF_S("自动寻卡设置成功");
			//isSectedCard_flag = 0U;
			//decodeSetDetectCard(rebackInfo);
		}
		break;
		case Block_0x14://表示这是从机读取扇区5 0x14的反馈
		case Block_0x15://表示这是从机读取扇区5 0x15的反馈
		case Block_0x16://表示这是从机读取扇区5 0x16的反馈
		case Block_0x18://表示这是从机读取扇区6 0x18的反馈
		case Block_0x19://表示这是从机读取扇区6 0x19的反馈
		case Block_0x1A://表示这是从机读取扇区6 0x1A的反馈
		{
			decodeReadBlock(rebackInfo);
		}
		break;
		default:
		break;
	}
}

/*
   设置自动寻卡
*/
void Setreadcard_AutoSearchCard(void)
{
	// 先计算正确的校验值
	u8 Le_u_lng;
	Le_u_lng = sizeof(Set_Card_ALL);
	calcCommandsBBC(Set_Card_ALL,Le_u_lng);
	SetUartCmn_TxMsg(Set_Card_ALL,Le_u_lng);
}
/*
	设置读卡密码
*/
void Setreadcard_Key(void)
{
	SetUartCmn_TxMsg(Sereadcard_u_KeyA,12);
	Sereadcard_SetKeyflag = 1U;
}



/*
   计算校验和
*/
static void calcCommandsBBC(uint8_t *command,uint8_t length) 
{
	u8 i;
	uint8_t c = command[1];
	for (i = 2; i < length - 2; i++) 
	{
		c ^= command[i];
	}

	command[length - 2] = (uint8_t) ~c;
}



static void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length) 
{
	blockCommand[4] = block;    // 要读的扇区的数据
	blockCommand[1] = block;    // 本次读取数据的请求头
	calcCommandsBBC(blockCommand,length);// 先计算正确的校验值
	SetUartCmn_TxMsg(blockCommand,length);
}


static void decodeLoadKeyResult(RebackInfo rebackInfo) 
{
 //   char status = rebackInfo.status;
	switch (rebackInfo.Dt.DtSrt.St) 
	{
		case OK:
			// 装载秘钥成功, 接下来就是设置模块自动读卡
			//sendCommand(Set_Card_ALL,sizeof(Set_Card_ALL));
		break;
		default:
			// 装载秘钥不成功,重新发送
							
			//restartMifareRead();
		break;
	}
}
 
/**
     * DB
     * 处理设置自动读卡
     *
     * @param rebackInfo
*/
static void decodeSetDetectCard(RebackInfo rebackInfo) 
{
	//char status = rebackInfo.status;
	switch (rebackInfo.Dt.DtSrt.St) 
	{
		case OK:
			//设置自动读卡成功
			//isSectedCard = true;
		break;
		default:
			// 设置自动读卡失败
			// restartMifareRead();
		break;
	}
}

	
/**
     * 
     * 处理串口接收到的数据
     *
     * @param rebackInfo
*/
static void decodeReadBlock(RebackInfo rebackInfo) 
{
	u8 Le_u_i;
	u8 Le_u_matchFlg = 0U;
	switch (rebackInfo.Dt.DtSrt.Seqnr) 
	{
		 case Block_0x14://读取小区id
		 {
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//读卡器密码验证失败
				{
					Sereadcard_SetKeyflag = 0U;
					USART_PRINTF_S("扇区5 key密码验证失败  X\n");
				}
				// 如果数据块0x14读取数据失败
				isSectedCard_flag = 1;
				USART_PRINTF_S("读扇区5块0x14失败  X\n");
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
			} 
			else 
			{	
				USART_PRINTF_S("读扇区5块0x14成功  √\n");
				if(1U == ReadCard_CardSetSt())
				{//卡片设置模式
					//("小区id:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{			
						sminfo.community_id[Le_u_i] = rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}
					readBlock(Read_Block, (uint8_t) 0x15,sizeof(Read_Block));
				}
				else
				{//非卡片设置模式，比对小区id
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{	
						if(sminfo1.community_id[Le_u_i] != rebackInfo.Dt.DtSrt.ValidDt[Le_u_i])
						{
							break;
						}
					}
					if(Le_u_i == rebackInfo.Dt.DtSrt.DtLng)
					{//小区id匹配
						if(1U == ReadCard_DEVICE_TYPE)
						{//围墙机，小区id匹配就可以开门
							open_door = 1;
							USART_PRINTF_S("围墙机，小区id匹配 -> 开门\n");
						}
						else if(2U == ReadCard_DEVICE_TYPE)
						{//门口机，继续匹配楼栋编号
							readBlock(Read_Block, (uint8_t)0x15,sizeof(Read_Block));
						}
						else
						{
							USART_PRINTF_D("设备类型错误,类型码 %d\n",ReadCard_DEVICE_TYPE);
						}
					}
					else
					{//小区id不匹配
						isSectedCard_flag = 1U;
						USART_PRINTF_S("读扇区5块0x14小区id不匹配  X\n");
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
					}
				}
			}
		 }
		break;
		case Block_0x15://读取楼栋编号1
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("读扇区5块0x15失败  X\n");
			} 
			else
			{	
				USART_PRINTF_S("读扇区5块0x15成功   √\n");
				if(1U == GetZR60Ctrl_u_CardSetSt())
				{//卡片设置模式
					//("楼栋编号1:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						sminfo.build_numOne[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		

					readBlock(Read_Block, (uint8_t)0x16,sizeof(Read_Block));
				}
				else
				{//非卡片设置模式，比对楼栋编号
					if((*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[0U])) == 0xAAAAAAAAAAAAAAAA) && (*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[8U])) == 0xAAAAAAAAAAAAAAAA))
					{//工作人员卡
						open_door = 1;
						USART_PRINTF_S("工作人员卡\n");
					}
					else
					{
						for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
						{//每个楼栋编号2字节，一个块可以装8个楼栋编号
							if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								if(*((uint16*)(&sminfo1.build_numOne[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
								{//比对楼栋编号
									Le_u_matchFlg = 1U;
									break;
								}
							}
						}
						
						if(1U == Le_u_matchFlg)
						{//匹配到楼栋编号
							open_door = 1;
							USART_PRINTF_S("读扇区5块0x15匹配到楼栋编号 -> 开门\n");
						}
						else
						{//未匹配到楼栋编号
							USART_PRINTF_S("扇区5块0x15未匹配到楼栋编号,继续匹配扇区0x16楼栋编号\n");
							readBlock(Read_Block, (uint8_t)0x16,sizeof(Read_Block));
						}
					}
				}
			}
		break;
		case Block_0x16://读取楼栋编号
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x16 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("读扇区5块0x16失败  X\n");
			} 
			else
			{
				USART_PRINTF_S("读扇区5块0x16成功  √\n");
				if(1U == GetZR60Ctrl_u_CardSetSt())
				{//卡片设置模式
					//("楼栋编号2:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						sminfo.build_numTwo[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		
					#ifdef ZR50
					setcard = 1U;
					#else
					readBlock(Read_Block, (uint8_t)0x18,sizeof(Read_Block));//读取楼栋id
					#endif
				}
				else
				{
					for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
					{//每个楼栋编号2字节，一个块可以装8个楼栋编号
						if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
						{
							if(*((uint16*)(&sminfo1.build_numTwo[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								Le_u_matchFlg = 1U;
								break;
							}
						}
					}

					if(1U == Le_u_matchFlg)
					{//匹配到楼栋编号
						open_door = 1;
						USART_PRINTF_S("读扇区5块0x16匹配到楼栋编号 -> 开门\n");
					}
					else
					{//未匹配到楼栋编号
						isSectedCard_flag = 1U;
						SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
						USART_PRINTF_S("读扇区5块0x16未匹配到楼栋编号\n");
					}
				}
			}
		}
		break;
		case Block_0x18://读取楼栋id（卡片设置模式下）
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//读卡器密码验证失败
				{
					Sereadcard_SetKeyflag = 0U;
					USART_PRINTF_S("扇区6 key密码验证失败  X\n");
				}
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("读扇区6块0x18失败  X\n");
			} 
			else
			{
				//("楼栋id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					sminfo.build_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				readBlock(Read_Block, (uint8_t)0x19,sizeof(Read_Block));//读取楼栋门id
			}
		}
		break;
		case Block_0x19://读取门id（卡片设置模式下）
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x19 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("读扇区6块0x19失败  X\n");
			} 
			else
			{
				//("门id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					sminfo.door_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				readBlock(Read_Block, (uint8_t)0x1A,sizeof(Read_Block));//读取下标
			}
		}
		break;
		case Block_0x1A://读取下标（卡片设置模式下）
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x20 fail\n");
				isSectedCard_flag = 1;
				SetAudioIO_PlayFile(AudioIO_CardInvalid,2U);
				USART_PRINTF_S("读扇区6块0x1A失败  X\n");
			} 
			else
			{
				//("下标:");
				sminfo.suffix = rebackInfo.Dt.DtSrt.ValidDt[0U];	
				setcard = 1U;
			}
		}
		break;
		default:
		break;
	}
}		

                                               

