 #include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./usart3/usart3.h"

#include "./tim/bsp_basic_tim.h"

#include "./readcard/readcard.h"
#include "./wdata/wdata.h"

#include "string.h"
 
 
 
 uint8_t setcard = 0;
 uint8_t isSectedCard_flag = 0;
 struct comm_info sminfo;
 struct comm_info sminfo1;
  RebackInfo  rebackinfo;
	u8 open_flag = 0;
	u8 open_door = 0;

	extern u8 card_set;
	unsigned char card_num[4];
   unsigned  char L_Key[]= {0x20, 0x01, 0x20, 0x06, 0xF5, 0x79, 0xED, 0xD8, 0x9F, 0x77, 0x00, 0x03};
	
        /**
         * �������߷�Χ�ڵ����п�
         */
    unsigned  char Set_Card_ALL[] = {0x20, 0x03, 0x25, 0x01, 0x01, 0x00, 0x03};
		

        /**
         * ��ȡģ�������:
         * 0x20��ʾ��ʼ��
         * 0x00��ʾ����(��ʵ��������д��ģ��, ģ��ԭ������)
         * 0x22��ʾ�Ƕ�����������
         * 0x01��ʾ����Я��������ֻ��һλ
         * 0x02��ʾ���ζ�ȡ��������02�е�����,
         * 0x00��ʾ������֤��, ��Ҫ����
         * 0x03��ʾ��������Ľ�����
         */
   unsigned     char Read_Block[] = {0x20, 0x00, 0x22, 0x01, 0x02, 0x00, 0x03};


	 
	 
