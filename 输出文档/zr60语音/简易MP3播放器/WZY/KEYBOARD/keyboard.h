#ifndef   __KEYBOARD_H
#define   __KEYBOARD_H
#include  "sys.h"

#define   ROW_0_PORT    GPIOB
#define   ROW_1_PORT    GPIOB
#define   ROW_2_PORT    GPIOA
#define   ROW_3_PORT    GPIOC
#define   LINE_0_PORT   GPIOC
#define   LINE_1_PORT   GPIOD
#define   LINE_2_PORT   GPIOD
#define   LINE_3_PORT   GPIOE

#define   ROW_0_PIN      GPIO_Pin_6
#define   ROW_1_PIN      GPIO_Pin_7
#define   ROW_2_PIN      GPIO_Pin_0
#define   ROW_3_PIN      GPIO_Pin_7
#define   LINE_0_PIN     GPIO_Pin_13
#define   LINE_1_PIN     GPIO_Pin_6
#define   LINE_2_PIN     GPIO_Pin_7
#define   LINE_3_PIN     GPIO_Pin_2

#define   LINE_0_IN      PCin(13)
#define   LINE_1_IN      PDin(6)
#define   LINE_2_IN      PDin(7)
#define   LINE_3_IN      PEin(2)
#define   ROW_0_IN       PBin(6)
#define   ROW_1_IN       PBin(7)
#define   ROW_2_IN       PAin(0)
#define   ROW_3_IN       PCin(7)

void  KEYBoard_LRInit_LScan(void);
void  KEYBoard_LRInit_RScan(void);
void  KEYBoard_GPIOInit(void);
uint8_t KEYBoard_LRScan(void);
#endif
