#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
extern vu16 TextColor, BackColor; 

/* LCD color */
#define RED	            0XF800
#define GREEN           0X07E0
#define BLUE            0X001F  
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define BLACK           0
#define WHITE           0XFFFF  
#define YELLOW          0XFFF0

#define Bank1_LCD_R    ((uint32_t)0x60000000)    //disp Reg ADDR
#define Bank1_LCD_D    ((uint32_t)0x60020000)	//disp Data ADDR

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void LCD_Init(void);
void LCD_SetTextColor(vu16 Color);
void LCD_SetBackColor(vu16 Color);
void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
u16 LCD_GetPoint(u16 x,u16 y);
u16 LCD_GetPoint(u16 x,u16 y);
void LCD_SetPoint(u16 x,u16 y);
void LCD_DrawSinglePoint(u16 x,u16 y,u16 color);
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2);
void LCD_DrawRect(u8 x1, u16 y1,u8 x2 ,u16 y2 );
void LCD_DrawCircle(u8 Xpos, u16 Ypos, u16 Radius);
void LCD_setwindow(unsigned char x0,unsigned int y0,unsigned char x1,unsigned int y1);

/*----- Medium layer function -----*/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

u16 LCD_BGR2RGB(u16 c);

/*----- Low layer function -----*/
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);

#endif 