void calcCommandsBBC(uint8_t *command,uint8_t length) 
	{

				u8 i;

        uint8_t c = command[1];
        for (i = 2; i < length - 2; i++) {
            c ^= command[i];
        }

        command[length - 2] = (uint8_t) ~c;

 }
	 
	 
 void  sendCommand(uint8_t *command,uint8_t length)
{
			        // �ȼ�����ȷ��У��ֵ
				u8 i;
			
      calcCommandsBBC(command,length);
			for(i = 0;i < length;i++)
			{
				Usart_SendByte(USART3, command[i]);
				printf("%#x ",command[i]);
			}	
			printf("\n ");
}
void init_serial()
{
	u8 i;
	//sendCommand(L_Key,sizeof(L_Key));
	sendCommand(Set_Card_ALL,sizeof(Set_Card_ALL));
	printf("init send: ");
				 		for(i = 0;i < sizeof(Set_Card_ALL);i++)
					{
					printf("%#x ",Set_Card_ALL[i]);
					}
					
					printf("\n ");
	
}

 void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length) {
        blockCommand[4] = block;    // Ҫ��������������
        blockCommand[1] = block;    // ���ζ�ȡ���ݵ�����ͷ
        sendCommand(blockCommand,length);
 }
 void decodeOnCard(RebackInfo rebackInfo) {

        switch (rebackInfo.status) {
            case OK:
                printf("decodeOnCard: ��⵽IC��, ���һ�ȡ����ic��ID��\n");
                // char[] validData = new char[rebackInfo.data[3]];
                strncpy(card_num,rebackInfo.data+4,4);
                readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));
                break;
            default:
                printf("decodeOnCard: ��⵽IC��, ��ȡIC����ʧ��\n");
                //cardInfo.cardId = null;
                //handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
                //OpenDoorManager.playUselessVoice();
                break;
        }
  }

 void decodeLoadKeyResult(RebackInfo rebackInfo) {
     //   char status = rebackInfo.status;

        switch (rebackInfo.status) {
            case OK:
                // װ����Կ�ɹ�, ��������������ģ���Զ�����
                sendCommand(Set_Card_ALL,sizeof(Set_Card_ALL));
                break;
            default:

                // װ����Կ���ɹ�,���·���
								
                //restartMifareRead();
                break;
        }
    }
 
	    /**
     * DB
     * ���������Զ�����
     *
     * @param rebackInfo
     */
 void decodeSetDetectCard(RebackInfo rebackInfo) {
//        char status = rebackInfo.status;

        switch (rebackInfo.status) {
            case OK:
                // �����Զ������ɹ�
                //isSectedCard = true;
                break;
            default:
                // �����Զ�����ʧ��
              //  restartMifareRead();
                break;
        }
    }
	
		



		

		
		
 void decodeReadBlock(RebackInfo rebackInfo) {
       // char status = rebackInfo.status;
       // char seqnr = rebackInfo.seqnr;
				u8 i;
        switch (rebackInfo.seqnr) {

             case Block_0x14:
                // ������ݿ�0x14��ȡ����ʧ��, �Ͷ�ȡ0x10���ݿ������
                // �����ȡ�ɹ��Ͷ�ȡ���ݿ�0x15������
                if (rebackInfo.status != OK) {
										isSectedCard_flag = 1;
                    printf( "decodeReadBlock: 0x14 fail\n");
										break;
                } else {
									
										printf("С��id:");
										for(i = 0;i < rebackInfo.length;i++)
									{
									printf("%x ",rebackinfo.data[i]);
										
										sminfo.community_id[i] = rebackinfo.data[i];
										//if(card_set == 1)
									//	{
											//sminfo1.community_id[i] = rebackinfo.data[i];
									//	}
									}
									printf("\n");
									
                    readBlock(Read_Block, (uint8_t) 0x15,sizeof(Read_Block));
                  //  printf("decodeReadBlock: 0x14 success\n");
                }
                break;
            case Block_0x15:
                if (rebackInfo.status != OK) {

                  //  printf("decodeReadBlock: Block_0x15 fail\n");
									isSectedCard_flag = 1;
									break;
                } else {
											
                    //printf("decodeReadBlock: Block_0x15 success\n");
                }
									printf("¥����:");
										for(i = 0;i < rebackInfo.length;i++)
									{
									printf("%x ",rebackinfo.data[i]);
										sminfo.build_num[i]= rebackinfo.data[i];
									//	if(card_set == 1)
									//	{
											//	sminfo1.build_num[i]= rebackinfo.data[i];
									//	}
					
									}
									
									printf("\n");
                readBlock(Read_Block, (uint8_t) 0x16,sizeof(Read_Block));
                break;
            case Block_0x16:
                if (rebackInfo.status != OK) {
                    printf("decodeReadBlock: Block_0x16 fail\n");
										isSectedCard_flag = 1;
									break;
                } else {
                   // printf("decodeReadBlock: Block_0x16 success");
                }
									printf("��Ԫ��:");
									for(i = 0;i < rebackInfo.length;i++)
									{
										printf("%x ",rebackinfo.data[i]);
										sminfo.cell_num[i] = rebackinfo.data[i];

									}
									
									printf("\n");

							
//                handler.sendEmptyMessageDelayed(ReadMifareHandler.READ_DELAY, 1000);
									isSectedCard_flag = 1;

									
									setcard =1;

									open_door = 1;
                break;
        }
    }		
		
 void handleMsg(RebackInfo rebackInfo) 
{
 

        switch (rebackInfo.seqnr) 
					{
						case On_Card :  
						
						// ��ǩλΪ0��ʾ��������
								decodeOnCard(rebackInfo);
                break;
            case Load_Key:           // ��ǩλΪLoad_Key��ʾ���Ǵӻ���������װ����Կ�ķ���
                //decodeLoadKeyResult(rebackInfo);
                break;
            case SetDetectCard_ALL: // ��ʾ���Ǵӻ��������������Զ������ķ���
								//decodeSetDetectCard(rebackInfo);
                break;
            case Block_0x14:         // ��ʾ���Ǵӻ���ȡ����0x14�ķ���
            case Block_0x15:         // ��ʾ���Ǵӻ���ȡ����0x15�ķ���
            case Block_0x16:         // ��ʾ���Ǵӻ���ȡ����0x16�ķ���
                decodeReadBlock(rebackInfo);
                break;
        }
 }
                                               
 
/**
  * @brief  ����ȡ����С��ID�Ƿ����ڱ�С��
  * @param  ��
  * @retval �ɹ���1				ʧ�ܣ�-1
  */
