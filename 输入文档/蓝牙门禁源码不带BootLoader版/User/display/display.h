#ifndef _DISPLAY_H
#define _DISPLAY_H


#include "stm32f4xx.h"
#include "./rtc/rtc.h"

void Write_Command(unsigned char com);

uint8_t Write_Data(uint8_t data);
void LCD_Init();
void display(uint8_t *buf,int n);
void display_time(void);

#endif