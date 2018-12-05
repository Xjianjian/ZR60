#ifndef   __FONTUPD_H
#define   __FONTUPD_H
#include  "sys.h"


extern uint8_t* const GBK24_PATH;
extern uint8_t* const GBK16_PATH;
extern uint8_t* const GBK12_PATH;
extern uint8_t* const UNIGBK_PATH;
void FONT_FillInfo(void);
void  FONT_Init(void);
#endif

