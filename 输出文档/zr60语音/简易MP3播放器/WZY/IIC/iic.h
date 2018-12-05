#ifndef   __IIC_H
#define   __IIC_H

#include  "sys.h"

#define   IIC_SCL_PORT   GPIOB
#define   IIC_SDA_PORT   GPIOB

#define   IIC_SCL_PIN    GPIO_Pin_8
#define   IIC_SDA_PIN    GPIO_Pin_9


#define   IIC_SDA_OUT()    GPIO_Set(IIC_SDA_PORT,IIC_SDA_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
#define   IIC_SDA_IN()     GPIO_Set(IIC_SDA_PORT,IIC_SDA_PIN,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);

#define   IIC_SCL              PBout(8)
#define   IIC_SDA_WRITE        PBout(9)
#define   IIC_SDA_READ         PBin(9)


void  IIC_Init(void);
void  IIC_Start(void);
void  IIC_Stop(void);
uint8_t IIC_WaitACK(void);
void  IIC_ACK(void);
void  IIC_NACK(void);
void  IIC_SendByte(uint8_t byte);

uint8_t IIC_ReadByte(uint8_t ack);
#endif
