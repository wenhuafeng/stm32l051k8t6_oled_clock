#include "main.h"
#if defined(SUPPORT_SI7021) && SUPPORT_SI7021
#include "si7021.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "trace.h"
#include "si7021_iic.h"

#define LOG_TAG "si7021"

static struct Si7021Type g_si7021;

void SI7021_Init(void)
{
    SI7021_I2C_Init();
    LOGI(LOG_TAG, "si7021 i2c init\r\n");
}

void SI7021_SampleTempHumi(void)
{
    bool f_temp;
    static uint8_t count = 0;

    count++;
    if (count > 4) {
        count  = 0x00;
        f_temp = SI7021_Measure(TEMP_HOLD_MASTER, &g_si7021);
        if (f_temp == false) {
            return;
        }

        LL_mDelay(1);
        f_temp = SI7021_Measure(HUMI_HOLD_MASTER, &g_si7021);
        if (f_temp == false) {
            return;
        }

        LOGI(LOG_TAG, "%d.%01dC %d.%01d%%\r\n", (char)(g_si7021.temp / 10), (char)(g_si7021.temp % 10),
             (char)(g_si7021.humi / 10), (char)(g_si7021.humi % 10));
    }
}

int16_t SI7021_GetTemp(void)
{
    return g_si7021.temp;
}

uint16_t SI7021_GetHumi(void)
{
    return g_si7021.humi;
}

#else

void SI7021_Init(void) {}
void SI7021_SampleTempHumi(void) {}
int16_t SI7021_GetTemp(void)
{
    return 0x00;
}

uint16_t SI7021_GetHumi(void)
{
    return 0x00;
}

#endif
