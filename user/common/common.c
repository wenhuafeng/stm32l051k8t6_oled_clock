#include "common.h"
#include <stdint.h>
#include "lptim.h"
#include "main.h"
#include "clock.h"
#include "display.h"
#include "trace.h"
#include "si7021.h"
#include "wifi_uart_if.h"
#include "esp8266_at.h"

#define LOG_TAG          "common"
#define SOFTWARE_VERSION "V101"

#define TIME_250MS 7
#define TIME_500MS 15
#define TIME_1S    31
#define TIME_2S    63

static bool f_32hz;
static bool f_32hz_1;
static bool f_250ms;
static bool f_500ms;
static bool f_1s;
static bool f_2s;

//#define LPTIM1_INT_TIME (32768 - 1) // 32768 / 32768 = 1S
//#define LPTIM1_INT_TIME (16384 - 1) // 16384 / 32768 = 0.5S
//#define LPTIM1_INT_TIME (8192 - 1)  // 8192 / 32768 = 0.25S
#define LPTIM1_INT_TIME (1024 - 1) // 1024 / 32768 = 0.03125S

static void LPTIM1_CounterStartIT(void)
{
    LL_LPTIM_EnableIT_ARRM(LPTIM1);
    LL_LPTIM_Enable(LPTIM1);
    LL_LPTIM_SetAutoReload(LPTIM1, LPTIM1_INT_TIME);
    LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

void LPTIM1_IsrHandle(void)
{
    static uint8_t ctr0 = 0x00;
    static uint8_t ctr1 = 0x00;
    static uint8_t ctr2 = 0x00;
    static uint8_t ctr3 = 0x00;

    if (LL_LPTIM_IsActiveFlag_ARRM(LPTIM1) == false) {
        return;
    }
    LL_LPTIM_ClearFLAG_ARRM(LPTIM1);

    f_32hz   = true;
    f_32hz_1 = true;

    ctr0++;
    if (ctr0 > TIME_250MS) {
        ctr0    = 0x00;
        f_250ms = true;
    }
    ctr1++;
    if (ctr1 > TIME_500MS) {
        ctr1    = 0x00;
        f_500ms = true;
    }
    ctr2++;
    if (ctr2 > TIME_1S) {
        ctr2 = 0x00;
        f_1s = true;
    }
    ctr3++;
    if (ctr3 > TIME_2S) {
        ctr3 = 0x00;
        f_2s = true;
    }
}

bool Get32HzFlag(void)
{
    return f_32hz;
}

void Set32HzFlag(bool flag)
{
    f_32hz = flag;
}

bool Get32HzTwoFlag(void)
{
    return f_32hz_1;
}

void Set32HzTwoFlag(bool flag)
{
    f_32hz_1 = flag;
}

void Set250msFlag(bool flag)
{
    f_250ms = flag;
}

bool Get250msFlag(void)
{
    return f_250ms;
}

void Set500msFlag(bool flag)
{
    f_500ms = flag;
}

bool Get500msFlag(void)
{
    return f_500ms;
}

void Set1sFlag(bool flag)
{
    f_1s = flag;
}

bool Get1sFlag(void)
{
    return f_1s;
}

void Set2sFlag(bool flag)
{
    f_2s = flag;
}

bool Get2sFlag(void)
{
    return f_2s;
}

static void EnterStop(void)
{
    /* 开启PWR时钟 */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    /* 设置中断线 */
    //LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);
    //LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);
    //LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);

    /* 设置RTC唤醒时间 */
    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_WAKEUP_Disable(RTC);
    while (!LL_RTC_IsActiveFlag_WUTW(RTC));
    LL_RTC_WAKEUP_SetAutoReload(RTC, 1);
    LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
    LL_RTC_EnableIT_WUT(RTC);
    LL_RTC_WAKEUP_Enable(RTC);
    LL_RTC_EnableWriteProtection(RTC);
    //power_off();
    LL_PWR_EnableUltraLowPower();
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    LL_LPM_EnableDeepSleep();
    __WFI();
    //power_on();
}

void COMMON_Init(void)
{
    LPTIM1_CounterStartIT();
    DISP_Init();
    SI7021_SampleTempHumi();
    WIFI_UART_ReceiveDmaInit();
    WIFI_Power(GetWifiData(), WIFI_POWER_ON);
    LOGI(LOG_TAG, "%s, %s, %s\r\n", SOFTWARE_VERSION, __TIME__, __DATE__);
}

void COMMON_Function(void)
{
    if (f_1s == false) {
        return;
    }
    f_1s = false;

    LED_BLINK();
    CLOCK_Run();
    SI7021_SampleTempHumi();
    WIFI_SendCommand(GetWifiData());
    DISP_Clock();
}
