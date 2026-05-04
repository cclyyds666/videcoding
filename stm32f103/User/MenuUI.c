#include "MenuUI.h"
#include "ST7735.h"
#include <stdio.h>

#define UI_BG             0x0841
#define UI_PANEL          0x18E3
#define UI_PANEL_DARK     0x1082
#define UI_ACCENT         ST7735_CYAN
#define UI_WARN           ST7735_ORANGE

static const char *WeekText[5] = {"MON", "TUE", "WED", "THU", "FRI"};
static const char *MenuItems[APP_MENU_COUNT] = {"Focus", "Schedule", "Settings", "Time"};

static AppState UI_LastState;
static uint8_t UI_HasLastState = 0;

static void UI_PrintTime(uint16_t x, uint16_t y, const AppTime *time, uint8_t scale, uint16_t color, uint16_t bg)
{
	char text[12];

	sprintf(text, "%02d:%02d", time->hour, time->minute);
	ST7735_DrawString(x, y, text, color, bg, scale);
}

static void UI_Header(const AppState *state, const char *title)
{
	char text[16];

	ST7735_FillRect(0, 0, 128, 18, ST7735_BLUE);
	ST7735_DrawString(4, 5, title, ST7735_WHITE, ST7735_BLUE, 1);
	sprintf(text, "%s %02d:%02d", WeekText[state->now.weekDay], state->now.hour, state->now.minute);
	ST7735_DrawString(64, 5, text, ST7735_WHITE, ST7735_BLUE, 1);
}

static void UI_Bottom(const char *hint)
{
	ST7735_FillRect(0, 114, 128, 14, UI_PANEL_DARK);
	ST7735_DrawString(4, 117, hint, ST7735_CYAN, UI_PANEL_DARK, 1);
}

static void UI_Progress(uint16_t x, uint16_t y, uint16_t w, uint8_t percent, uint16_t color)
{
	uint16_t fill;

	if (percent > 100)
	{
		percent = 100;
	}

	fill = (w - 4) * percent / 100;
	ST7735_DrawRect(x, y, w, 10, ST7735_WHITE);
	ST7735_FillRect(x + 2, y + 2, w - 4, 6, UI_PANEL_DARK);
	ST7735_FillRect(x + 2, y + 2, fill, 6, color);
}

static void UI_DrawHome(const AppState *state)
{
	char text[18];

	ST7735_FillScreen(UI_BG);
	UI_Header(state, "POMO");

	UI_PrintTime(18, 30, &state->now, 3, ST7735_WHITE, UI_BG);
	ST7735_DrawString(30, 58, "Today focus  1/4", ST7735_CYAN, UI_BG, 1);

	ST7735_FillRect(8, 74, 112, 28, UI_PANEL);
	ST7735_DrawString(14, 80, "Next:", ST7735_WHITE, UI_PANEL, 1);
	if (state->scheduleAlert)
	{
		const AppCourse *course = &state->todayCourses[state->scheduleAlert - 1];
		sprintf(text, "%s %02d:%02d", course->name, course->hour, course->minute);
		ST7735_DrawString(50, 80, text, UI_WARN, UI_PANEL, 1);
		ST7735_DrawString(14, 92, "Class in 1 hour", UI_WARN, UI_PANEL, 1);
	}
	else
	{
		ST7735_DrawString(50, 80, "No alert", ST7735_GREEN, UI_PANEL, 1);
		ST7735_DrawString(14, 92, "Schedule is calm", ST7735_WHITE, UI_PANEL, 1);
	}

	UI_Bottom("OK: menu");
}

static void UI_DrawMenu(const AppState *state)
{
	uint8_t i;
	uint16_t y;
	uint16_t bg;
	uint16_t fg;

	ST7735_FillScreen(UI_BG);
	UI_Header(state, "MENU");

	for (i = 0; i < APP_MENU_COUNT; i++)
	{
		y = 25 + i * 21;
		if (i == state->menuIndex)
		{
			bg = UI_WARN;
			fg = ST7735_BLACK;
		}
		else
		{
			bg = UI_PANEL;
			fg = ST7735_WHITE;
		}

		ST7735_FillRect(8, y, 112, 16, bg);
		ST7735_DrawString(14, y + 4, MenuItems[i], fg, bg, 1);
	}

	UI_Bottom("UP/DN OK BACK ready");
}

