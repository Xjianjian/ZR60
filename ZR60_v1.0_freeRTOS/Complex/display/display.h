#ifndef _DISPLAY_H
#define _DISPLAY_H


#include "stm32f4xx.h"
#include "./hym8563/hym8563.h"

extern unsigned long now_time;


void Write_Command(unsigned char com);

uint8_t Write_Data(uint8_t data);
void LCD_Init(void);
void display(uint8_t *buf,int n);
void display_time(void);

#endif
