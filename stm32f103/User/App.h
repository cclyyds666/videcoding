#ifndef __APP_H
#define __APP_H

#include "stm32f10x.h"

#define APP_MENU_COUNT       4
#define APP_COURSE_COUNT     4

typedef enum
{
	APP_PAGE_HOME = 0,
	APP_PAGE_MENU,
	APP_PAGE_FOCUS_SETUP,
	APP_PAGE_FOCUS_RUNNING,
	APP_PAGE_SCHEDULE,
	APP_PAGE_SETTINGS,
	APP_PAGE_TIME_SET
} AppPage;

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t weekDay;
} AppTime;

typedef struct
{
	char name[12];
	uint8_t hour;
	uint8_t minute;
} AppCourse;

typedef struct
{
	AppTime now;
	AppPage page;
	uint8_t menuIndex;
	uint8_t focusMinutes;
	uint16_t focusRemainSeconds;
	uint8_t focusRunning;
	uint8_t backlightLevel;
	uint8_t scheduleAlert;
	uint8_t demoMode;
	const AppCourse *todayCourses;
} AppState;

void App_Init(void);
void App_Task(void);

#endif