static void UI_DrawMenuItem(uint8_t index, uint8_t selected)
{
	uint16_t y;
	uint16_t bg;
	uint16_t fg;

	if (index >= APP_MENU_COUNT)
	{
		return;
	}

	y = 25 + index * 21;
	if (selected)
	{
		bg = UI_WARN;
		fg = ST7735_BLACK;
	}
	else
	{
		bg = UI_PANEL;
		fg = ST7735_WHITE;
	}

	ST7735_FillRect(8, y, 112, 16, bg);
	ST7735_DrawString(14, y + 4, MenuItems[index], fg, bg, 1);
}

static void UI_DrawFocusSetup(const AppState *state)
{
	char text[16];

	ST7735_FillScreen(UI_BG);
	UI_Header(state, "FOCUS SET");

	ST7735_DrawString(12, 30, "Focus duration", ST7735_WHITE, UI_BG, 1);
	sprintf(text, "%02d min", state->focusMinutes);
	ST7735_DrawString(28, 50, text, UI_WARN, UI_BG, 2);
	ST7735_DrawString(18, 78, "OK: start", ST7735_GREEN, UI_BG, 1);
	ST7735_DrawString(18, 92, "BACK: cancel", ST7735_WHITE, UI_BG, 1);

	UI_Bottom("UP/DN set  OK start");
}

static void UI_DrawFocusTime(const AppState *state)
{
	char text[16];
	uint16_t remainMinutes;
	uint16_t remainSeconds;

	remainMinutes = state->focusRemainSeconds / 60;
	remainSeconds = state->focusRemainSeconds % 60;
	sprintf(text, "%02d:%02d", remainMinutes, remainSeconds);
	ST7735_FillRect(20, 36, 90, 24, UI_BG);
	ST7735_DrawString(20, 36, text, ST7735_WHITE, UI_BG, 3);
}

static void UI_DrawFocusProgress(const AppState *state)
{
	uint8_t percent;
	uint16_t totalSeconds;

	totalSeconds = state->focusMinutes * 60;
	if (totalSeconds == 0)
	{
		percent = 0;
	}
	else
	{
		percent = 100 - (uint8_t)(state->focusRemainSeconds * 100 / totalSeconds);
	}
	UI_Progress(14, 76, 100, percent, ST7735_GREEN);
}

static void UI_DrawFocusStatus(const AppState *state)
{
	ST7735_FillRect(26, 92, 66, 8, UI_BG);

	if (state->focusRunning)
	{
		ST7735_DrawString(26, 92, "Running", ST7735_GREEN, UI_BG, 1);
	}
	else
	{
		ST7735_DrawString(26, 92, "Finished", UI_WARN, UI_BG, 1);
	}

	UI_Bottom("BACK cancels focus");
}

static void UI_DrawFocusRunning(const AppState *state)
{
	ST7735_FillScreen(UI_BG);
	UI_Header(state, "FOCUS");
	UI_DrawFocusTime(state);
	UI_DrawFocusProgress(state);
	UI_DrawFocusStatus(state);
}

static void UI_DrawSchedule(const AppState *state)
{
	uint8_t i;
	uint16_t y;
	char text[20];

	ST7735_FillScreen(UI_BG);
	UI_Header(state, "SCHEDULE");

	for (i = 0; i < APP_COURSE_COUNT; i++)
	{
		const AppCourse *course = &state->todayCourses[i];
		y = 24 + i * 20;
		ST7735_FillRect(6, y, 116, 16, (state->scheduleAlert == i + 1) ? UI_WARN : UI_PANEL);
		sprintf(text, "%02d:%02d  %s", course->hour, course->minute, course->name);
		ST7735_DrawString(10, y + 4, text, (state->scheduleAlert == i + 1) ? ST7735_BLACK : ST7735_WHITE,
		                  (state->scheduleAlert == i + 1) ? UI_WARN : UI_PANEL, 1);
	}

	UI_Bottom("Mon-Fri stored");
}

