#include "main.h"
#if defined(SSD1306_I2C_SOFTWARE) && SSD1306_I2C_SOFTWARE
#include "ssd1306_iic.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "trace.h"

#define TAG_LOG "iic_sw"

#define I2C_DELAY 100

#define SCL_Pin       LL_GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin       LL_GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

enum SdaIoType {
    SDA_OUTPUT,
    SDA_INPUT,
};

#define I2C_SDA_IN()               \
    do {                           \
        HTU21D_SDA_SET(SDA_INPUT); \
    } while (0)
#define I2C_SDA_OUT()               \
    do {                            \
        HTU21D_SDA_SET(SDA_OUTPUT); \
    } while (0)
#define I2C_SCL_HIGH()                                \
    do {                                              \
        LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin); \
    } while (0)
#define I2C_SCL_LOW()                                   \
    do {                                                \
        LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin); \
    } while (0)
#define I2C_SDA_HIGH()                                \
    do {                                              \
        LL_GPIO_SetOutputPin(SDA_GPIO_Port, SDA_Pin); \
    } while (0)
#define I2C_SDA_LOW()                                   \
    do {                                                \
        LL_GPIO_ResetOutputPin(SDA_GPIO_Port, SDA_Pin); \
    } while (0)

static void I2C_DelayUs(uint8_t delay)
{
    while(delay--);
}

static void HTU21D_SDA_SET(enum SdaIoType io)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (io == SDA_OUTPUT) {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    } else {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    }

    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
}

static void I2C_Start(void)
{
    I2C_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SDA_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_LOW();
    I2C_DelayUs(I2C_DELAY);
}

static void I2C_Stop(void)
{
    I2C_SDA_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY);
}

#if 0
static void I2C_Ack(void)
{
    I2C_SDA_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY);
}

static void I2C_Nack(void)
{
    I2C_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY);
    I2C_SCL_LOW();
}
#endif

static bool I2C_WriteByte(uint8_t send)
{
    uint8_t temp;
    uint8_t i;
    bool ret = false;

    for (i = 0; i < 8; i++) {
        I2C_SCL_LOW();
        if ((send << i) & 0x80) {
            I2C_SDA_HIGH();
        } else {
            I2C_SDA_LOW();
        }
        I2C_DelayUs(I2C_DELAY);
        I2C_SCL_HIGH();
        I2C_DelayUs(I2C_DELAY);
    }

    I2C_SCL_LOW();
    I2C_DelayUs(I2C_DELAY);
    I2C_SDA_HIGH();
    I2C_DelayUs(I2C_DELAY);

    //I2C_DelayUs(I2C_DELAY);
    I2C_SCL_HIGH();
    I2C_DelayUs(I2C_DELAY);

    // read ack
    I2C_SDA_IN();
    temp = 20;
    while (--temp) {
        I2C_DelayUs(I2C_DELAY);
        if (LL_GPIO_IsInputPinSet(SDA_GPIO_Port, SDA_Pin) == 0) {
            ret = true;
            break;
        }
    }
    I2C_SDA_OUT();
    I2C_SCL_LOW();
    I2C_DelayUs(I2C_DELAY);
    if (temp == 0) {
        ret = false;
    }

    return ret;
}

#if 0
static uint8_t I2C_ReadByte(void)
{
    uint8_t value = 0;
    uint8_t i;

    I2C_SDA_IN();
    for (i = 0; i < 8; i++) {
        I2C_SCL_HIGH();
        I2C_DelayUs(I2C_DELAY);
        value <<= 1;
        if (LL_GPIO_IsInputPinSet(SDA_GPIO_Port, SDA_Pin) == 1) {
            value += 1;
        }
        I2C_SCL_LOW();
    }
    I2C_SDA_OUT();

    return value;
}
#endif

void SSD1306_I2C_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SCL_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);

    I2C_Stop();
}

void SSD1306_WriteCommand(uint8_t cmd)
{
    bool err;

    I2C_Start();
    err = I2C_WriteByte(SSD1306_I2C_ADDR | WRITE);
    if (err != true) {
        NOP();
        goto I2C_ERROR;
    }
    err = I2C_WriteByte(0x00);
    if (err != true) {
        goto I2C_ERROR;
    }
    err = I2C_WriteByte(cmd);
    if (err != true) {
        goto I2C_ERROR;
    }
    I2C_Stop();
    return;

I2C_ERROR:
    I2C_Stop();
    NOP();
    LOGE(TAG_LOG, "ssd1306 write command error!\r\n");
}

void SSD1306_I2C_WriteMulti(uint8_t reg, uint8_t *data, uint16_t count)
{
    bool err;

    I2C_Start();
    err = I2C_WriteByte(SSD1306_I2C_ADDR | WRITE);
    if (err != true) {
        NOP();
        goto I2C_ERROR;
    }
    err = I2C_WriteByte(reg);
    if (err != true) {
        goto I2C_ERROR;
    }

    while (count-- != 0x00) {
        err = I2C_WriteByte(*data++);
        if (err != true) {
            goto I2C_ERROR;
        }
    }
    I2C_Stop();
    return;

I2C_ERROR:
    I2C_Stop();
    NOP();
    LOGE(TAG_LOG, "ssd1306 write multi error!\r\n");
}

#if 0
uint8_t SSD1306_I2C_Read(uint8_t reg)
{
    bool err;
    uint8_t read;

    I2C_Start();
    err = I2C_WriteByte(SSD1306_I2C_ADDR | READ);
    if (err != 0) {
        goto I2C_ERROR;
    }
    read = I2C_ReadByte();
    I2C_Nack();
    I2C_Stop();
    return read;

I2C_ERROR:
    asm("nop");
    return 0xff;
}

uint8_t* SSD1306_I2C_ReadMulti(uint8_t reg, uint8_t len)
{

}
#endif

#endif
