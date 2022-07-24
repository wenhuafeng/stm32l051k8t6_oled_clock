#include "main.h"
#if defined(SI7021_I2C_HARDWARE) && SI7021_I2C_HARDWARE
#include "si7021_iic.h"
#include <stdint.h>
#include "i2c.h"
#include "si7021.h"
#include "trace.h"

#define LOG_TAG "si7021_iic_hw"

#define HSB 0
#define LSB 1

#define LIB_I2C_TIMEOUT 1000
#define LIB_I2C_POLLING(condition)                                 \
    do {                                                           \
        uint16_t n = LIB_I2C_TIMEOUT;                              \
        while (condition) {                                        \
            if (--n == 0) {                                        \
                LOGE(LOG_TAG, "si7021 measure: %d\r\n", __LINE__); \
                goto I2C_ERROR;                                    \
            }                                                      \
        }                                                          \
    } while (0)

static uint8_t CalcCrc8(uint8_t crc, uint8_t *buf, uint8_t size)
{
    while (size--) {
        crc ^= *buf++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

void SI7021_I2C_Init(void)
{
    MX_I2C1_Init();
    LL_I2C_Enable(I2C1);
}

bool SI7021_Measure(uint8_t type, struct Si7021Type *th)
{
    uint8_t i;
    uint16_t temp;
    float buffer;
    uint8_t read[3];

    LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));

    LL_I2C_DisableAutoEndMode(I2C1);
    LL_I2C_SetSlaveAddr(I2C1, SI7021_ADDR);
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
    LL_I2C_SetTransferSize(I2C1, 1);
    LL_I2C_GenerateStartCondition(I2C1);

    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    LL_I2C_TransmitData8(I2C1, type);
    LIB_I2C_POLLING(!LL_I2C_IsActiveFlag_TC(I2C1));

    LL_mDelay(100);

    LL_I2C_DisableAutoEndMode(I2C1);
    LL_I2C_SetSlaveAddr(I2C1, SI7021_ADDR);
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_READ);
    LL_I2C_SetTransferSize(I2C1, sizeof(read));

    LL_I2C_GenerateStartCondition(I2C1);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    for (i = 0; i < sizeof(read); i++) {
        LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_RXNE(I2C1)));
        read[i] = LL_I2C_ReceiveData8(I2C1);
    }
    LL_I2C_GenerateStopCondition(I2C1);
    LL_I2C_ClearFlag_STOP(I2C1);

    LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));

    if (CalcCrc8(0x00, read, (sizeof(read) - 1)) != read[2]) {
        LOGE(LOG_TAG, "si7021 crc8 error\r\n");
        goto I2C_ERROR;
    }
    temp = (uint16_t)(read[HSB] << 8) | read[LSB];
    if (type != TEMP_HOLD_MASTER) {
        buffer   = temp * 125.0;
        buffer   = buffer / 65536 - 6;
        th->humi = buffer * 10;
    } else {
        buffer   = temp * 175.72;
        buffer   = buffer / 65536 - 46.85;
        th->temp = buffer * 10;
    }
    return true;

I2C_ERROR:
    LL_I2C_GenerateStopCondition(I2C1);
    return false;
}

#endif
