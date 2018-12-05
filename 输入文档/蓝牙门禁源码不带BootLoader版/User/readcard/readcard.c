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
         * 请求天线范围内的所有卡
         */
    unsigned  char Set_Card_ALL[] = {0x20, 0x03, 0x25, 0x01, 0x01, 0x00, 0x03};
		

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
			        // 先计算正确的校验值
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
        blockCommand[4] = block;    // 要读的扇区的数据
        blockCommand[1] = block;    // 本次读取数据的请求头
        sendCommand(blockCommand,length);
 }
 void decodeOnCard(RebackInfo rebackInfo) {

        switch (rebackInfo.status) {
            case OK:
                printf("decodeOnCard: 检测到IC卡, 并且获取到了ic卡ID号\n");
                // char[] validData = new char[rebackInfo.data[3]];
                strncpy(card_num,rebackInfo.data+4,4);
                readBlock(Read_Block, (uint8_t) 0x14,sizeof(Read_Block));
                break;
            default:
                printf("decodeOnCard: 检测到IC卡, 获取IC卡号失败\n");
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
                // 装载秘钥成功, 接下来就是设置模块自动读卡
                sendCommand(Set_Card_ALL,sizeof(Set_Card_ALL));
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
 void decodeSetDetectCard(RebackInfo rebackInfo) {
//        char status = rebackInfo.status;

        switch (rebackInfo.status) {
            case OK:
                // 设置自动读卡成功
                //isSectedCard = true;
                break;
            default:
                // 设置自动读卡失败
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
                // 如果数据块0x14读取数据失败, 就读取0x10数据块的数据
                // 如果读取成功就读取数据块0x15的数据
                if (rebackInfo.status != OK) {
										isSectedCard_flag = 1;
                    printf( "decodeReadBlock: 0x14 fail\n");
										break;
                } else {
									
										printf("小区id:");
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
									printf("楼栋号:");
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
									printf("单元号:");
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
						
						// 标签位为0表示读到卡了
								decodeOnCard(rebackInfo);
                break;
            case Load_Key:           // 标签位为Load_Key表示这是从机给主机的装载秘钥的反馈
                //decodeLoadKeyResult(rebackInfo);
                break;
            case SetDetectCard_ALL: // 表示这是从机给主机的设置自动读卡的反馈
								//decodeSetDetectCard(rebackInfo);
                break;
            case Block_0x14:         // 表示这是从机读取扇区0x14的反馈
            case Block_0x15:         // 表示这是从机读取扇区0x15的反馈
            case Block_0x16:         // 表示这是从机读取扇区0x16的反馈
                decodeReadBlock(rebackInfo);
                break;
        }
 }
                                               
 
/**
  * @brief  检查读取到的小区ID是否属于本小区
  * @param  无
  * @retval 成功：1				失败：-1
  */
int country_check(unsigned char* buf1,unsigned char* buf2,u8 size)		//小区ID检查
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
		printf("小区id对比成功\n");
		return 1;
}

 /**
  * @brief  检查是否为工作人员
  * @param  无
  * @retval 成功：1				失败：-1
  */
int worker_check(void)		//检查是否是工作人员
{
	/*
	if(card_building[0] == 0xAAAA && card_building[1] == 0xAAAA && card_building[2] == 0xAAAA && card_building[3] == 0xAAAA &&
	 	 card_building[4] == 0xAAAA && card_building[5] == 0xAAAA && card_building[6] == 0xAAAA && card_building[7] == 0xAAAA)
	{
		return 1;		//这是工作人员的卡
	}
	else
	{
		return -1;		//对比失败，不能进入
	}*/
}

 /**
  * @brief  将读取到的两个数据块的数据整合到一起，方便对比
  * @param  无
  * @retval 成功：1				失败：-1
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
  * @brief  检查是否为本小区住户
  * @param  无
  * @retval 成功：1				失败：-1
  */
int user_check(unsigned char* buf1,unsigned char* buf2,u8 size,u8 mach_type)		//对比普通楼栋信息
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
	printf("当前为工作人员\n");
		return 1;
check_aa:	
	printf("对比楼栋\n");
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
						printf("对比成功，准备开门\n");
						
						return 1;
					}
				}
		}
	}
	return 0;
}
else
{
	printf("当前为围墙机\n");
	return 1;
}
	 
	
}

 /**
  * @brief  读卡成功以后的数据处理
  * @param  无
  * @retval 成功：1				失败：-1
  */
int card_data_handle(struct comm_info *read_test,struct comm_info *local_test,u8 mach_type)//读卡后的数据处理
{
	int ret = 0;

/*
	if(card_num = 挂失卡)
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
		printf("准备开门\n");
		open_flag = 1;
		return 1;
	}

	
	return 0;
}