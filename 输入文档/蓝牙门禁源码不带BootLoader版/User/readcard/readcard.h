#ifndef __READCARD_H
#define	__READCARD_H



#include "stm32f4xx.h"
#include <stdio.h>
#include "./wdata/wdata.h"

#define OK  0x00             // ģ��ĺ������óɹ�
#define QUIT  0x1e           // ��һ�η�������ʱ�����
#define COMM_ERR  0xff       // ����ͨ�Ŵ���
#define NO_TAG_ERR  0x01     // ����Ч����û�п�
#define CRC_ERR  2           // �ӿ��н��յ��˴����CRCУ���
#define NOT_AUTH_ERR  0x0a   // ��û����֤
#define PARITY_ERR  5        // �ӿ��н��յ��˴����Ч��λ
#define BIT_COUNT_ERR  0x0b  // �ӿ��н��յ��˴���������λ



 #define On_Card  0x00
        /**
         * ����key
         */
#define Load_Key  0x01

        /**
         * ���Ƭ����ȡ��Ƭ��
         */
#define Get_CardSnr  0x02

        /**
         * ����Ϊ������鿨Ƭģʽ IDLEģʽ
         */
#define SetDetectCard_ALL  0x03

        /**
         * ����0x14 �洢��С��ID��ϢcommunityId
         */
#define Block_0x14  0x14

        /**
         * ����0x15 �洢��¥�������ϢbuildNum1
         */
#define Block_0x15  0x15

        /**
         * ����0x16 �����¥�������ϢbuildNum2
         */
#define Block_0x16  0x16

        /**
         * ���ݿ�10, ��IC���洢�û�ID��
         */
#define Block_0x10  0x10

        /**
         * ���ݿ�10, ��IC���洢�ֻ�����Ŀ�
         */
#define Block_0x11 0x11





typedef struct RebackInfo {
        /**
         * ��ʾ��
         */
        unsigned char seqnr;

        /**
         * ״̬��
         */
        unsigned char status;

	
				unsigned char length;
        /**
         * ��������
         */
        unsigned char data[20];

        /**
         * <pre>
         *     @author Ԭ��о
         *     @desc ״̬���ֶ�
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