#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

typedef enum
{
	KEY_NONE = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_OK,
	KEY_BACK
} KeyValue;

void Key_Init(void);
KeyValue Key_GetValue(void);

#endif
