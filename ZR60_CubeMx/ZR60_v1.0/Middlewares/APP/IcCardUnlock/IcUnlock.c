/******************************************************
文件名：	IcUnlock

描述：	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include	"IcCardUnlock/IcUnlock.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static uint16_t w_SetAutoCard_DelayTime = 0U;//设置自动寻卡延时时间
static uint8_t  SeIcUnlock_u_cardSetSt = 0U;//母卡设置状态
//static uint8_t  SeIcUnlock_cardRenewflag = 0U; //母卡信息更新标志
static uint8_t  SeIcUnlock_u_cardInfoReadFlag = 0U;//母卡数据读取完成标志
static IcUnlock_RxMsgStruct  IcUnlock_RxMsg;//缓存ic读卡接收数据
static uint8_t  SeIcUnlock_AutoSectedCardFlag = 1U;//自动寻卡设置标志：1--设置自动寻卡
static uint8_t  open_door = 0;//开门标志：1--开门

static IcUnlock_CardConf Se_Cardinfo;//存储母卡信息（用于开门比对）
static IcUnlock_CardConf Se_Cardinfo_Temp;//存储母卡信息（用于读取的母卡卡片数据暂存）
static CardNumInfo Se_CardID;//当前开门使用的ic卡卡号
		
/***********请求报文帧*************/
static uint8_t  ReqPacket_ReadBlock[7U] = {0x20,0x00, 0x22, 0x01, 0x02, 0x00,0x03};
static uint8_t  SeIcUnlock_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xF5 ,0x79 ,0xED, 0xD8, 0x9F, 0x77 ,0x89, 0x03};
static uint8_t  SeIcUnlock_DetectCardPakt[7U] = {0x20,SetDetectCard_ALL, 0x25, 0x01, 0x01, 0xD9,0x03};
/**********************************/

