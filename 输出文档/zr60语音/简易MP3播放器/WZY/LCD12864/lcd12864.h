#ifndef   __LCD12864_H
#define   __LCD12864_H
#include  "sys.h"
#define   LCD_RS_PORT      GPIOB
#define   LCD_RW_PORT      GPIOE
#define   LCD_EN_PORT      GPIOE
#define   LCD_PSB_PORT     GPIOE
#define   LCD_BL_PORT      GPIOB

#define   LCD_D0_PORT      GPIOE
#define   LCD_D1_PORT      GPIOF
#define   LCD_D2_PORT      GPIOF
#define   LCD_D3_PORT      GPIOG
#define   LCD_D4_PORT      GPIOG
#define   LCD_D5_PORT      GPIOG
#define   LCD_D6_PORT      GPIOG
#define   LCD_D7_PORT      GPIOG

#define   LCD_D0_Pin       GPIO_Pin_6
#define   LCD_D1_Pin       GPIO_Pin_6
#define   LCD_D2_Pin       GPIO_Pin_11
#define   LCD_D3_Pin       GPIO_Pin_6
#define   LCD_D4_Pin       GPIO_Pin_7
#define   LCD_D5_Pin       GPIO_Pin_8
#define   LCD_D6_Pin       GPIO_Pin_12
#define   LCD_D7_Pin       GPIO_Pin_15

#define   LCD_RS_Pin       GPIO_Pin_1
#define   LCD_RW_Pin       GPIO_Pin_3
#define   LCD_EN_Pin       GPIO_Pin_4
#define   LCD_PSB_Pin      GPIO_Pin_5
#define   LCD_BL_Pin       GPIO_Pin_0

#define   LCD_RS           PBout(1)
#define   LCD_RW           PEout(3)
#define   LCD_EN           PEout(4)
#define   LCD_PSB          PEout(5)
#define   LCD_BL           PBout(0)

#define   LCD_D0_OUT       PEout(6)
#define   LCD_D1_OUT       PFout(6)
#define   LCD_D2_OUT       PFout(11)
#define   LCD_D3_OUT       PGout(6)
#define   LCD_D4_OUT       PGout(7)
#define   LCD_D5_OUT       PGout(8)
#define   LCD_D6_OUT       PGout(12)
#define   LCD_D7_OUT       PGout(15)

#define   LCD_D0_IN       PEin(6)
#define   LCD_D1_IN       PFin(6)
#define   LCD_D2_IN       PFin(11)
#define   LCD_D3_IN       PGin(6)
#define   LCD_D4_IN       PGin(7)
#define   LCD_D5_IN       PGin(8)
#define   LCD_D6_IN       PGin(12)
#define   LCD_D7_IN       PGin(15)

#define   LCD_BL_ON       GPIO_ResetBits(LCD_BL_PORT,LCD_BL_Pin);//点亮背光
#define   LCD_BL_OFF      GPIO_SetBits(LCD_BL_PORT,LCD_BL_Pin);//关闭背光
#define   LCD_CMD          0
#define   LCD_DATA         1
//基本指令集
#define   LCD_BASIC_SET    0x30
//扩充指令集
#define   LCD_EXTERN_SET   0x34
//绘图显示开  
#define   LCD_DRAW_ON      LCD_EXTERN_SET|0x02
//绘图显示关               
#define   LCD_DRAW_OFF     LCD_EXTERN_SET|0x00
//暂定 反白指令在扩充指令集中
#define   LCD_INVERSE_ONE    0x04|0x00
#define   LCD_INVERSE_TWO    0x04|0x01
#define   LCD_INVERSE_THREE  0x04|0x02
#define   LCD_INVERSE_FOUR   0x04|0x03

#define   ISTRUE(x)    (x>0?1:0)
void LCD_GPIO_Init(void);
uint8_t  ReadByteFromLCD(void);
void WriteByteToLCD(uint8_t byte);
uint8_t  LCD_Busy(void);
void  WriteToLCD(uint8_t mode,uint8_t byte);
void  LCD12864_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t x,uint8_t y);
void LCD_ShowString(uint8_t x,uint8_t y,uint8_t *s);
void LCD_SetPoint(uint8_t x,uint8_t y);
void  LCD_Clear_DGRAM(void);
void LCD_Refresh_GRAM(void);
void  LCD_DrawPoint(uint8_t x,uint8_t y,uint8_t mode);
void LCD_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
#endif