static void UI_DrawSettings(const AppState *state)
{
	char text[18];

	ST7735_FillScreen(UI_BG);
	UI_Header(state, "SETTING");

	ST7735_DrawString(12, 32, "Backlight", ST7735_WHITE, UI_BG, 1);
	sprintf(text, "%d%%", state->backlightLevel);
	ST7735_DrawString(84, 32, text, UI_WARN, UI_BG, 1);
	UI_Progress(14, 50, 100, state->backlightLevel, UI_WARN);

	ST7735_DrawString(12, 72, "Time sync", ST7735_WHITE, UI_BG, 1);
	ST7735_DrawString(78, 72, "Manual", ST7735_CYAN, UI_BG, 1);
	ST7735_DrawString(12, 88, "Keys: PB12-15", ST7735_GREEN, UI_BG, 1);

	UI_Bottom("UP/DN brightness");
}

static void UI_UpdateSettingsBacklight(const AppState *state)
{
	char text[18];

	ST7735_FillRect(84, 32, 34, 8, UI_BG);
	sprintf(text, "%d%%", state->backlightLevel);
	ST7735_DrawString(84, 32, text, UI_WARN, UI_BG, 1);
	UI_Progress(14, 50, 100, state->backlightLevel, UI_WARN);
}

static void UI_DrawTimeSet(const AppState *state)
{
	ST7735_FillScreen(UI_BG);
	UI_Header(state, "TIME SET");

	ST7735_DrawString(18, 32, "Manual clock", ST7735_WHITE, UI_BG, 1);
	UI_PrintTime(20, 52, &state->now, 3, UI_WARN, UI_BG);
	ST7735_DrawString(18, 86, "UP/DN adjust", ST7735_CYAN, UI_BG, 1);
	ST7735_DrawString(18, 100, "OK next field", ST7735_WHITE, UI_BG, 1);
}

void MenuUI_Init(void)
{
	ST7735_Init();
	ST7735_SetRotation(1);
}

void MenuUI_Draw(const AppState *state)
{
	switch (state->page)
	{
		case APP_PAGE_HOME:
			UI_DrawHome(state);
			break;
		case APP_PAGE_MENU:
			UI_DrawMenu(state);
			break;
		case APP_PAGE_FOCUS_SETUP:
			UI_DrawFocusSetup(state);
			break;
		case APP_PAGE_FOCUS_RUNNING:
			UI_DrawFocusRunning(state);
			break;
		case APP_PAGE_SCHEDULE:
			UI_DrawSchedule(state);
			break;
		case APP_PAGE_SETTINGS:
			UI_DrawSettings(state);
			break;
		default:
			UI_DrawTimeSet(state);
			break;
	}

	UI_LastState = *state;
	UI_HasLastState = 1;
}

void MenuUI_Update(const AppState *state)
{
	if (!UI_HasLastState || state->page != UI_LastState.page)
	{
		MenuUI_Draw(state);
		return;
	}

	switch (state->page)
	{
		case APP_PAGE_MENU:
			if (state->menuIndex != UI_LastState.menuIndex)
			{
				UI_DrawMenuItem(UI_LastState.menuIndex, 0);
				UI_DrawMenuItem(state->menuIndex, 1);
			}
			break;

		case APP_PAGE_FOCUS_RUNNING:
			if (state->focusRemainSeconds != UI_LastState.focusRemainSeconds)
			{
				UI_DrawFocusTime(state);
				UI_DrawFocusProgress(state);
			}
			if (state->focusRunning != UI_LastState.focusRunning)
			{
				UI_DrawFocusStatus(state);
			}
			break;

		case APP_PAGE_SETTINGS:
			if (state->backlightLevel != UI_LastState.backlightLevel)
			{
				UI_UpdateSettingsBacklight(state);
			}
			break;

		case APP_PAGE_HOME:
		case APP_PAGE_TIME_SET:
			if (state->now.minute != UI_LastState.now.minute ||
			    state->now.hour != UI_LastState.now.hour ||
			    state->scheduleAlert != UI_LastState.scheduleAlert)
			{
				MenuUI_Draw(state);
				return;
			}
			break;

		default:
			MenuUI_Draw(state);
			return;
	}

	UI_LastState = *state;
}
