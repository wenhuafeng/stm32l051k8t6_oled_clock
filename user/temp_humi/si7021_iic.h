#ifndef SI7021_IIC_H
#define SI7021_IIC_H

#include <stdint.h>
#include <stdbool.h>
#include "si7021.h"

extern bool SI7021_Measure(uint8_t type, struct Si7021Type *th);

#endif
