#ifndef  __TEXT_H
#define  __TEXT_H
#include "sys.h"

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义
extern struct FileInfo fileinfo_f12,fileinfo_f16,fileinfo_f24;
extern struct FileInfo  fileinfo_gbkuni;
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);
void Show_Font(unsigned char x,unsigned char y,u8 *font,u8 size);
void Show_Str(unsigned char x,unsigned char y,u8*str,u8 size);
void  unigbk_exchange(u8* src,u8* dst,u8 mode);
unsigned short unigbk(unsigned short src,unsigned char dir);
u8 text_hex2chr(u8 hex);
u8 text_chr2hex(u8 chr);
void  hexstring2string(struct FileInfo FileInfo,u8* dstbuf);
#endif
