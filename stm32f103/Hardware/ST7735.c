#include "ST7735.h"
#include "Delay.h"

#define ST7735_X_OFFSET   2
#define ST7735_Y_OFFSET   3

#define ST7735_SCL_PORT   GPIOA
#define ST7735_SCL_PIN    GPIO_Pin_5
#define ST7735_SDA_PORT   GPIOA
#define ST7735_SDA_PIN    GPIO_Pin_7
#define ST7735_CS_PORT    GPIOA
#define ST7735_CS_PIN     GPIO_Pin_4
#define ST7735_DC_PORT    GPIOA
#define ST7735_DC_PIN     GPIO_Pin_3
#define ST7735_RST_PORT   GPIOA
#define ST7735_RST_PIN    GPIO_Pin_2
#define ST7735_BL_PORT    GPIOB
#define ST7735_BL_PIN     GPIO_Pin_0
#define ST7735_BL_TIM     TIM3
#define ST7735_BL_MAX     1000

#define ST7735_SWRESET    0x01
#define ST7735_SLPOUT     0x11
#define ST7735_COLMOD     0x3A
#define ST7735_MADCTL     0x36
#define ST7735_CASET      0x2A
#define ST7735_RASET      0x2B
#define ST7735_RAMWR      0x2C
#define ST7735_INVON      0x21
#define ST7735_NORON      0x13
#define ST7735_DISPON     0x29

