#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

struct TimeType {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t week;
    uint8_t month;
    uint16_t year;
};

void CLOCK_CalculateWeek(struct TimeType *time);
bool CLOCK_Run(void);
void CLOCK_Get(void);
void CLOCK_Set(struct TimeType *time);
struct TimeType *CLOCK_GetData(void);

#endif
