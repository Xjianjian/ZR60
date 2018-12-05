#ifndef __LHL_FONT_H_
#define __LHL_FONT_H_
#include "LHL_GUICore.h"
enum{
	FONT_12x12,
	FONT_16x16
};
void LHL_FontSet(short Font);

void LHLFont_PutChinese16x16(unsigned short x,unsigned short y,char *p,unsigned short charColor,unsigned short bkColor,FONT_TYPE Type);
void LHL_PutChar8x16(unsigned short x,unsigned short y,char p,unsigned short charColor,unsigned short bkColor,FONT_TYPE Type);
void LHL_PutChar6x12(unsigned short x,unsigned short y,char p,unsigned short charColor,unsigned short bkColor,FONT_TYPE Type);
char LHL_FontInit(void);
void LHL_FontPutStringAt(unsigned short xPos,unsigned short yPos,char *ch,unsigned short Fontcolor
													,unsigned short Bkcolor,unsigned char Type);
void LHL_FontShowNumDec(unsigned short xPos,unsigned short yPos,int Num,unsigned short Fontcolor,
													unsigned short Bkcolor,unsigned char Type,unsigned char Len);
void LHL_FontShowNumFloat(unsigned short xPos,unsigned short yPos,float Num,unsigned short Fontcolor,
													unsigned short Bkcolor,unsigned char Type,unsigned char Len);
void LHL_FontShowStyle(unsigned char NewStyle);
int LHL_Fontcifang(int m);
char* LHL_Fontitoa(int n,unsigned char m);
void LHL_FontStr_Add(char *Str1 ,char *Str2);
int LHL_FontStr_length(char *Str);
long LHL_Fontatoi(char *Str);
int Str_equel(char *str1,char *str2);
int LHL_FontStr_Line(char *Str);
int LHL_GetNumDecLen(int m);

void LHLFont_DebugString(const char *pch);

#endif