static const uint8_t Font5x7[][5] = {
	{0x00,0x00,0x00,0x00,0x00}, /*   */
	{0x00,0x00,0x5F,0x00,0x00}, /* ! */
	{0x00,0x07,0x00,0x07,0x00}, /* " */
	{0x14,0x7F,0x14,0x7F,0x14}, /* # */
	{0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
	{0x23,0x13,0x08,0x64,0x62}, /* % */
	{0x36,0x49,0x55,0x22,0x50}, /* & */
	{0x00,0x05,0x03,0x00,0x00}, /* ' */
	{0x00,0x1C,0x22,0x41,0x00}, /* ( */
	{0x00,0x41,0x22,0x1C,0x00}, /* ) */
	{0x14,0x08,0x3E,0x08,0x14}, /* * */
	{0x08,0x08,0x3E,0x08,0x08}, /* + */
	{0x00,0x50,0x30,0x00,0x00}, /* , */
	{0x08,0x08,0x08,0x08,0x08}, /* - */
	{0x00,0x60,0x60,0x00,0x00}, /* . */
	{0x20,0x10,0x08,0x04,0x02}, /* / */
	{0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
	{0x00,0x42,0x7F,0x40,0x00}, /* 1 */
	{0x42,0x61,0x51,0x49,0x46}, /* 2 */
	{0x21,0x41,0x45,0x4B,0x31}, /* 3 */
	{0x18,0x14,0x12,0x7F,0x10}, /* 4 */
	{0x27,0x45,0x45,0x45,0x39}, /* 5 */
	{0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
	{0x01,0x71,0x09,0x05,0x03}, /* 7 */
	{0x36,0x49,0x49,0x49,0x36}, /* 8 */
	{0x06,0x49,0x49,0x29,0x1E}, /* 9 */
	{0x00,0x36,0x36,0x00,0x00}, /* : */
	{0x00,0x56,0x36,0x00,0x00}, /* ; */
	{0x08,0x14,0x22,0x41,0x00}, /* < */
	{0x14,0x14,0x14,0x14,0x14}, /* = */
	{0x00,0x41,0x22,0x14,0x08}, /* > */
	{0x02,0x01,0x51,0x09,0x06}, /* ? */
	{0x32,0x49,0x79,0x41,0x3E}, /* @ */
	{0x7E,0x11,0x11,0x11,0x7E}, /* A */
	{0x7F,0x49,0x49,0x49,0x36}, /* B */
	{0x3E,0x41,0x41,0x41,0x22}, /* C */
	{0x7F,0x41,0x41,0x22,0x1C}, /* D */
	{0x7F,0x49,0x49,0x49,0x41}, /* E */
	{0x7F,0x09,0x09,0x09,0x01}, /* F */
	{0x3E,0x41,0x49,0x49,0x7A}, /* G */
	{0x7F,0x08,0x08,0x08,0x7F}, /* H */
	{0x00,0x41,0x7F,0x41,0x00}, /* I */
	{0x20,0x40,0x41,0x3F,0x01}, /* J */
	{0x7F,0x08,0x14,0x22,0x41}, /* K */
	{0x7F,0x40,0x40,0x40,0x40}, /* L */
	{0x7F,0x02,0x0C,0x02,0x7F}, /* M */
	{0x7F,0x04,0x08,0x10,0x7F}, /* N */
	{0x3E,0x41,0x41,0x41,0x3E}, /* O */
	{0x7F,0x09,0x09,0x09,0x06}, /* P */
	{0x3E,0x41,0x51,0x21,0x5E}, /* Q */
	{0x7F,0x09,0x19,0x29,0x46}, /* R */
	{0x46,0x49,0x49,0x49,0x31}, /* S */
	{0x01,0x01,0x7F,0x01,0x01}, /* T */
	{0x3F,0x40,0x40,0x40,0x3F}, /* U */
	{0x1F,0x20,0x40,0x20,0x1F}, /* V */
	{0x3F,0x40,0x38,0x40,0x3F}, /* W */
	{0x63,0x14,0x08,0x14,0x63}, /* X */
	{0x07,0x08,0x70,0x08,0x07}, /* Y */
	{0x61,0x51,0x49,0x45,0x43}, /* Z */
	{0x00,0x7F,0x41,0x41,0x00}, /* [ */
	{0x02,0x04,0x08,0x10,0x20}, /* \ */
	{0x00,0x41,0x41,0x7F,0x00}, /* ] */
	{0x04,0x02,0x01,0x02,0x04}, /* ^ */
	{0x40,0x40,0x40,0x40,0x40}, /* _ */
	{0x00,0x01,0x02,0x04,0x00}, /* ` */
	{0x20,0x54,0x54,0x54,0x78}, /* a */
	{0x7F,0x48,0x44,0x44,0x38}, /* b */
	{0x38,0x44,0x44,0x44,0x20}, /* c */
	{0x38,0x44,0x44,0x48,0x7F}, /* d */
	{0x38,0x54,0x54,0x54,0x18}, /* e */
	{0x08,0x7E,0x09,0x01,0x02}, /* f */
	{0x0C,0x52,0x52,0x52,0x3E}, /* g */
	{0x7F,0x08,0x04,0x04,0x78}, /* h */
	{0x00,0x44,0x7D,0x40,0x00}, /* i */
	{0x20,0x40,0x44,0x3D,0x00}, /* j */
	{0x7F,0x10,0x28,0x44,0x00}, /* k */
	{0x00,0x41,0x7F,0x40,0x00}, /* l */
	{0x7C,0x04,0x18,0x04,0x78}, /* m */
	{0x7C,0x08,0x04,0x04,0x78}, /* n */
	{0x38,0x44,0x44,0x44,0x38}, /* o */
	{0x7C,0x14,0x14,0x14,0x08}, /* p */
	{0x08,0x14,0x14,0x18,0x7C}, /* q */
	{0x7C,0x08,0x04,0x04,0x08}, /* r */
	{0x48,0x54,0x54,0x54,0x20}, /* s */
	{0x04,0x3F,0x44,0x40,0x20}, /* t */
	{0x3C,0x40,0x40,0x20,0x7C}, /* u */
	{0x1C,0x20,0x40,0x20,0x1C}, /* v */
	{0x3C,0x40,0x30,0x40,0x3C}, /* w */
	{0x44,0x28,0x10,0x28,0x44}, /* x */
	{0x0C,0x50,0x50,0x50,0x3C}, /* y */
	{0x44,0x64,0x54,0x4C,0x44}, /* z */
	{0x00,0x08,0x36,0x41,0x00}, /* { */
	{0x00,0x00,0x7F,0x00,0x00}, /* | */
	{0x00,0x41,0x36,0x08,0x00}, /* } */
	{0x08,0x04,0x08,0x10,0x08}  /* ~ */
};

static void ST7735_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ST7735_SCL_PIN | ST7735_SDA_PIN | ST7735_CS_PIN |
	                              ST7735_DC_PIN | ST7735_RST_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = ST7735_BL_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(ST7735_CS_PORT, ST7735_CS_PIN);
	GPIO_SetBits(ST7735_SCL_PORT, ST7735_SCL_PIN);
	GPIO_SetBits(ST7735_SDA_PORT, ST7735_SDA_PIN);
	GPIO_SetBits(ST7735_RST_PORT, ST7735_RST_PIN);
}

static void ST7735_BacklightPWM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Period = ST7735_BL_MAX - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ST7735_BL_TIM, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(ST7735_BL_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(ST7735_BL_TIM, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(ST7735_BL_TIM, ENABLE);
	TIM_Cmd(ST7735_BL_TIM, ENABLE);
}

static void ST7735_WriteByte(uint8_t data)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		GPIO_ResetBits(ST7735_SCL_PORT, ST7735_SCL_PIN);

		if (data & 0x80)
		{
			GPIO_SetBits(ST7735_SDA_PORT, ST7735_SDA_PIN);
		}
		else
		{
			GPIO_ResetBits(ST7735_SDA_PORT, ST7735_SDA_PIN);
		}

		GPIO_SetBits(ST7735_SCL_PORT, ST7735_SCL_PIN);
		data <<= 1;
	}
}

static void ST7735_WriteCommand(uint8_t command)
{
	GPIO_ResetBits(ST7735_CS_PORT, ST7735_CS_PIN);
	GPIO_ResetBits(ST7735_DC_PORT, ST7735_DC_PIN);
	ST7735_WriteByte(command);
	GPIO_SetBits(ST7735_CS_PORT, ST7735_CS_PIN);
}

static void ST7735_WriteData(uint8_t data)
{
	GPIO_ResetBits(ST7735_CS_PORT, ST7735_CS_PIN);
	GPIO_SetBits(ST7735_DC_PORT, ST7735_DC_PIN);
	ST7735_WriteByte(data);
	GPIO_SetBits(ST7735_CS_PORT, ST7735_CS_PIN);
}

static void ST7735_WriteData16(uint16_t data)
{
	ST7735_WriteData(data >> 8);
	ST7735_WriteData(data & 0xFF);
}

static void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	x0 += ST7735_X_OFFSET;
	x1 += ST7735_X_OFFSET;
	y0 += ST7735_Y_OFFSET;
	y1 += ST7735_Y_OFFSET;

	ST7735_WriteCommand(ST7735_CASET);
	ST7735_WriteData16(x0);
	ST7735_WriteData16(x1);

	ST7735_WriteCommand(ST7735_RASET);
	ST7735_WriteData16(y0);
	ST7735_WriteData16(y1);

	ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_SetBacklight(uint8_t enabled)
{
	if (enabled)
	{
		ST7735_SetBacklightLevel(100);
	}
	else
	{
		ST7735_SetBacklightLevel(0);
	}
}

void ST7735_SetBacklightLevel(uint8_t level)
{
	uint16_t pulse;

	if (level > 100)
	{
		level = 100;
	}

	pulse = (uint16_t)((uint32_t)level * ST7735_BL_MAX / 100);
	TIM_SetCompare3(ST7735_BL_TIM, pulse);
}

void ST7735_SetRotation(uint8_t rotation)
{
	ST7735_WriteCommand(ST7735_MADCTL);

	switch (rotation & 0x03)
	{
		case 0:
			ST7735_WriteData(0xC8);
			break;
		case 1:
			ST7735_WriteData(0xA8);
			break;
		case 2:
			ST7735_WriteData(0x08);
			break;
		default:
			ST7735_WriteData(0x68);
			break;
	}
}

void ST7735_Init(void)
{
	ST7735_GPIO_Init();
	ST7735_BacklightPWM_Init();
	ST7735_SetBacklightLevel(0);

	GPIO_ResetBits(ST7735_RST_PORT, ST7735_RST_PIN);
	Delay_ms(50);
	GPIO_SetBits(ST7735_RST_PORT, ST7735_RST_PIN);
	Delay_ms(120);

	ST7735_WriteCommand(ST7735_SWRESET);
	Delay_ms(150);

	ST7735_WriteCommand(ST7735_SLPOUT);
	Delay_ms(150);

	ST7735_WriteCommand(ST7735_COLMOD);
	ST7735_WriteData(0x05);
	Delay_ms(10);

	ST7735_SetRotation(1);

	ST7735_WriteCommand(ST7735_INVON);
	ST7735_WriteCommand(ST7735_NORON);
	Delay_ms(10);

	ST7735_WriteCommand(ST7735_DISPON);
	Delay_ms(120);

	ST7735_SetBacklightLevel(75);
	ST7735_FillScreen(ST7735_BLACK);
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
	{
		return;
	}

	ST7735_SetAddressWindow(x, y, x, y);
	ST7735_WriteData16(color);
}

void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint32_t pixels;

	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
	{
		return;
	}

	if ((x + w) > ST7735_WIDTH)
	{
		w = ST7735_WIDTH - x;
	}
	if ((y + h) > ST7735_HEIGHT)
	{
		h = ST7735_HEIGHT - y;
	}

	ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1);

	GPIO_ResetBits(ST7735_CS_PORT, ST7735_CS_PIN);
	GPIO_SetBits(ST7735_DC_PORT, ST7735_DC_PIN);
	for (pixels = (uint32_t)w * h; pixels > 0; pixels--)
	{
		ST7735_WriteByte(color >> 8);
		ST7735_WriteByte(color & 0xFF);
	}
	GPIO_SetBits(ST7735_CS_PORT, ST7735_CS_PIN);
}

