#include "main.h"
#if defined(SI7021_I2C_SOFTWARE) && SI7021_I2C_SOFTWARE
#include "si7021_iic.h"
#include <stdint.h>
#include "common.h"
#include "si7021.h"
#include "trace.h"

#define LOG_TAG "si7021_iic_sw"

#define I2C_DELAY_TIME 250

#define SCL_Pin       LL_GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin       LL_GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

enum SdaIoType {
    SDA_OUTPUT,
    SDA_INPUT,
};

#define HSB 0
#define LSB 1

#define SI7021_SDA_IN()            \
    do {                           \
        HTU21D_SDA_SET(SDA_INPUT); \
    } while (0)
#define SI7021_SDA_OUT()            \
    do {                            \
        HTU21D_SDA_SET(SDA_OUTPUT); \
    } while (0)

#define SI7021_SCL_HIGH()                             \
    do {                                              \
        LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin); \
    } while (0)
#define SI7021_SCL_LOW()                                \
    do {                                                \
        LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin); \
    } while (0)

#define SI7021_SDA_HIGH()                             \
    do {                                              \
        LL_GPIO_SetOutputPin(SDA_GPIO_Port, SDA_Pin); \
    } while (0)
#define SI7021_SDA_LOW()                                \
    do {                                                \
        LL_GPIO_ResetOutputPin(SDA_GPIO_Port, SDA_Pin); \
    } while (0)

static void HTU21D_SDA_SET(enum SdaIoType io)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    if (io == SDA_OUTPUT) {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    } else {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    }

    GPIO_InitStruct.Pin        = SDA_Pin;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
}

static void I2C_DelayUs(uint8_t delay)
{
    while (delay-- != 0) {
        NOP();
    }
}

static void I2C_Start(void)
{
    SI7021_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SDA_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
}

static void I2C_Stop(void)
{
    SI7021_SDA_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
}

static void I2C_Ack(void)
{
    SI7021_SDA_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
}

static void I2C_Nack(void)
{
    SI7021_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_LOW();
}

static bool I2C_SendByte(uint8_t send)
{
    uint8_t i;
    bool ret = false;
    uint8_t delay;

    for (i = 0; i < 8; i++) {
        SI7021_SCL_LOW();
        if ((send << i) & 0x80) {
            SI7021_SDA_HIGH();
        } else {
            SI7021_SDA_LOW();
        }
        I2C_DelayUs(I2C_DELAY_TIME);
        SI7021_SCL_HIGH();
        I2C_DelayUs(I2C_DELAY_TIME);
    }

    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_LOW();
    SI7021_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);

    I2C_DelayUs(I2C_DELAY_TIME);
    SI7021_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY_TIME);

    SI7021_SDA_IN();
    delay = 200;
    while (delay != 0x00) {
        I2C_DelayUs(I2C_DELAY_TIME);
        if (LL_GPIO_IsInputPinSet(SDA_GPIO_Port, SDA_Pin) == 0) {
            ret = true;
            break;
        }
        delay--;
    }
    SI7021_SDA_OUT();
    if (delay == 0) {
        ret = false;
    }
    SI7021_SCL_LOW();
    I2C_DelayUs(I2C_DELAY_TIME);

    return ret;
}

static uint8_t I2C_ReadByte(void)
{
    uint8_t value = 0;
    uint8_t i;

    SI7021_SDA_IN();
    for (i = 0; i < 8; i++) {
        SI7021_SCL_HIGH();
        I2C_DelayUs(I2C_DELAY_TIME);
        value <<= 1;
        if (LL_GPIO_IsInputPinSet(SDA_GPIO_Port, SDA_Pin) == 1) {
            value += 1;
        }
        SI7021_SCL_LOW();
    }
    SI7021_SDA_OUT();

    return value;
}

static uint8_t CalcCrc8(uint8_t crc, uint8_t *buf, uint8_t size)
{
    while (size--) {
        crc ^= *buf++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else
                crc <<= 1;
        }
    }

    return crc;
}

bool SI7021_Measure(uint8_t type, struct Si7021Type *th)
{
    uint8_t read[2];
    uint8_t crc8;
    uint16_t temp;
    float buffer;

    I2C_Start();
    if (I2C_SendByte(SALVE_ADDR) == false) { // slave addr
        goto ERROR;
    }
    if (I2C_SendByte(type) == false) { // measure cmd
        goto ERROR;
    }

    LL_mDelay(100);
    I2C_Start();
    if (I2C_SendByte(SALVE_ADDR + 1) == false) {
        goto ERROR;
    }

    read[HSB] = I2C_ReadByte();
    I2C_Ack();
    read[LSB] = I2C_ReadByte();
    I2C_Ack();
    crc8 = I2C_ReadByte();
    I2C_Nack();
    I2C_Stop();

    if (CalcCrc8(0x00, read, sizeof(read)) != crc8) {
        goto ERROR;
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

ERROR:
    LOGE(LOG_TAG, "si7021 measure");
    return false;
}

#endif
