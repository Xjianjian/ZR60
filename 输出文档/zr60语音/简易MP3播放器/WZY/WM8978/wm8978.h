#ifndef   __WM8978_H
#define   __WM8978_H

#include  "sys.h"

#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 

#define  WM8978_R0   0x00
#define  WM8978_R1   0x01
#define  WM8978_R2   0x02
#define  WM8978_R3   0x03
#define  WM8978_R4   0x04
#define  WM8978_R5   0x05
#define  WM8978_R6   0x06
#define  WM8978_R7   0x07
#define  WM8978_R8   0x08
#define  WM8978_R9   0x09
#define  WM8978_R10   0x0A
#define  WM8978_R11   0x0B
#define  WM8978_R12   0x0C
#define  WM8978_R13   0x0D
#define  WM8978_R14   0x0E
#define  WM8978_R15   0x0F
#define  WM8978_R16   0x10
#define  WM8978_R17   0x11
#define  WM8978_R18   0x12
#define  WM8978_R19   0x13
#define  WM8978_R20   0x14
#define  WM8978_R21   0x15
#define  WM8978_R22   0x16
#define  WM8978_R23   0x17
#define  WM8978_R24   0x18
#define  WM8978_R25   0x19
#define  WM8978_R26   0x1A
#define  WM8978_R27   0x1B
#define  WM8978_R28   0x1C
#define  WM8978_R29   0x1D
#define  WM8978_R30   0x1E
#define  WM8978_R31   0x1F
#define  WM8978_R32   0x20
#define  WM8978_R33   0x21
#define  WM8978_R34   0x22
#define  WM8978_R35   0x23
#define  WM8978_R36   0x24
#define  WM8978_R37   0x25
#define  WM8978_R38   0x26
#define  WM8978_R39   0x27
#define  WM8978_R40   0x28
#define  WM8978_R41   0x29
#define  WM8978_R42   0x2A
#define  WM8978_R43   0x2B
#define  WM8978_R44   0x2C
#define  WM8978_R45   0x2D
#define  WM8978_R46   0x2E
#define  WM8978_R47   0x2F
#define  WM8978_R48   0x30
#define  WM8978_R49   0x31
#define  WM8978_R50   0x32
#define  WM8978_R51   0x33
#define  WM8978_R52   0x34
#define  WM8978_R53   0x35
#define  WM8978_R54   0x36
#define  WM8978_R55   0x37

uint8_t WM8978_Write_Reg(uint8_t reg,uint16_t val);
uint16_t WM8978_Read_Reg(uint8_t reg);
uint8_t  WM8978_Init(void);
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen);
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen);
void WM8978_MIC_Gain(uint8_t gain);
void WM8978_LINEIN_Gain(u8 gain);
void WM8978_AUX_Gain(u8 gain);
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len);
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr);
void WM8978_SPKvol_Set(uint8_t volx);
void WM8978_3D_Set(uint8_t depth);
void WM8978_EQ_3D_Dir(uint8_t dir);
void WM8978_EQ1_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ2_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ3_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ4_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ5_Set(uint8_t cfreq,uint8_t gain);

#endif
