#include "main.h"
#if defined(SUPPORT_OLED_DISPLAY) && SUPPORT_OLED_DISPLAY
#include "display.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "fonts.h"
#include "trace.h"
#include "clock.h"
#include "si7021.h"

#define LOG_TAG "display"

void DISP_Init(void)
{
    if (SSD1306_Init() == false) {
        LOGE(LOG_TAG, "SSD1306 init error!\r\n");
    }
}

void DISP_Clock(void)
{
    char data[16];
    int16_t temp = SI7021_GetTemp();
    uint16_t humi = SI7021_GetHumi();
    struct TimeType *time = CLOCK_GetData();

    SSD1306_GotoXY(0, 0);
    sprintf(data, "%02d", time->hour);
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(32, 0);
    sprintf(data, ":");
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(46, 0);
    sprintf(data, "%02d", time->minute);
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(78, 0);
    sprintf(data, ":");
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(94, 0);
    sprintf(data, "%02d", time->second);
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    //SSD1306_GotoXY(0, 0);
    //sprintf(data, "%02d:%02d:%02d", time->hour, time->minute, time->second);
    //SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(8, 24);
    sprintf(data, "%02d-%02d-%02d", time->day, time->month, time->year);
    SSD1306_Puts(data, &Font_11x18, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(4, 42);
    sprintf(data, "%d.%01dC %d.%01d%%", (char)(temp / 10), (char)(temp % 10), (char)(humi / 10), (char)(humi % 10));
    SSD1306_Puts(data, &Font_11x18, SSD1306_COLOR_WHITE);

    SSD1306_UpdateScreen();
}

#else

void DISP_Init(void) {}
void DISP_Clock(void) {}

#endif
