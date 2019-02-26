/******************************************************
�ļ�����	IcUnlock

������	
Data			Vasion			author
2018/04/7		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include	"IcCardUnlock/IcUnlock.h"

/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/
static uint16_t w_SetAutoCard_DelayTime = 0U;//�����Զ�Ѱ����ʱʱ��
static uint8_t  SeIcUnlock_u_cardSetSt = 0U;//ĸ������״̬
//static uint8_t  SeIcUnlock_cardRenewflag = 0U; //ĸ����Ϣ���±�־
static uint8_t  SeIcUnlock_u_cardInfoReadFlag = 0U;//ĸ�����ݶ�ȡ��ɱ�־
static IcUnlock_RxMsgStruct  IcUnlock_RxMsg;//����ic������������
static uint8_t  SeIcUnlock_AutoSectedCardFlag = 1U;//�Զ�Ѱ�����ñ�־��1--�����Զ�Ѱ��
static uint8_t  open_door = 0;//���ű�־��1--����

static IcUnlock_CardConf Se_Cardinfo;//�洢ĸ����Ϣ�����ڿ��űȶԣ�
static IcUnlock_CardConf Se_Cardinfo_Temp;//�洢ĸ����Ϣ�����ڶ�ȡ��ĸ����Ƭ�����ݴ棩
static CardNumInfo Se_CardID;//��ǰ����ʹ�õ�ic������
		
/***********������֡*************/
static uint8_t  ReqPacket_ReadBlock[7U] = {0x20,0x00, 0x22, 0x01, 0x02, 0x00,0x03};
static uint8_t  SeIcUnlock_u_KeyA[12U] = {0x20 ,Load_Key, 0x20 ,0x06, 0xF5 ,0x79 ,0xED, 0xD8, 0x9F, 0x77 ,0x89, 0x03};
static uint8_t  SeIcUnlock_DetectCardPakt[7U] = {0x20,SetDetectCard_ALL, 0x25, 0x01, 0x01, 0xD9,0x03};
/**********************************/

/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void  SetCard_MainFunction(void);//ĸ������
static void  OpenDoor_IC_MainFunction(void);//����
static void  IcUnlock_handleMsg(IcUnlock_RxMsgStruct rebackInfo);
static void IcUnlock_calcCommandsBBC(uint8_t *Dt,uint8_t length);
static void IcUnlock_readBlock(uint8_t * requestPacket, uint8_t  requestBlock,uint8_t requestPacketLength);
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitIcUnlock_parameter

*��  �Σ�void

*����ֵ��void

*��  ������ʼ��

