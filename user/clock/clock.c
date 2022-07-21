#include "clock.h"
#include <stdbool.h>
//#include "common.h"
#include "rtc.h"
#include "trace.h"

#define LOG_TAG "clock"

struct TimeType g_time = {
    .second = 0,
    .minute = 0,
    .hour = 0,
    .day = 1,
    .month = 1,
    .year = 2022
};

static uint8_t GetMaxDay(uint16_t year, uint8_t month)
{
    uint8_t day;
    uint8_t daysTable[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month == 2) {
        if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
            day = 29;
        } else {
            day = 28;
        }
    } else {
        day = daysTable[month];
    }

    return day;
}

void CLOCK_CalculateWeek(struct TimeType *time)
{
    int16_t yearTemp = 0;
    int16_t yearHigh;
    int16_t yearLow;
    int8_t monthTemp = 0;
    int8_t wk;

    if (time == NULL) {
        return;
    }

    if (time->month < 3) {
        monthTemp = time->month + 12;
        yearTemp  = time->year - 1;
    } else {
        monthTemp = time->month;
        yearTemp  = time->year;
    }

    yearHigh = yearTemp / 100;
    yearLow  = yearTemp % 100;

    wk = yearLow + (yearLow / 4) + (yearHigh / 4);
    wk = wk - (2 * yearHigh) + (26 * (monthTemp + 1) / 10) + time->day - 1;
    wk = (wk + 140) % 7;

    time->week = wk;
}

bool CLOCK_Run(void)
{
    struct TimeType *time = &g_time;

    time->second++;
    if (time->second < 60) {
        return false;
    }

    time->second = 0;
    time->minute++;
    if (time->minute < 60) {
        return false;
    }

    time->minute = 0;
    time->hour++;
    if (time->hour < 24) {
        return false;
    }

    time->hour = 0;
    time->day++;
    if (time->day <= GetMaxDay(time->year, time->month)) {
        goto calc_week;
    }

    time->day = 1;
    time->month++;
    if (time->month < 13) {
        goto calc_week;
    }

    time->month = 1;
    time->year++;

calc_week:
    CLOCK_CalculateWeek(time);

    return true;
}

void CLOCK_Get(struct TimeType *time)
{
    //LL_RTC_TimeTypeDef sTime;
    //LL_RTC_DateTypeDef sDate;

    if (time == NULL) {
        return;
    }

    //LL_RTC_TS_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    //LL_RTC_TS_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    time->second = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    time->minute = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    time->hour   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
    time->day    = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    time->month  = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    time->week   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetWeekDay(RTC));
    time->year   = (__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC)) + 2000);

    LOGI(LOG_TAG, "get time: %d-%d-%d %02d:%02d:%02d \r\n", time->year, time->month, time->day, time->hour,
         time->minute, time->second);
}

void CLOCK_Set(struct TimeType *time)
{
    LL_RTC_TimeTypeDef sTime;
    LL_RTC_DateTypeDef sDate;

    if (time == NULL) {
        return;
    }

    sTime.Seconds = time->second;
    sTime.Minutes = time->minute;
    sTime.Hours   = time->hour;

    sDate.Day    = time->day;
    sDate.Month   = time->month;
    sDate.WeekDay = time->week;
    sDate.Year    = (time->year % 100);

    //LL_RTC_TS_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    //LL_RTC_TS_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_EnableInitMode(RTC);
    while (LL_RTC_IsActiveFlag_INIT(RTC) != 1);
    LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_AM_OR_24, sTime.Hours, sTime.Minutes, sTime.Seconds);
    LL_RTC_DATE_Config(RTC, sDate.WeekDay, sDate.Day, sDate.Month, sDate.Year);
    //Exit_RTC_InitMode();
    LL_RTC_DisableInitMode(RTC);
    LL_RTC_ClearFlag_RS(RTC);
    while(LL_RTC_IsActiveFlag_RS(RTC) != 1);
    LL_RTC_EnableWriteProtection(RTC);

    LOGI(LOG_TAG, "set time: %d-%d-%d %02d:%02d:%02d \r\n", time->year, time->month, time->day, time->hour,
         time->minute, time->second);
}

struct TimeType *CLOCK_GetData(void)
{
    return &g_time;
}