/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void  SetCard_MainFunction(void);//母卡设置
static void  OpenDoor_IC_MainFunction(void);//开锁
static void  IcUnlock_handleMsg(IcUnlock_RxMsgStruct rebackInfo);
static void IcUnlock_calcCommandsBBC(uint8_t *Dt,uint8_t length);
static void IcUnlock_readBlock(uint8_t * requestPacket, uint8_t  requestBlock,uint8_t requestPacketLength);
/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitIcUnlock_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitIcUnlock_parameter(void)
{
	uint8_t LeZR60_u_Xor = 0;
	char Le_u_print[33] = {0};

	LeZR60_u_Xor = GetIcUnlockCfg_u_CheckSum(IC_UNLOCK_EE_CARD_INFO);//读取扇区0数据校验和（存放母卡信息）
	if((LeZR60_u_Xor == IcUnlockCfg_ReadEE(IC_UNLOCK_EE_CARD_INFO,Se_Cardinfo.community_id,sizeof(Se_Cardinfo))) && \
	  (STD_ACTIVE == GetIcUnlockCfg_u_DtVild(IC_UNLOCK_EE_CARD_INFO)))
	{
		printf("\nflash存储的母卡信息数据有效 √\r\n");
		SetIcUnlockCfg_EEVild(IC_UNLOCK_EE_CARD_INFO);//置位数据有效标志
		IcUnlockCfg_CardRenewCallback(&Se_Cardinfo);
	}
	else
	{
		printf("flash存储的母卡信息数据无效 X\r\n");
		printf("母卡数据校验和 %d\n",LeZR60_u_Xor);
		printf("母卡数据有效性标志 %d\n",GetIcUnlockCfg_u_DtVild(IC_UNLOCK_EE_CARD_INFO));
		printf("\n读取的本地母卡配置信息：");
		Json_HexToStr(Le_u_print,Se_Cardinfo.community_id,16);
		printf("\n小区ID：%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_numOne,16);
		printf("\n楼栋编号1：%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_numTwo,16);
		printf("\n楼栋编号2：%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_id,16);
		printf("\n楼栋ID：%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.door_id,16);
		printf("\n门ID：%s",Le_u_print);	
		memset(Le_u_print,0,sizeof(Le_u_print));//清0
		sprintf(Le_u_print,"%d",Se_Cardinfo.suffix);
		printf("\n下标：%s",Le_u_print);
	}
}

/******************************************************
*函数名：TskIcUnlock_MainFunction

*形  参：

*返回值：

*描  述：串口接收

*备  注：串口接收中断调用
******************************************************/
void TskIcUnlock_MainFunction(void)
{ 
	SetCard_MainFunction();//母卡设置
	
	OpenDoor_IC_MainFunction();//ic卡开锁	
}

/******************************************************
*函数名：OpenDoor_IC_MainFunction

*形  参：

*返回值：

*描  述：开锁

*备  注：
******************************************************/
static void  OpenDoor_IC_MainFunction(void)//开锁
{
	char Le_u_UnlockInfo[32U] = {0x00};
	//IC_UNLOCK_PRINTF_S("\r\n=> OpenDoor_IC_MainFunction\r\n");
	/*处理接收的ic卡数据*/
	if(1 == GetIcUnlockCfg_u_RxMsgRenew(IcUnlock_RxMsg.Dt.Block))//收到读卡器发来的一帧数据
	{
		//IC_UNLOCK_PRINTF_S("读卡器串口数据更新\r\n");
		IcUnlock_handleMsg(IcUnlock_RxMsg); //处理读卡数据	
	}

	if(open_door == 1)//开锁
	{	
		IC_UNLOCK_PRINTF_S("ic卡 -> 开门");
		open_door = 0;
		SetIcUnlockCfg_openDoor();
		SeIcUnlock_AutoSectedCardFlag = 1;//开启自动寻卡
		w_SetAutoCard_DelayTime = 0U;
		SetIcUnlockCfg_PlayFile(IC_UNLOCK_DOOROPEN);	
		
		/*记录开锁信息*/
		Json_HexToStr(Le_u_UnlockInfo,(uint8*)Se_CardID.CardID,Se_CardID.lng);
		IcUnlockCfg_u_RecordUnlockLog(Le_u_UnlockInfo,IC_UNLOCK_LOGCACHE_CARD);
	}
	
	if(SeIcUnlock_AutoSectedCardFlag == 1)//自动寻卡设置
	{
		if(w_SetAutoCard_DelayTime >= (2500/IC_UNLOCK_SCHEDULING_CYCLE))//延时2.5s下发自动寻卡设置
		{
			if(1U == GetIcUnlock_u_TxIdle())//串口tx空闲
			{
				w_SetAutoCard_DelayTime = 0U;
				SetIcUnlockCfg_TxMsg(SeIcUnlock_DetectCardPakt,7);//下发自动寻卡指令
				SeIcUnlock_AutoSectedCardFlag = 0;
			}
		}
		else
		{
			w_SetAutoCard_DelayTime++;
		}
	}
	else
	{
		w_SetAutoCard_DelayTime = 0U;
	}
	//IC_UNLOCK_PRINTF_S("\r\nOpenDoor_IC_MainFunction =>\r\n");
}

/******************************************************
*函数名：SetCard_MainFunction

*形  参：

*返回值：

*描  述：母卡设置

*备  注：
******************************************************/
static void  SetCard_MainFunction(void)//母卡设置
{
	uint8_t Le_u_i;
	static uint32_t SeCardSet_dw_Timer = 0U;
	//IC_UNLOCK_PRINTF_S("\r\n=> SetCard_MainFunction\r\n");
	switch(SeIcUnlock_u_cardSetSt)
	{
		case 0U://空闲状态
		{
			if(GetIcUnlockCfg_u_cardSetKeySt() == STD_ACTIVE)//母卡设置按键有效
			{
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_SETCARD);
				SeCardSet_dw_Timer = 0U;
				SeIcUnlock_u_cardSetSt = 1U;
				IC_UNLOCK_PRINTF_S("进入母卡设置模式");
			}
		}
		break;
		case 1U://等待母卡设置状态
		{
			SeCardSet_dw_Timer++;
			if(SeCardSet_dw_Timer <= (9000/IC_UNLOCK_SCHEDULING_CYCLE))
			{
				if(SeIcUnlock_u_cardInfoReadFlag == 1)
				{
					SeIcUnlock_u_cardInfoReadFlag = 0;	
					if(0U == IcUnlock_u_ArrayCmp(Se_Cardinfo_Temp.community_id, \
											Se_Cardinfo.community_id,sizeof(Se_Cardinfo)))//检查母卡信息有更新
					{//母卡信息有更新则写flash,设置重新初始化设备
						(void)IcUnlockCfg_WrEE(IC_UNLOCK_EE_CARD_INFO,&Se_Cardinfo_Temp,sizeof(Se_Cardinfo_Temp));
						for(Le_u_i = 0U;Le_u_i < 16U;Le_u_i++)
						{
							Se_Cardinfo.community_id[Le_u_i] = Se_Cardinfo_Temp.community_id[Le_u_i];
							Se_Cardinfo.build_numOne[Le_u_i] = Se_Cardinfo_Temp.build_numOne[Le_u_i];
							Se_Cardinfo.build_numTwo[Le_u_i] = Se_Cardinfo_Temp.build_numTwo[Le_u_i];
							Se_Cardinfo.build_id[Le_u_i] = Se_Cardinfo_Temp.build_id[Le_u_i];
							Se_Cardinfo.door_id[Le_u_i] = Se_Cardinfo_Temp.door_id[Le_u_i];
						}
						Se_Cardinfo.suffix = Se_Cardinfo_Temp.suffix;
					}
					IcUnlockCfg_CardRenewCallback(&Se_Cardinfo);//回调接口，用于母卡重新配置后相关数据的重新设置
					SeIcUnlock_u_cardSetSt = 0U;
					SeIcUnlock_AutoSectedCardFlag = 1;//开启自动寻卡
					SetIcUnlockCfg_PlayFile(IC_UNLOCK_SETCARDSUCCESS);	
					IC_UNLOCK_PRINTF_S("母卡设置成功  √");
				}
			}
			else//等待超时
			{
				SeIcUnlock_u_cardSetSt = 0U;
				IC_UNLOCK_PRINTF_S("等待母卡设置超时，退出母卡设置模式");
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_SETCARDCANCEL);
			}
		}
		break;
		default:
		break;
	}
	//IC_UNLOCK_PRINTF_S("\r\nSetCard_MainFunction =>\r\n");
}

/******************************************************
*函数名：IcUnlock_handleMsg

*形  参：

*返回值：

*描  述：接收报文处理

*备  注：
******************************************************/
static void IcUnlock_handleMsg(IcUnlock_RxMsgStruct rebackInfo) 
{
	uint8 Le_u_i;
	uint8 Le_u_matchFlg = 0U;
#if defined(IC_UNLOCK_DEBUG)
	char Le_u_print[33] = {0};
#endif
	switch (rebackInfo.Dt.DtSrt.Seqnr) 
	{
		case GetCard_Num : //标签位为0表示读到卡了
		{	
			if(OK == rebackInfo.Dt.DtSrt.St)//获取卡号信息 
			{
				if(0U == SeIcUnlock_u_cardSetSt)//非卡片设置状态
				{
					Se_CardID.lng = rebackInfo.Dt.DtSrt.ValidDt[3];//获取卡号长度信息
					if((Se_CardID.lng == 4U) && (0U == IcUnlock_VrfyCardNumber(*((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4]))))))//卡号验证通过,卡号不在黑名单中
					{
						*((uint32*)Se_CardID.CardID) = *((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4])));
						IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x14,sizeof(ReqPacket_ReadBlock));//读取扇区0x14：小区id
						IC_UNLOCK_PRINTF_CARD_NUM("卡号 %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5], \
												  rebackInfo.Dt.DtSrt.ValidDt[6],rebackInfo.Dt.DtSrt.ValidDt[7]);
						IC_UNLOCK_PRINTF_S("验证通过  √");
					}
					else
					{
						IC_UNLOCK_PRINTF_CARD_NUM("卡号 %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5], \
												  rebackInfo.Dt.DtSrt.ValidDt[6],rebackInfo.Dt.DtSrt.ValidDt[7]);
						IC_UNLOCK_PRINTF_S("验证不通过  X");
						SeIcUnlock_AutoSectedCardFlag = 1U;
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
					}
				}
				else
				{
					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x14,sizeof(ReqPacket_ReadBlock));//读取扇区0x14
				}
			}
		}
		break;
		case Load_Key://标签位为Load_Key表示这是从机给主机的装载秘钥的反馈
		{
			IC_UNLOCK_PRINTF_S("key密码设置成功");
		}
		break;
		case SetDetectCard_ALL://表示这是从机给主机的设置自动读卡的反馈
		{
			IC_UNLOCK_PRINTF_S("自动寻卡设置成功");
		}
		break;
		case Block_0x14://表示这是从机读取扇区5 0x14的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//读卡器密码验证失败
				{
					IC_UNLOCK_PRINTF_S("扇区5 key密码验证失败  X\n");
					IC_UNLOCK_PRINTF_S("重新设置验证秘钥\r\n");
					SetIcUnlockCfg_TxMsg(SeIcUnlock_u_KeyA,12);
				}
				// 如果数据块0x14读取数据失败
				SeIcUnlock_AutoSectedCardFlag = 1;
				IC_UNLOCK_PRINTF_S("读扇区5块0x14失败  X\n");
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
			} 
			else 
			{	
				IC_UNLOCK_PRINTF_S("\n读扇区5块0x14（小区ID）成功,扇区数据：");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				if(1U == SeIcUnlock_u_cardSetSt)
				{//卡片设置模式
					//("小区id:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{			
						Se_Cardinfo_Temp.community_id[Le_u_i] = rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}

					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x15,sizeof(ReqPacket_ReadBlock));
				}
				else
				{//非卡片设置模式，比对小区id
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{	
						if(Se_Cardinfo.community_id[Le_u_i] != rebackInfo.Dt.DtSrt.ValidDt[Le_u_i])
						{
							break;
						}
					}
					if(Le_u_i == rebackInfo.Dt.DtSrt.DtLng)
					{//小区id匹配
						if(1U == IC_UNLOCK_DEVICE_TYPE)
						{//围墙机，小区id匹配就可以开门
							open_door = 1;
							IC_UNLOCK_PRINTF_S("围墙机，小区id匹配 -> 开门\n");
						}
						else if(2U == IC_UNLOCK_DEVICE_TYPE)
						{//门口机，继续匹配楼栋编号
							IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x15,sizeof(ReqPacket_ReadBlock));
						}
						else
						{
							IC_UNLOCK_PRINTF_D("设备类型错误,类型码 %d\n",IC_UNLOCK_DEVICE_TYPE);
						}
					}
					else
					{//小区id不匹配
						SeIcUnlock_AutoSectedCardFlag = 1U;
						IC_UNLOCK_PRINTF_S("读扇区5块0x14小区id不匹配  X\n");
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
					}
				}
			}
		}
		break;
		case Block_0x15://表示这是从机读取扇区5 0x15的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("读扇区5块0x15失败  X\n");
			} 
			else
			{	
				IC_UNLOCK_PRINTF_S("\n读扇区5块0x15（楼栋编号）成功,读取数据：");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif				
				if(1U == SeIcUnlock_u_cardSetSt)
				{//卡片设置模式
					//("楼栋编号1:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						Se_Cardinfo_Temp.build_numOne[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		

					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x16,sizeof(ReqPacket_ReadBlock));
				}
				else
				{//非卡片设置模式，比对楼栋编号
					if((*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[0U])) == 0xAAAAAAAAAAAAAAAA) && (*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[8U])) == 0xAAAAAAAAAAAAAAAA))
					{//工作人员卡
						open_door = 1;
						IC_UNLOCK_PRINTF_S("工作人员卡\n");
					}
					else
					{
						for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
						{//每个楼栋编号2字节，一个块可以装8个楼栋编号
							if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								if(*((uint16*)(&Se_Cardinfo.build_numOne[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
								{//比对楼栋编号
									Le_u_matchFlg = 1U;
									break;
								}
							}
						}
						
						if(1U == Le_u_matchFlg)
						{//匹配到楼栋编号
							open_door = 1;
							IC_UNLOCK_PRINTF_S("读扇区5块0x15匹配到楼栋编号 -> 开门\n");
						}
						else
						{//未匹配到楼栋编号
							IC_UNLOCK_PRINTF_S("扇区5块0x15未匹配到楼栋编号,继续匹配扇区0x16楼栋编号\n");
							IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x16,sizeof(ReqPacket_ReadBlock));
						}
					}
				}
			}
		}
		break;
		case Block_0x16://表示这是从机读取扇区5 0x16的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("读扇区5块0x16失败  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n读扇区5块0x16（楼栋编号）成功,读取数据：");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif					
				if(1U == SeIcUnlock_u_cardSetSt)
				{//卡片设置模式
					//("楼栋编号2:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						Se_Cardinfo_Temp.build_numTwo[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		
					#ifdef ZR50
					SeIcUnlock_u_cardInfoReadFlag = 1U;
					#else
					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x18,sizeof(ReqPacket_ReadBlock));//读取楼栋id
					#endif
				}
				else
				{
					for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
					{//每个楼栋编号2字节，一个块可以装8个楼栋编号
						if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
						{
							if(*((uint16*)(&Se_Cardinfo.build_numOne[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								Le_u_matchFlg = 1U;
								break;
							}
						}
					}

					if(1U == Le_u_matchFlg)
					{//匹配到楼栋编号
						open_door = 1;
						IC_UNLOCK_PRINTF_S("读扇区5块0x16匹配到楼栋编号 -> 开门\n");
					}
					else
					{//未匹配到楼栋编号
						SeIcUnlock_AutoSectedCardFlag = 1U;
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
						IC_UNLOCK_PRINTF_S("读扇区5块0x16未匹配到楼栋编号\n");
					}
				}
			}
		}
		break;
		case Block_0x18://表示这是从机读取扇区6 0x18的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//读卡器密码验证失败
				{
					IC_UNLOCK_PRINTF_S("扇区6 key密码验证失败  X\n");
					IC_UNLOCK_PRINTF_S("重新设置验证秘钥\r\n");
					SetIcUnlockCfg_TxMsg(SeIcUnlock_u_KeyA,12);
				}
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("读扇区6块0x18失败  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n读扇区6块0x18（楼栋ID）成功,读取数据：");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				//("楼栋id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					Se_Cardinfo_Temp.build_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x19,sizeof(ReqPacket_ReadBlock));//读取楼栋门id
			}
		}
		break;
		case Block_0x19://表示这是从机读取扇区6 0x19的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("读扇区6块0x19失败  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n读扇区6块0x19（门ID）成功,读取数据：");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				//("门id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					Se_Cardinfo_Temp.door_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x1A,sizeof(ReqPacket_ReadBlock));//读取下标
			}
		}
		break;
		case Block_0x1A://表示这是从机读取扇区6 0x1A的反馈
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x20 fail\n");
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("读扇区6块0x1A失败  X\n");
			} 
			else
			{//("下标:");
				Se_Cardinfo_Temp.suffix = 0U;
				Se_Cardinfo_Temp.suffix = rebackInfo.Dt.DtSrt.ValidDt[0];
				Se_Cardinfo_Temp.suffix = ((Se_Cardinfo_Temp.suffix << 8U) | rebackInfo.Dt.DtSrt.ValidDt[1]);
				SeIcUnlock_u_cardInfoReadFlag = 1U;
#if defined(IC_UNLOCK_DEBUG)
				memset(Le_u_print,0,sizeof(Le_u_print));//清0
				sprintf(Le_u_print,"%d",Se_Cardinfo_Temp.suffix);
				IC_UNLOCK_PRINTF_D("\n读扇区6块0x1A（下标）成功,读取数据：%s",Le_u_print);
#endif				
#if defined(IC_UNLOCK_DEBUG)
				IC_UNLOCK_PRINTF_S("\n母卡配置更新后信息：");
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.community_id,16);
				IC_UNLOCK_PRINTF_D("\n小区ID：%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_numOne,16);
				IC_UNLOCK_PRINTF_D("\n楼栋编号1：%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_numTwo,16);
				IC_UNLOCK_PRINTF_D("\n楼栋编号2：%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_id,16);
				IC_UNLOCK_PRINTF_D("\n楼栋ID：%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.door_id,16);
				IC_UNLOCK_PRINTF_D("\n门ID：%s",Le_u_print);	
				memset(Le_u_print,0,sizeof(Le_u_print));//清0
				sprintf(Le_u_print,"%d",Se_Cardinfo_Temp.suffix);
				IC_UNLOCK_PRINTF_D("\n下标：%s",Le_u_print);
#endif
			}
		}
		break;
		default:
		break;
	}
}

