#include "main.h"
#if defined(SSD1306_I2C_HARDWARE) && SSD1306_I2C_HARDWARE
#include "ssd1306_iic.h"
#include <stdint.h>
#include "common.h"
#include "i2c.h"

//  _____ ___   _____
// |_   _|__ \ / ____|
//   | |    ) | |
//   | |   / /| |
//  _| |_ / /_| |____
// |_____|____|\_____|

#define LIB_I2C_TIMEOUT 100
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
    //LL_I2C_EnableAutoEndMode(I2C1);
}

#if 0
void ssd1306_I2C_Write(uint8_t reg, uint8_t data)
{
    while (LL_I2C_IsActiveFlag_BUSY(I2C1));

    //LL_I2C_Enable(I2C1);
    LL_I2C_SetSlaveAddr(I2C1, (SSD1306_I2C_ADDR << 1));
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
    LL_I2C_SetTransferSize(I2C1, 2);     // Transfer of <reg><data>
    LL_I2C_GenerateStartCondition(I2C1);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    //while(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    LL_I2C_TransmitData8(I2C1, reg);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    LL_I2C_TransmitData8(I2C1, data);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
    LL_I2C_GenerateStopCondition(I2C1);
    //LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));
    //LL_I2C_Disable(I2C1);
    //return true;

I2C_ERROR:
    __asm("nop");
    //return false;
}
#endif

static void SSD1306_I2C_Write(uint8_t addr, uint8_t *buf, uint16_t bytes_count)
{
    uint16_t i;

    LIB_I2C_POLLING(LL_I2C_IsActiveFlag_BUSY(I2C1));

    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2C1);
    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));

    LL_I2C_TransmitData8(I2C1, (SSD1306_I2C_ADDR | WRITE));
    LIB_I2C_POLLING(!LL_I2C_IsActiveFlag_ADDR(I2C1));

    LL_I2C_ClearFlag_ADDR(I2C1);

    LL_I2C_TransmitData8(I2C1, addr);
    LIB_I2C_POLLING(!LL_I2C_IsActiveFlag_TXE(I2C1));
    for (i = 0; i < bytes_count; i++) {
        LL_I2C_TransmitData8(I2C1, buf[i]);
        LIB_I2C_POLLING(!LL_I2C_IsActiveFlag_TXE(I2C1));
    }
    LL_I2C_GenerateStopCondition(I2C1);

I2C_ERROR:
    NOP();
}

void SSD1306_WriteCommand(uint8_t cmd)
{
    SSD1306_I2C_Write(0x00, &cmd, 1);
}

void SSD1306_WriteData(uint8_t data)
{
}

void SSD1306_I2C_WriteMulti(uint8_t reg, uint8_t *data, uint16_t count)
{
    SSD1306_I2C_Write(0x40, data, count);

//    uint8_t i;

//    while (LL_I2C_IsActiveFlag_BUSY(I2C1))
//        ;
//
//    //LL_I2C_Enable(I2C1);
//    LL_I2C_SetSlaveAddr(I2C1, (SSD1306_I2C_ADDR << 1));
//    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
//    LL_I2C_SetTransferSize(I2C1, count); // Transfer of data_n bytes + addr byte
//    LL_I2C_GenerateStartCondition(I2C1); // START
//    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
//    LL_I2C_TransmitData8(I2C1, reg); // Send address
//    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
//
//    for (i = 1; i < count; i++) {
//        LL_I2C_TransmitData8(I2C1, *data++); // Push data buffer to slave device
//        LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TXE(I2C1)));
//    }
//    LIB_I2C_POLLING(!(LL_I2C_IsActiveFlag_TC(I2C1)));
//    LL_I2C_GenerateStopCondition(I2C1);
//    //LL_I2C_Disable(I2C1);
//    //return true;
//
//I2C_ERROR:
//    __asm("nop");
//    //return false;
}

#endif
