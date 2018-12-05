#ifndef __READCARD_H
#define	__READCARD_H



#include "stm32f4xx.h"
#include <stdio.h>
#include "./wdata/wdata.h"

#define OK  0x00             // 模块的函数调用成功
#define QUIT  0x1e           // 上一次发送命令时被打断
#define COMM_ERR  0xff       // 串行通信错误
#define NO_TAG_ERR  0x01     // 在有效期内没有卡
#define CRC_ERR  2           // 从卡中接收到了错误的CRC校验和
#define NOT_AUTH_ERR  0x0a   // 卡没有验证
#define PARITY_ERR  5        // 从卡中接收到了错误的效验位
#define BIT_COUNT_ERR  0x0b  // 从卡中接收到了错误数量的位



 #define On_Card  0x00
        /**
         * 加载key
         */
#define Load_Key  0x01

        /**
         * 激活卡片并获取卡片号
         */
#define Get_CardSnr  0x02

        /**
         * 设置为主动检查卡片模式 IDLE模式
         */
#define SetDetectCard_ALL  0x03

        /**
         * 扇区0x14 存储了小区ID信息communityId
         */
#define Block_0x14  0x14

        /**
         * 扇区0x15 存储了楼栋编号信息buildNum1
         */
#define Block_0x15  0x15

        /**
         * 扇区0x16 村粗了楼栋编号信息buildNum2
         */
#define Block_0x16  0x16

        /**
         * 数据块10, 旧IC卡存储用户ID块
         */
#define Block_0x10  0x10

        /**
         * 数据块10, 旧IC卡存储手机号码的块
         */
#define Block_0x11 0x11





typedef struct RebackInfo {
        /**
         * 标示符
         */
        unsigned char seqnr;

        /**
         * 状态码
         */
        unsigned char status;

	
				unsigned char length;
        /**
         * 数据数组
         */
        unsigned char data[20];

        /**
         * <pre>
         *     @author 袁光芯
         *     @desc 状态码字段
         *     @date 2017-04-28
         * </pre>
         */
}RebackInfo;
void calcCommandsBBC(uint8_t *command,uint8_t length); 
 void  sendCommand(uint8_t *command,uint8_t length);
 void readBlock(uint8_t * blockCommand, uint8_t  block,uint8_t length);
 void decodeOnCard(RebackInfo rebackInfo);
 void decodeLoadKeyResult(RebackInfo rebackInfo);
 void decodeReadBlock(RebackInfo rebackInfo);
void handleMsg(RebackInfo rebackInfo);
void init_serial(void);
//int card_data_handle(struct comm_info *read_test,struct comm_info *local_test);

int card_data_handle(struct comm_info *read_test,struct comm_info *local_test,u8 mach_type);

#endif