#ifndef TRACE_H
#define TRACE_H

#define UART 0
#define RTT  1
#define DBG_MODULE RTT

#if (DBG_MODULE == UART)
#include <stdio.h>
#define PRINTF printf

#define LOGF(tag, format, ...)                                \
    do {                                                      \
        PRINTF("Fatal: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGE(tag, format, ...)                                \
    do {                                                      \
        PRINTF("Error: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGW(tag, format, ...)                                \
    do {                                                      \
        PRINTF("Warn:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGI(tag, format, ...)                                \
    do {                                                      \
        PRINTF("Info:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGD(tag, format, ...)                                \
    do {                                                      \
        PRINTF("Debug: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)

#define LOG(format, ...)                  \
    do {                                  \
        PRINTF(0, format, ##__VA_ARGS__); \
    } while (0)
#define DBG_INFO(format, ...)          \
    do {                               \
        PRINTF(format, ##__VA_ARGS__); \
    } while (0)

#elif (DBG_MODULE == RTT)
#include "SEGGER_RTT.h"
#define PRINTF SEGGER_RTT_printf

#define LOGF(tag, format, ...)                                   \
    do {                                                         \
        PRINTF(0, "Fatal: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGE(tag, format, ...)                                   \
    do {                                                         \
        PRINTF(0, "Error: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGW(tag, format, ...)                                   \
    do {                                                         \
        PRINTF(0, "Warn:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGI(tag, format, ...)                                   \
    do {                                                         \
        PRINTF(0, "Info:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGD(tag, format, ...)                                   \
    do {                                                         \
        PRINTF(0, "Debug: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)

#define LOG(format, ...)                  \
    do {                                  \
        PRINTF(0, format, ##__VA_ARGS__); \
    } while (0)
#define DBG_INFO(format, ...)             \
    do {                                  \
        PRINTF(0, format, ##__VA_ARGS__); \
    } while (0)

#endif
#endif
