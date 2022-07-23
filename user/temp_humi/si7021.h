#ifndef SI7021_H
#define SI7021_H

#include <stdint.h>

#define WRITE_CMD  0x80
#define READ_CDM   0x81
#define SALVE_ADDR 0x80

#define HUMI_HOLD_MASTER 0xE5
#define TEMP_HOLD_MASTER 0xE3

#define HUMI_NOHOLD_MASTER 0xF5
#define TEMP_NOHOLD_MASTER 0xF3

struct Si7021Type {
    int16_t temp;
    uint16_t humi;
    uint8_t crc;
};

extern void SI7021_SampleTempHumi(void);
extern int16_t SI7021_GetTemp(void);
extern uint16_t SI7021_GetHumi(void);

#endif