/******************************************************
*函数名：GetIcUnlock_u_CardSetSt

*形  参：

*返回值：

*描  述：获取卡片设置状态

*备  注：
******************************************************/
uint8_t	GetIcUnlock_u_CardSetSt(void)
{
	return SeIcUnlock_u_cardSetSt;
}


/******************************************************
*函数名：SetIcUnlock_u_CardReadSuccess

*形  参：

*返回值：

*描  述：卡片数据读取完成

*备  注：
******************************************************/
void SetIcUnlock_u_CardReadSuccess(void)
{
	SeIcUnlock_u_cardInfoReadFlag = 1U;//母卡数据读取完成标志
}


/******************************************************
*函数名：IcUnlock_u_ArrayCmp

*形  参：

*返回值：

*描  述：数组比较

*备  注：
******************************************************/
char IcUnlock_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len)
{
	unsigned int Le_w_i;
	for(Le_w_i = 0; Le_w_i < len; Le_w_i++)
		if(a[Le_w_i]!=b[Le_w_i]) return 0;//出现不相等元素，返回0.
	return 1;//完全相等，返回1。
}



/*
   计算校验和
*/
static void IcUnlock_calcCommandsBBC(uint8_t *Dt,uint8_t length) 
{
	uint8 i;
	uint8_t c = Dt[1];
	for (i = 2; i < length - 2; i++) 
	{
		c ^= Dt[i];
	}

	Dt[length - 2] = (uint8_t) ~c;
}


