#ifndef SI7021_IIC_H
#define SI7021_IIC_H

#include <stdint.h>
#include <stdbool.h>
#include "si7021.h"

#define SI7021_ADDR 0x80

#define HUMI_HOLD_MASTER 0xE5
#define TEMP_HOLD_MASTER 0xE3

#define HUMI_NOHOLD_MASTER 0xF5
#define TEMP_NOHOLD_MASTER 0xF3

extern void SI7021_I2C_Init(void);
extern bool SI7021_Measure(uint8_t type, struct Si7021Type *th);

#endif
