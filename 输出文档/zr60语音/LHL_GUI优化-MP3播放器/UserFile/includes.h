#ifndef __INCLUDES_H_
#define	__INCLUDES_H_

#define F407_FREERTOS
/* Standard includes. */
#include <stdio.h>
#include "stdlib.h"
#include "arm_math.h"

#include "stm32f4xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f4xx.h"
/*******************************************************/
#include "Lcd.h"
#include "TouchScreen.h"
#include "sdio_sd.h"
#include "ff.h"
#include "diskio.h"
#include "LHL_Button.h"
#include "LHL_Font.h"
#include "LHL_GUICore.h"
#include "Bmp.h"
#include "LHL_NormalButton.h"
#include "BSP.h"
/*******************************************************/
#include "GUI_Process.h"
#include "myString.h"
#include "MGUI.h"

/*******************************************************/
#define  StackSize_GUIPROCESS				1024
enum{
	GUIPROCESSPro,
};
/**********************线程的频率定义********************/

/*********************与控制线程频率有关的变量************/

/**********************消息队列*************************/

#endif
