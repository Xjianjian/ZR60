#include "includes.h"
FATFS fs;
void BSP_Init(void)
{
	LCD_Init();
 	ADS7843_Init();
	LCD_Clear(65535);
 	f_mount(0, &fs);
 	LHL_FontInit();
	GUI_ALLOC_Init();
}
