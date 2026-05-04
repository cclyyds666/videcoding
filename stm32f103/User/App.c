#include "App.h"
#include "Delay.h"
#include "Key.h"
#include "MenuUI.h"
#include "ST7735.h"

static AppState App_State;

static const AppCourse App_Schedule[5][APP_COURSE_COUNT] = {
	{{"Math", 8, 0}, {"English", 10, 0}, {"Physics", 14, 0}, {"PE", 16, 0}},
	{{"Chinese", 8, 0}, {"Chem", 10, 0}, {"Math", 14, 0}, {"Club", 16, 0}},
	{{"English", 8, 0}, {"Biology", 10, 0}, {"History", 14, 0}, {"Lab", 16, 0}},
	{{"Physics", 8, 0}, {"Math", 10, 0}, {"Art", 14, 0}, {"Reading", 16, 0}},
	{{"Chem", 8, 0}, {"English", 10, 0}, {"Computer", 14, 0}, {"Review", 16, 0}}
};

static void App_UpdateScheduleAlert(void)
{
	uint8_t i;
	uint16_t nowMinute;
	uint16_t courseMinute;

	App_State.scheduleAlert = 0;

	if (App_State.now.weekDay >= 5)
	{
		return;
	}

	nowMinute = App_State.now.hour * 60 + App_State.now.minute;

	for (i = 0; i < APP_COURSE_COUNT; i++)
	{
		courseMinute = App_State.todayCourses[i].hour * 60 + App_State.todayCourses[i].minute;
		if (courseMinute >= nowMinute && (courseMinute - nowMinute) <= 60)
		{
			App_State.scheduleAlert = i + 1;
			return;
		}
	}
}

static void App_TickClock(void)
{
	App_State.now.second++;
	if (App_State.now.second >= 60)
	{
		App_State.now.second = 0;
		App_State.now.minute++;
	}

	if (App_State.now.minute >= 60)
	{
		App_State.now.minute -= 60;
		App_State.now.hour++;
	}

	if (App_State.now.hour >= 24)
	{
		App_State.now.hour = 0;
		App_State.now.weekDay++;
		if (App_State.now.weekDay >= 5)
		{
			App_State.now.weekDay = 0;
		}
		App_State.todayCourses = App_Schedule[App_State.now.weekDay];
	}
}

static void App_TickFocus(void)
{
	if (App_State.focusRunning && App_State.focusRemainSeconds > 0)
	{
		App_State.focusRemainSeconds--;
		if (App_State.focusRemainSeconds == 0)
		{
			App_State.focusRunning = 0;
		}
	}
}

static void App_EnterMenuItem(void)
{
	switch (App_State.menuIndex)
	{
		case 0:
			App_State.page = APP_PAGE_FOCUS_SETUP;
			break;
		case 1:
			App_State.page = APP_PAGE_SCHEDULE;
			break;
		case 2:
			App_State.page = APP_PAGE_SETTINGS;
			break;
		default:
			App_State.page = APP_PAGE_TIME_SET;
			break;
	}
}

static void App_HandleKey(KeyValue key)
{
	if (key == KEY_NONE)
	{
		return;
	}

	App_State.demoMode = 0;

	if (key == KEY_BACK)
	{
		if (App_State.page == APP_PAGE_HOME)
		{
			App_State.page = APP_PAGE_MENU;
		}
		else if (App_State.page == APP_PAGE_FOCUS_RUNNING)
		{
			App_State.focusRunning = 0;
			App_State.focusRemainSeconds = App_State.focusMinutes * 60;
			App_State.page = APP_PAGE_MENU;
		}
		else
		{
			App_State.page = APP_PAGE_MENU;
		}
		return;
	}

	switch (App_State.page)
	{
		case APP_PAGE_HOME:
			if (key == KEY_OK)
			{
				App_State.page = APP_PAGE_MENU;
			}
			break;

		case APP_PAGE_MENU:
			if (key == KEY_UP)
			{
				App_State.menuIndex = (App_State.menuIndex == 0) ? (APP_MENU_COUNT - 1) : (App_State.menuIndex - 1);
			}
			else if (key == KEY_DOWN)
			{
				App_State.menuIndex++;
				if (App_State.menuIndex >= APP_MENU_COUNT)
				{
					App_State.menuIndex = 0;
				}
			}
			else if (key == KEY_OK)
			{
				App_EnterMenuItem();
			}
			break;

		case APP_PAGE_FOCUS_SETUP:
			if (key == KEY_UP && App_State.focusMinutes < 60)
			{
				App_State.focusMinutes += 5;
			}
			else if (key == KEY_DOWN && App_State.focusMinutes > 5)
			{
				App_State.focusMinutes -= 5;
			}
			else if (key == KEY_OK)
			{
				App_State.focusRemainSeconds = App_State.focusMinutes * 60;
				App_State.focusRunning = 1;
				App_State.page = APP_PAGE_FOCUS_RUNNING;
			}
			break;

		case APP_PAGE_SETTINGS:
			if (key == KEY_UP && App_State.backlightLevel < 100)
			{
				App_State.backlightLevel += 25;
				ST7735_SetBacklightLevel(App_State.backlightLevel);
			}
			else if (key == KEY_DOWN && App_State.backlightLevel > 25)
			{
				App_State.backlightLevel -= 25;
				ST7735_SetBacklightLevel(App_State.backlightLevel);
			}
			break;

		case APP_PAGE_TIME_SET:
			if (key == KEY_UP)
			{
				App_State.now.minute = (App_State.now.minute + 1) % 60;
			}
			else if (key == KEY_DOWN)
			{
				App_State.now.minute = (App_State.now.minute == 0) ? 59 : (App_State.now.minute - 1);
			}
			break;

		default:
			if (key == KEY_OK)
			{
				App_State.page = APP_PAGE_MENU;
			}
			break;
	}
}

void App_Init(void)
{
	App_State.now.hour = 7;
	App_State.now.minute = 10;
	App_State.now.second = 0;
	App_State.now.weekDay = 0;
	App_State.page = APP_PAGE_HOME;
	App_State.menuIndex = 0;
	App_State.focusMinutes = 25;
	App_State.focusRemainSeconds = App_State.focusMinutes * 60;
	App_State.focusRunning = 0;
	App_State.backlightLevel = 75;
	App_State.scheduleAlert = 0;
	App_State.demoMode = 0;
	App_State.todayCourses = App_Schedule[App_State.now.weekDay];

	Key_Init();
	MenuUI_Init();
	ST7735_SetBacklightLevel(App_State.backlightLevel);
	MenuUI_Update(&App_State);
}

void App_Task(void)
{
	static uint16_t elapsedMs = 0;
	KeyValue key;
	uint8_t needRedraw = 0;

	Delay_ms(50);
	elapsedMs += 50;

	key = Key_GetValue();
	if (key != KEY_NONE)
	{
		needRedraw = 1;
	}
	App_HandleKey(key);

	if (elapsedMs >= 1000)
	{
		elapsedMs = 0;
		App_TickClock();
		App_TickFocus();
		App_UpdateScheduleAlert();

		if (App_State.page == APP_PAGE_FOCUS_RUNNING)
		{
			needRedraw = 1;
		}
		else if (App_State.now.second == 0)
		{
			needRedraw = 1;
		}
	}

	if (needRedraw)
	{
		MenuUI_Update(&App_State);
	}
}
