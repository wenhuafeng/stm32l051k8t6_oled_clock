#ifndef SI7021_H
#define SI7021_H

#include <stdint.h>

struct Si7021Type {
    int16_t temp;
    uint16_t humi;
};

extern void SI7021_Init(void);
extern void SI7021_SampleTempHumi(void);
extern int16_t SI7021_GetTemp(void);
extern uint16_t SI7021_GetHumi(void);

#endif
