#ifndef   __AT24C02_H
#define   __AT24C02_H
#include  "sys.h"

void AT24CXX_Init(void);
u8 AT24CXX_ReadOneByte(u16 ReadAddr);
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);
u8 AT24CXX_Check(void);
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);

#endif
