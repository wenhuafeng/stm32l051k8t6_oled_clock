#include "display.h"
#include <stdint.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "fonts.h"
#include "trace.h"
#include "clock.h"

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
    struct TimeType *time = CLOCK_GetData();

    //CLOCK_Get(&time);
    SSD1306_GotoXY(2, 0);

    sprintf(data, "%02d:%02d", time->hour, time->minute);
    SSD1306_Puts(data, &Font_16x26, SSD1306_COLOR_WHITE);

    sprintf(data, ":%02d", time->second);
    SSD1306_Puts(data, &Font_11x18, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(4, 24);
    sprintf(data, "%02d-%02d-%02d", time->day, time->month, time->year);
    SSD1306_Puts(data, &Font_11x18, SSD1306_COLOR_WHITE);

    //SSD1306_GotoXY(4, 42);
    //sprintf(data, "%d.%01dC %d.%01d%%", (char)(tempHumi.temperature / 10),
    //        (char)(tempHumi.temperature % 10), (char)(tempHumi.humidity / 10),
    //        (char)(tempHumi.humidity % 10));
    //SSD1306_Puts(data, &Font_11x18, SSD1306_COLOR_WHITE);

    SSD1306_UpdateScreen();
}
