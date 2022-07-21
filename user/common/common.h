#ifndef COMMON_H
#define COMMON_H

#include "main.h"

#ifdef __CC_ARM
#define NOP() __nop()
#elif __GNUC__
#define NOP() asm("nop")
#endif

#define LED_BLINK()                                        \
    do {                                                   \
        LL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin); \
    } while (0)
#define LED_ON()                                                \
    do {                                                        \
        LL_GPIO_ResetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin); \
    } while (0)
#define LED_OFF()                                             \
    do {                                                      \
        LL_GPIO_SetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin); \
    } while (0)

extern void LPTIM1_IsrHandle(void);
extern void COMMON_Init(void);
extern void COMMON_Function(void);

#endif