*��  ע��
******************************************************/
void InitIcUnlock_parameter(void)
{
	uint8_t LeZR60_u_Xor = 0;
	char Le_u_print[33] = {0};

	LeZR60_u_Xor = GetIcUnlockCfg_u_CheckSum(IC_UNLOCK_EE_CARD_INFO);//��ȡ����0����У��ͣ����ĸ����Ϣ��
	if((LeZR60_u_Xor == IcUnlockCfg_ReadEE(IC_UNLOCK_EE_CARD_INFO,Se_Cardinfo.community_id,sizeof(Se_Cardinfo))) && \
	  (STD_ACTIVE == GetIcUnlockCfg_u_DtVild(IC_UNLOCK_EE_CARD_INFO)))
	{
		printf("\nflash�洢��ĸ����Ϣ������Ч ��\r\n");
		SetIcUnlockCfg_EEVild(IC_UNLOCK_EE_CARD_INFO);//��λ������Ч��־
		IcUnlockCfg_CardRenewCallback(&Se_Cardinfo);
	}
	else
	{
		printf("flash�洢��ĸ����Ϣ������Ч X\r\n");
		printf("ĸ������У��� %d\n",LeZR60_u_Xor);
		printf("ĸ��������Ч�Ա�־ %d\n",GetIcUnlockCfg_u_DtVild(IC_UNLOCK_EE_CARD_INFO));
		printf("\n��ȡ�ı���ĸ��������Ϣ��");
		Json_HexToStr(Le_u_print,Se_Cardinfo.community_id,16);
		printf("\nС��ID��%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_numOne,16);
		printf("\n¥�����1��%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_numTwo,16);
		printf("\n¥�����2��%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.build_id,16);
		printf("\n¥��ID��%s",Le_u_print);
		Json_HexToStr(Le_u_print,Se_Cardinfo.door_id,16);
		printf("\n��ID��%s",Le_u_print);	
		memset(Le_u_print,0,sizeof(Le_u_print));//��0
		sprintf(Le_u_print,"%d",Se_Cardinfo.suffix);
		printf("\n�±꣺%s",Le_u_print);
	}
}

/******************************************************
*��������TskIcUnlock_MainFunction

*��  �Σ�

*����ֵ��

*��  �������ڽ���

*��  ע�����ڽ����жϵ���
******************************************************/
void TskIcUnlock_MainFunction(void)
{ 
	SetCard_MainFunction();//ĸ������
	
	OpenDoor_IC_MainFunction();//ic������	
}

/******************************************************
*��������OpenDoor_IC_MainFunction

*��  �Σ�

*����ֵ��

*��  ��������

*��  ע��
******************************************************/
static void  OpenDoor_IC_MainFunction(void)//����
{
	char Le_u_UnlockInfo[32U] = {0x00};
	//IC_UNLOCK_PRINTF_S("\r\n=> OpenDoor_IC_MainFunction\r\n");
	/*������յ�ic������*/
	if(1 == GetIcUnlockCfg_u_RxMsgRenew(IcUnlock_RxMsg.Dt.Block))//�յ�������������һ֡����
	{
		//IC_UNLOCK_PRINTF_S("�������������ݸ���\r\n");
		IcUnlock_handleMsg(IcUnlock_RxMsg); //�����������	
	}

	if(open_door == 1)//����
	{	
		IC_UNLOCK_PRINTF_S("ic�� -> ����");
		open_door = 0;
		SetIcUnlockCfg_openDoor();
		SeIcUnlock_AutoSectedCardFlag = 1;//�����Զ�Ѱ��
		w_SetAutoCard_DelayTime = 0U;
		SetIcUnlockCfg_PlayFile(IC_UNLOCK_DOOROPEN);	
		
		/*��¼������Ϣ*/
		Json_HexToStr(Le_u_UnlockInfo,(uint8*)Se_CardID.CardID,Se_CardID.lng);
		IcUnlockCfg_u_RecordUnlockLog(Le_u_UnlockInfo,IC_UNLOCK_LOGCACHE_CARD);
	}
	
	if(SeIcUnlock_AutoSectedCardFlag == 1)//�Զ�Ѱ������
	{
		if(w_SetAutoCard_DelayTime >= (2500/IC_UNLOCK_SCHEDULING_CYCLE))//��ʱ2.5s�·��Զ�Ѱ������
		{
			if(1U == GetIcUnlock_u_TxIdle())//����tx����
			{
				w_SetAutoCard_DelayTime = 0U;
				SetIcUnlockCfg_TxMsg(SeIcUnlock_DetectCardPakt,7);//�·��Զ�Ѱ��ָ��
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
*��������SetCard_MainFunction

*��  �Σ�

*����ֵ��

*��  ����ĸ������

*��  ע��
******************************************************/
static void  SetCard_MainFunction(void)//ĸ������
{
	uint8_t Le_u_i;
	static uint32_t SeCardSet_dw_Timer = 0U;
	//IC_UNLOCK_PRINTF_S("\r\n=> SetCard_MainFunction\r\n");
	switch(SeIcUnlock_u_cardSetSt)
	{
		case 0U://����״̬
		{
			if(GetIcUnlockCfg_u_cardSetKeySt() == STD_ACTIVE)//ĸ�����ð�����Ч
			{
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_SETCARD);
				SeCardSet_dw_Timer = 0U;
				SeIcUnlock_u_cardSetSt = 1U;
				IC_UNLOCK_PRINTF_S("����ĸ������ģʽ");
			}
		}
		break;
		case 1U://�ȴ�ĸ������״̬
		{
			SeCardSet_dw_Timer++;
			if(SeCardSet_dw_Timer <= (9000/IC_UNLOCK_SCHEDULING_CYCLE))
			{
				if(SeIcUnlock_u_cardInfoReadFlag == 1)
				{
					SeIcUnlock_u_cardInfoReadFlag = 0;	
					if(0U == IcUnlock_u_ArrayCmp(Se_Cardinfo_Temp.community_id, \
											Se_Cardinfo.community_id,sizeof(Se_Cardinfo)))//���ĸ����Ϣ�и���
					{//ĸ����Ϣ�и�����дflash,�������³�ʼ���豸
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
					IcUnlockCfg_CardRenewCallback(&Se_Cardinfo);//�ص��ӿڣ�����ĸ���������ú�������ݵ���������
					SeIcUnlock_u_cardSetSt = 0U;
					SeIcUnlock_AutoSectedCardFlag = 1;//�����Զ�Ѱ��
					SetIcUnlockCfg_PlayFile(IC_UNLOCK_SETCARDSUCCESS);	
					IC_UNLOCK_PRINTF_S("ĸ�����óɹ�  ��");
				}
			}
			else//�ȴ���ʱ
			{
				SeIcUnlock_u_cardSetSt = 0U;
				IC_UNLOCK_PRINTF_S("�ȴ�ĸ�����ó�ʱ���˳�ĸ������ģʽ");
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
*��������IcUnlock_handleMsg

*��  �Σ�

*����ֵ��

*��  �������ձ��Ĵ���

*��  ע��
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
		case GetCard_Num : //��ǩλΪ0��ʾ��������
		{	
			if(OK == rebackInfo.Dt.DtSrt.St)//��ȡ������Ϣ 
			{
				if(0U == SeIcUnlock_u_cardSetSt)//�ǿ�Ƭ����״̬
				{
					Se_CardID.lng = rebackInfo.Dt.DtSrt.ValidDt[3];//��ȡ���ų�����Ϣ
					if((Se_CardID.lng == 4U) && (0U == IcUnlock_VrfyCardNumber(*((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4]))))))//������֤ͨ��,���Ų��ں�������
					{
						*((uint32*)Se_CardID.CardID) = *((uint32*)(&(rebackInfo.Dt.DtSrt.ValidDt[4])));
						IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x14,sizeof(ReqPacket_ReadBlock));//��ȡ����0x14��С��id
						IC_UNLOCK_PRINTF_CARD_NUM("���� %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5], \
												  rebackInfo.Dt.DtSrt.ValidDt[6],rebackInfo.Dt.DtSrt.ValidDt[7]);
						IC_UNLOCK_PRINTF_S("��֤ͨ��  ��");
					}
					else
					{
						IC_UNLOCK_PRINTF_CARD_NUM("���� %x%x%x%x ",rebackInfo.Dt.DtSrt.ValidDt[4],rebackInfo.Dt.DtSrt.ValidDt[5], \
												  rebackInfo.Dt.DtSrt.ValidDt[6],rebackInfo.Dt.DtSrt.ValidDt[7]);
						IC_UNLOCK_PRINTF_S("��֤��ͨ��  X");
						SeIcUnlock_AutoSectedCardFlag = 1U;
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
					}
				}
				else
				{
					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x14,sizeof(ReqPacket_ReadBlock));//��ȡ����0x14
				}
			}
		}
		break;
		case Load_Key://��ǩλΪLoad_Key��ʾ���Ǵӻ���������װ����Կ�ķ���
		{
			IC_UNLOCK_PRINTF_S("key�������óɹ�");
		}
		break;
		case SetDetectCard_ALL://��ʾ���Ǵӻ��������������Զ������ķ���
		{
			IC_UNLOCK_PRINTF_S("�Զ�Ѱ�����óɹ�");
		}
		break;
		case Block_0x14://��ʾ���Ǵӻ���ȡ����5 0x14�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//������������֤ʧ��
				{
					IC_UNLOCK_PRINTF_S("����5 key������֤ʧ��  X\n");
					IC_UNLOCK_PRINTF_S("����������֤��Կ\r\n");
					SetIcUnlockCfg_TxMsg(SeIcUnlock_u_KeyA,12);
				}
				// ������ݿ�0x14��ȡ����ʧ��
				SeIcUnlock_AutoSectedCardFlag = 1;
				IC_UNLOCK_PRINTF_S("������5��0x14ʧ��  X\n");
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
			} 
			else 
			{	
				IC_UNLOCK_PRINTF_S("\n������5��0x14��С��ID���ɹ�,�������ݣ�");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				if(1U == SeIcUnlock_u_cardSetSt)
				{//��Ƭ����ģʽ
					//("С��id:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{			
						Se_Cardinfo_Temp.community_id[Le_u_i] = rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}

					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x15,sizeof(ReqPacket_ReadBlock));
				}
				else
				{//�ǿ�Ƭ����ģʽ���ȶ�С��id
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{	
						if(Se_Cardinfo.community_id[Le_u_i] != rebackInfo.Dt.DtSrt.ValidDt[Le_u_i])
						{
							break;
						}
					}
					if(Le_u_i == rebackInfo.Dt.DtSrt.DtLng)
					{//С��idƥ��
						if(1U == IC_UNLOCK_DEVICE_TYPE)
						{//Χǽ����С��idƥ��Ϳ��Կ���
							open_door = 1;
							IC_UNLOCK_PRINTF_S("Χǽ����С��idƥ�� -> ����\n");
						}
						else if(2U == IC_UNLOCK_DEVICE_TYPE)
						{//�ſڻ�������ƥ��¥�����
							IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x15,sizeof(ReqPacket_ReadBlock));
						}
						else
						{
							IC_UNLOCK_PRINTF_D("�豸���ʹ���,������ %d\n",IC_UNLOCK_DEVICE_TYPE);
						}
					}
					else
					{//С��id��ƥ��
						SeIcUnlock_AutoSectedCardFlag = 1U;
						IC_UNLOCK_PRINTF_S("������5��0x14С��id��ƥ��  X\n");
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
					}
				}
			}
		}
		break;
		case Block_0x15://��ʾ���Ǵӻ���ȡ����5 0x15�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("������5��0x15ʧ��  X\n");
			} 
			else
			{	
				IC_UNLOCK_PRINTF_S("\n������5��0x15��¥����ţ��ɹ�,��ȡ���ݣ�");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif				
				if(1U == SeIcUnlock_u_cardSetSt)
				{//��Ƭ����ģʽ
					//("¥�����1:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						Se_Cardinfo_Temp.build_numOne[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		

					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x16,sizeof(ReqPacket_ReadBlock));
				}
				else
				{//�ǿ�Ƭ����ģʽ���ȶ�¥�����
					if((*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[0U])) == 0xAAAAAAAAAAAAAAAA) && (*((uint64_t*)(&rebackInfo.Dt.DtSrt.ValidDt[8U])) == 0xAAAAAAAAAAAAAAAA))
					{//������Ա��
						open_door = 1;
						IC_UNLOCK_PRINTF_S("������Ա��\n");
					}
					else
					{
						for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
						{//ÿ��¥�����2�ֽڣ�һ�������װ8��¥�����
							if(0xFFFF != *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
							{
								if(*((uint16*)(&Se_Cardinfo.build_numOne[0U])) == *((uint16*)(&rebackInfo.Dt.DtSrt.ValidDt[2*Le_u_i])))
								{//�ȶ�¥�����
									Le_u_matchFlg = 1U;
									break;
								}
							}
						}
						
						if(1U == Le_u_matchFlg)
						{//ƥ�䵽¥�����
							open_door = 1;
							IC_UNLOCK_PRINTF_S("������5��0x15ƥ�䵽¥����� -> ����\n");
						}
						else
						{//δƥ�䵽¥�����
							IC_UNLOCK_PRINTF_S("����5��0x15δƥ�䵽¥�����,����ƥ������0x16¥�����\n");
							IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x16,sizeof(ReqPacket_ReadBlock));
						}
					}
				}
			}
		}
		break;
		case Block_0x16://��ʾ���Ǵӻ���ȡ����5 0x16�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("������5��0x16ʧ��  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n������5��0x16��¥����ţ��ɹ�,��ȡ���ݣ�");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif					
				if(1U == SeIcUnlock_u_cardSetSt)
				{//��Ƭ����ģʽ
					//("¥�����2:");
					for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
					{
						Se_Cardinfo_Temp.build_numTwo[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
					}		
					#ifdef ZR50
					SeIcUnlock_u_cardInfoReadFlag = 1U;
					#else
					IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x18,sizeof(ReqPacket_ReadBlock));//��ȡ¥��id
					#endif
				}
				else
				{
					for(Le_u_i = 0U;Le_u_i < (rebackInfo.Dt.DtSrt.DtLng/2);Le_u_i++)
					{//ÿ��¥�����2�ֽڣ�һ�������װ8��¥�����
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
					{//ƥ�䵽¥�����
						open_door = 1;
						IC_UNLOCK_PRINTF_S("������5��0x16ƥ�䵽¥����� -> ����\n");
					}
					else
					{//δƥ�䵽¥�����
						SeIcUnlock_AutoSectedCardFlag = 1U;
						SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
						IC_UNLOCK_PRINTF_S("������5��0x16δƥ�䵽¥�����\n");
					}
				}
			}
		}
		break;
		case Block_0x18://��ʾ���Ǵӻ���ȡ����6 0x18�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				if(rebackInfo.Dt.DtSrt.St == 0x09)//������������֤ʧ��
				{
					IC_UNLOCK_PRINTF_S("����6 key������֤ʧ��  X\n");
					IC_UNLOCK_PRINTF_S("����������֤��Կ\r\n");
					SetIcUnlockCfg_TxMsg(SeIcUnlock_u_KeyA,12);
				}
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("������6��0x18ʧ��  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n������6��0x18��¥��ID���ɹ�,��ȡ���ݣ�");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				//("¥��id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					Se_Cardinfo_Temp.build_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x19,sizeof(ReqPacket_ReadBlock));//��ȡ¥����id
			}
		}
		break;
		case Block_0x19://��ʾ���Ǵӻ���ȡ����6 0x19�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("������6��0x19ʧ��  X\n");
			} 
			else
			{
				IC_UNLOCK_PRINTF_S("\n������6��0x19����ID���ɹ�,��ȡ���ݣ�");
#if defined(IC_UNLOCK_DEBUG)
				Json_HexToStr(Le_u_print,rebackInfo.Dt.DtSrt.ValidDt,16);
				IC_UNLOCK_PRINTF_S(Le_u_print);
#endif
				//("��id:");
				for(Le_u_i = 0;Le_u_i < rebackInfo.Dt.DtSrt.DtLng;Le_u_i++)
				{
					Se_Cardinfo_Temp.door_id[Le_u_i]= rebackInfo.Dt.DtSrt.ValidDt[Le_u_i];
				}		
				IcUnlock_readBlock(ReqPacket_ReadBlock, (uint8_t) 0x1A,sizeof(ReqPacket_ReadBlock));//��ȡ�±�
			}
		}
		break;
		case Block_0x1A://��ʾ���Ǵӻ���ȡ����6 0x1A�ķ���
		{
			if (rebackInfo.Dt.DtSrt.St != OK) 
			{
				//("decodeReadBlock: Block_0x20 fail\n");
				SeIcUnlock_AutoSectedCardFlag = 1;
				SetIcUnlockCfg_PlayFile(IC_UNLOCK_CARD_INVALID);
				IC_UNLOCK_PRINTF_S("������6��0x1Aʧ��  X\n");
			} 
			else
			{//("�±�:");
				Se_Cardinfo_Temp.suffix = 0U;
				Se_Cardinfo_Temp.suffix = rebackInfo.Dt.DtSrt.ValidDt[0];
				Se_Cardinfo_Temp.suffix = ((Se_Cardinfo_Temp.suffix << 8U) | rebackInfo.Dt.DtSrt.ValidDt[1]);
				SeIcUnlock_u_cardInfoReadFlag = 1U;
#if defined(IC_UNLOCK_DEBUG)
				memset(Le_u_print,0,sizeof(Le_u_print));//��0
				sprintf(Le_u_print,"%d",Se_Cardinfo_Temp.suffix);
				IC_UNLOCK_PRINTF_D("\n������6��0x1A���±꣩�ɹ�,��ȡ���ݣ�%s",Le_u_print);
#endif				
#if defined(IC_UNLOCK_DEBUG)
				IC_UNLOCK_PRINTF_S("\nĸ�����ø��º���Ϣ��");
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.community_id,16);
				IC_UNLOCK_PRINTF_D("\nС��ID��%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_numOne,16);
				IC_UNLOCK_PRINTF_D("\n¥�����1��%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_numTwo,16);
				IC_UNLOCK_PRINTF_D("\n¥�����2��%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.build_id,16);
				IC_UNLOCK_PRINTF_D("\n¥��ID��%s",Le_u_print);
				Json_HexToStr(Le_u_print,Se_Cardinfo_Temp.door_id,16);
				IC_UNLOCK_PRINTF_D("\n��ID��%s",Le_u_print);	
				memset(Le_u_print,0,sizeof(Le_u_print));//��0
				sprintf(Le_u_print,"%d",Se_Cardinfo_Temp.suffix);
				IC_UNLOCK_PRINTF_D("\n�±꣺%s",Le_u_print);
#endif
			}
		}
		break;
		default:
		break;
	}
}

