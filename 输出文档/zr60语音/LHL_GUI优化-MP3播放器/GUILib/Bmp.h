#ifndef __Bmp_H_
#define __Bmp_H_
#include "ff.h"
#include "Lcd.h"
#include "stdio.h"
#define BMPBUFFERLEN	2048//图片最高分辨率为682*682
#define WORD					unsigned short
#define DWORD					unsigned int
#define LONG					long
#define BMP_ER				unsigned char
#define BOOL				unsigned char
#define TRUE					1
#define	FALSE					0
#define	BMPFILE			FIL
#define BmpOpenFile(BMPFilePoint,STRING)							f_open(BMPFilePoint,STRING,FA_OPEN_EXISTING | FA_READ)
#define	BmpReadData(BMPFilePoint,Buffer,Length,pn)		f_read(BMPFilePoint,Buffer,Length,pn)
#define BmpFileSeek(BMPFilePoint,Pos)									f_lseek(BMPFilePoint,Pos) 

#define bmp4(x,y)																			LCD_GetPoint(y,x)
/********************************/
#define		BF_OK					(BMP_ER)0
#define		BF_FILEER			(BMP_ER)1	
#define		BF_POINTER		(BMP_ER)2		
#define		BF_SIZEER			(BMP_ER)3	
/*********************************/
typedef struct tagBITMAPINFO
{
	BMPFILE File;
	DWORD bfSize;
	DWORD bfOffBits;
	LONG biWidth;
	LONG biHeight;
	WORD biBitCount;
	LONG biXPelsPerMeter;  
	LONG biYPelsPerMeter;
	unsigned char XtoY;
	float ZIP;
	short CutXB;
	short CutYB;
	short CutXE;
	short CutYE;
	short SizeAfterCutX;
	short SizeAfterCutY;
	unsigned char Transparent;
} BITMAPINFO; 

#include "LHL_GUICore.h"
#define	BmpDrawPoint(x,y,color)												GUICore_DrawPoint(x,y,color)	 

BMP_ER BmpLoad(BITMAPINFO *BmpInfo,const char *Setu);
BMP_ER BmpDraw(BITMAPINFO *BmpInfo,unsigned short XPos,unsigned short YPos);
BMP_ER BmpUnLoad(BITMAPINFO *BmpInfo);
BMP_ER BmpSwapXY(BITMAPINFO *BmpInfo,unsigned char NewState);
BMP_ER BmpZIP(BITMAPINFO *BmpInfo,float NewState);
BMP_ER BmpZIP_Size(BITMAPINFO *BmpInfo,short x,short y);
BMP_ER BmpCutPic(BITMAPINFO *BmpInfo,short xb,short yb,short xe,short ye);
BMP_ER BmpTransParent(BITMAPINFO *BmpInfo,unsigned char NewState);
int  BmpScreen_short(unsigned short int  x, unsigned  short int  y, unsigned  short int  Width, unsigned  short int  Height, FIL *File) ;
#endif
