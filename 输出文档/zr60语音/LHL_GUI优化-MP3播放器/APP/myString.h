#ifndef __MYSTRING_H_
#define __MYSTRING_H_
#define FONTTYPE			unsigned char
enum
{
	FONT_BLACK,//ºÚÌå
	FONT_CH_64x64,
};
extern const unsigned char Font_Black[];
extern const unsigned char Font_CH_64x64[];
extern const unsigned char gImage_Welcom[391];
extern const unsigned char gImage_Locker[391];
extern const unsigned char gImage_temprature[198];
extern const unsigned char gImage_USERManager[686];
extern const unsigned char gImage_Right[1656];
extern const unsigned char gImage_Error[1532];
extern const unsigned char gImage_Warming[486];
void MyString_PutCharAt(char ch,FONTTYPE fonttype,short x,short y,
								unsigned short Fontcolor,
								unsigned short Bkcolor,
								unsigned char isTransParent);
void MyString_PutStringAt(char *pch,FONTTYPE fonttype,short x,short y,
								unsigned short Fontcolor,
								unsigned short Bkcolor,
								unsigned char isTransParent);
void DisplayDigit(int iNumber,short PosX,short PosY,float m,unsigned short color,short Len);

void ImageShow(const unsigned char *ch,short x,short y,
								unsigned short Bkcolor,unsigned short Color);
#endif