/*
   请求报文封装、发送
*/
static void IcUnlock_readBlock(uint8_t * requestPacket, uint8_t  requestBlock,uint8_t requestPacketLength) 
{
	requestPacket[4] = requestBlock;    // 要读的扇区的数据
	requestPacket[1] = requestBlock;    // 本次读取数据的请求头
	IcUnlock_calcCommandsBBC(requestPacket,requestPacketLength);// 先计算正确的校验值
	SetIcUnlockCfg_TxMsg(requestPacket,requestPacketLength);
}

/******************************************************
*函数名：GetIcUnlock_communityID

*形  参：

*返回值：

*描  述：获取小区ID

*备  注：
******************************************************/
void GetIcUnlock_communityID(unsigned char* community_id)
 {
	uint8 Le_u_i;
	for(Le_u_i = 0;Le_u_i < 16;Le_u_i++)
	{
		community_id[Le_u_i] = Se_Cardinfo.community_id[Le_u_i];
	}
}


/******************************************************
*函数名：GetIcUnlock_buildID

*形  参：

*返回值：

*描  述：获取楼栋ID

*备  注：
******************************************************/
void GetIcUnlock_buildID(unsigned char* build_id)
 {
	uint8 Le_u_i;
	for(Le_u_i = 0;Le_u_i < 16;Le_u_i++)
	{
		build_id[Le_u_i] = Se_Cardinfo.build_id[Le_u_i];
	}
}


/******************************************************
*函数名：GetIcUnlock_w_suffix

*形  参：

*返回值：

*描  述：获取下标

*备  注：
******************************************************/
unsigned short GetIcUnlock_w_suffix(void)
{
	return Se_Cardinfo.suffix;
}