void ST7735_FillScreen(uint16_t color)
{
	ST7735_FillRect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if (w == 0 || h == 0)
	{
		return;
	}

	ST7735_FillRect(x, y, w, 1, color);
	ST7735_FillRect(x, y + h - 1, w, 1, color);
	ST7735_FillRect(x, y, 1, h, color);
	ST7735_FillRect(x + w - 1, y, 1, h, color);
}

void ST7735_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bgColor, uint8_t scale)
{
	uint8_t col;
	uint8_t row;
	uint8_t line;
	uint16_t px;
	uint16_t py;

	if (scale == 0)
	{
		scale = 1;
	}

	if (ch < ' ' || ch > '~')
	{
		ch = '?';
	}

	for (col = 0; col < 6; col++)
	{
		line = (col < 5) ? Font5x7[ch - ' '][col] : 0x00;

		for (row = 0; row < 8; row++)
		{
			px = x + col * scale;
			py = y + row * scale;

			if (line & (1 << row))
			{
				ST7735_FillRect(px, py, scale, scale, color);
			}
			else
			{
				ST7735_FillRect(px, py, scale, scale, bgColor);
			}
		}
	}
}

void ST7735_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, uint8_t scale)
{
	while (*str)
	{
		ST7735_DrawChar(x, y, *str, color, bgColor, scale);
		x += 6 * scale;
		str++;
	}
}

void ST7735_TestPattern(void)
{
	uint16_t i;
	const uint16_t colors[8] = {
		ST7735_RED, ST7735_GREEN, ST7735_BLUE, ST7735_WHITE,
		ST7735_YELLOW, ST7735_CYAN, ST7735_MAGENTA, ST7735_ORANGE
	};

	for (i = 0; i < 8; i++)
	{
		ST7735_FillRect(i * 16, 0, 16, 64, colors[i]);
	}

	ST7735_FillRect(0, 64, 128, 64, ST7735_BLACK);
	ST7735_DrawRect(4, 68, 120, 56, ST7735_WHITE);
	ST7735_DrawRect(10, 74, 108, 44, ST7735_GREEN);
	ST7735_FillRect(18, 82, 92, 28, ST7735_BLUE);
	ST7735_FillRect(26, 90, 76, 12, ST7735_YELLOW);

	for (i = 0; i < 128; i += 4)
	{
		ST7735_DrawPixel(i, i, ST7735_WHITE);
		ST7735_DrawPixel(127 - i, i, ST7735_RED);
	}
}
