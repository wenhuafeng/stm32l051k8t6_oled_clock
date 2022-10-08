#include "main.h"
#if defined(SSD1306_I2C_HARDWARE) && SSD1306_I2C_HARDWARE
#include "ssd1306_iic.h"
#include <stdint.h>
#include "common.h"
#include "i2c.h"
#include "trace.h"

#define LOG_TAG "ssd2306_iic_hw"

//  _____ ___   _____
// |_   _|__ \ / ____|
//   | |    ) | |
//   | |   / /| |
//  _| |_ / /_| |____
// |_____|____|\_____|

#define LIB_I2C_TIMEOUT 1000
#define LIB_I2C_POLLING(condition)    \
    do {                              \
        uint16_t n = LIB_I2C_TIMEOUT; \
        while (condition) {           \
            if (--n == 0) {           \
                goto I2C_ERROR;       \
            }                         \
        }                             \
    } while (0)

void SSD1306_I2C_Init(void)
{
    MX_I2C1_Init();
    LL_I2C_Enable(I2C1);
}

static void SSD1306_I2C_Write(uint8_t reg, uint8_t *buf, uint16_t count)
{
    uint16_t i;

    LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));

    LL_I2C_DisableAutoEndMode(I2C1);
    LL_I2C_SetSlaveAddr(I2C1, SSD1306_I2C_ADDR);
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
    LL_I2C_SetTransferSize(I2C1, count + 1);

    LL_I2C_GenerateStartCondition(I2C1);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    LL_I2C_TransmitData8(I2C1, reg);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    for (i = 0; i < count; i++) {
        LL_I2C_TransmitData8(I2C1, buf[i]);
        LIB_I2C_POLLING(!LL_I2C_IsActiveFlag_TXE(I2C1));
    }
    LL_I2C_GenerateStopCondition(I2C1);
    LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));
    return;

I2C_ERROR:
    LL_I2C_GenerateStopCondition(I2C1);
    LOGE(LOG_TAG, "ssd1306 i2c write!\r\n");
}

void SSD1306_WriteCommand(uint8_t cmd)
{
    SSD1306_I2C_Write(0x00, &cmd, 1);
}

void SSD1306_I2C_WriteMulti(uint8_t reg, uint8_t *data, uint16_t count)
{
    SSD1306_I2C_Write(0x40, data, count);
}

#endif