/******************************************************
*��������GetIcUnlock_u_CardSetSt

*��  �Σ�

*����ֵ��

*��  ������ȡ��Ƭ����״̬

*��  ע��
******************************************************/
uint8_t	GetIcUnlock_u_CardSetSt(void)
{
	return SeIcUnlock_u_cardSetSt;
}


/******************************************************
*��������SetIcUnlock_u_CardReadSuccess

*��  �Σ�

*����ֵ��

*��  ������Ƭ���ݶ�ȡ���

*��  ע��
******************************************************/
void SetIcUnlock_u_CardReadSuccess(void)
{
	SeIcUnlock_u_cardInfoReadFlag = 1U;//ĸ�����ݶ�ȡ��ɱ�־
}


/******************************************************
*��������IcUnlock_u_ArrayCmp

*��  �Σ�

*����ֵ��

*��  ��������Ƚ�

*��  ע��
******************************************************/
char IcUnlock_u_ArrayCmp(unsigned char *a,unsigned char *b, unsigned int len)
{
	unsigned int Le_w_i;
	for(Le_w_i = 0; Le_w_i < len; Le_w_i++)
		if(a[Le_w_i]!=b[Le_w_i]) return 0;//���ֲ����Ԫ�أ�����0.
	return 1;//��ȫ��ȣ�����1��
}



/*
   ����У���
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
   �����ķ�װ������
*/
static void IcUnlock_readBlock(uint8_t * requestPacket, uint8_t  requestBlock,uint8_t requestPacketLength) 
{
	requestPacket[4] = requestBlock;    // Ҫ��������������
	requestPacket[1] = requestBlock;    // ���ζ�ȡ���ݵ�����ͷ
	IcUnlock_calcCommandsBBC(requestPacket,requestPacketLength);// �ȼ�����ȷ��У��ֵ
	SetIcUnlockCfg_TxMsg(requestPacket,requestPacketLength);
}

/******************************************************
*��������GetIcUnlock_communityID

*��  �Σ�

*����ֵ��

*��  ������ȡС��ID

*��  ע��
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
*��������GetIcUnlock_buildID

*��  �Σ�

*����ֵ��

*��  ������ȡ¥��ID

*��  ע��
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
*��������GetIcUnlock_w_suffix

*��  �Σ�

*����ֵ��

*��  ������ȡ�±�

*��  ע��
******************************************************/
unsigned short GetIcUnlock_w_suffix(void)
{
	return Se_Cardinfo.suffix;
}