int country_check(unsigned char* buf1,unsigned char* buf2,u8 size)		//С��ID���
{
		u8 i;
		for(i = 0;i < size;i++)
		{
				printf("%x_",buf1[i]);
				if(buf1[i] != buf2[i] )
				{
					return 0;
				}
				
			
		}
		printf("С��id�Աȳɹ�\n");
		return 1;
}

 /**
  * @brief  ����Ƿ�Ϊ������Ա
  * @param  ��
  * @retval �ɹ���1				ʧ�ܣ�-1
  */
int worker_check(void)		//����Ƿ��ǹ�����Ա
{
	/*
	if(card_building[0] == 0xAAAA && card_building[1] == 0xAAAA && card_building[2] == 0xAAAA && card_building[3] == 0xAAAA &&
	 	 card_building[4] == 0xAAAA && card_building[5] == 0xAAAA && card_building[6] == 0xAAAA && card_building[7] == 0xAAAA)
	{
		return 1;		//���ǹ�����Ա�Ŀ�
	}
	else
	{
		return -1;		//�Ա�ʧ�ܣ����ܽ���
	}*/
}

 /**
  * @brief  ����ȡ�����������ݿ���������ϵ�һ�𣬷���Ա�
  * @param  ��
  * @retval �ɹ���1				ʧ�ܣ�-1
  */
int card_building_handle(void)
{
	/*
	int i = 0;
	for(i = 0;i < 8;i++)
	{
		card_building[i] = card_building1[i];
	}
	for(i = 0;i < 8;i++)
	{
		card_building[i+8] = card_building2[i];
	}
	return 1;*/
}

 /**
  * @brief  ����Ƿ�Ϊ��С��ס��
  * @param  ��
  * @retval �ɹ���1				ʧ�ܣ�-1
  */
int user_check(unsigned char* buf1,unsigned char* buf2,u8 size,u8 mach_type)		//�Ա���ͨ¥����Ϣ
{
	int i ,j;



	
//#if (build_type == 1)
if(mach_type == 2)
{

	for(i = 0;i < size;i++)
	{
		printf("%x_",buf1[i]);
			if(buf1[i] != 0xAA)
			{
				goto check_aa;
			}
			
	}
	printf("��ǰΪ������Ա\n");
		return 1;
check_aa:	
	printf("�Ա�¥��\n");
		for(i = 0;i < 16;i++)
	{
		printf("%#x ",buf1[i]);
	}
	printf("\n");
	printf("123456\n");
		for(j = 0;j < 16;j++)
		{
				printf("%#x ",buf2[j]);
		}
		printf("\n");
	for(i = 0;i < size/2;i++)
	{
		for(j = 0;j < size/2;j++)
		{
			if(buf1[i*2+1] !=0xff && buf1[j*2+1]!= 0xff)
		  {
					if((buf1[i*2] == buf2[j*2]) && (buf1[i*2+1] == buf2[j*2+1]))
					{
						printf("buf1[i*2] is %d\n",buf1[i*2]);
						printf("buf1[i*2] is %d\n",buf1[i*2+1]);
						printf("buf1[i*2] is %d\n",buf2[j*2]);
						printf("buf1[i*2] is %d\n",buf2[j*2+1]);
						printf("�Աȳɹ���׼������\n");
						
						return 1;
					}
				}
		}
	}
	return 0;
}
else
{
	printf("��ǰΪΧǽ��\n");
	return 1;
}
	 
	
}

 /**
  * @brief  �����ɹ��Ժ�����ݴ���
  * @param  ��
  * @retval �ɹ���1				ʧ�ܣ�-1
  */
int card_data_handle(struct comm_info *read_test,struct comm_info *local_test,u8 mach_type)//����������ݴ���
{
	int ret = 0;

/*
	if(card_num = ��ʧ��)
	{
		
		
	}
*/
	u8 j;
	
	printf("local_test->build_num:");
			for(j = 0;j < 16;j++)
		{
				printf("%#x ",local_test->build_num[j]);
		}
		printf("\n");
	
	
	if(country_check(read_test->community_id,local_test->community_id,16) == 1 && user_check(read_test->build_num,local_test->build_num,16,mach_type) == 1 )
	{
		printf("׼������\n");
		open_flag = 1;
		return 1;
	}

	
	return 0;
}