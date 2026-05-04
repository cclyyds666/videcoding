#include "Key.h"
#include "Delay.h"

#define KEY_PORT        GPIOB
#define KEY_UP_PIN      GPIO_Pin_12
#define KEY_DOWN_PIN    GPIO_Pin_13
#define KEY_OK_PIN      GPIO_Pin_14
#define KEY_BACK_PIN    GPIO_Pin_15

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN | KEY_DOWN_PIN | KEY_OK_PIN | KEY_BACK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

KeyValue Key_GetValue(void)
{
	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_UP_PIN) == Bit_RESET)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_UP_PIN) == Bit_RESET)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_UP_PIN) == Bit_RESET);
			return KEY_UP;
		}
	}

	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_DOWN_PIN) == Bit_RESET)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_DOWN_PIN) == Bit_RESET)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_DOWN_PIN) == Bit_RESET);
			return KEY_DOWN;
		}
	}

	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_OK_PIN) == Bit_RESET)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_OK_PIN) == Bit_RESET)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_OK_PIN) == Bit_RESET);
			return KEY_OK;
		}
	}

	if (GPIO_ReadInputDataBit(KEY_PORT, KEY_BACK_PIN) == Bit_RESET)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_BACK_PIN) == Bit_RESET)
		{
			while (GPIO_ReadInputDataBit(KEY_PORT, KEY_BACK_PIN) == Bit_RESET);
			return KEY_BACK;
		}
	}

	return KEY_NONE;
}
