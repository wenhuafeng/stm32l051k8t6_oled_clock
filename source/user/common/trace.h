#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>

#define LOGF(tag, format, ...)                                \
    do {                                                      \
        printf("Fatal: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGE(tag, format, ...)                                \
    do {                                                      \
        printf("Error: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGW(tag, format, ...)                                \
    do {                                                      \
        printf("Warn:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGI(tag, format, ...)                                \
    do {                                                      \
        printf("Info:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGD(tag, format, ...)                                \
    do {                                                      \
        printf("Debug: [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)

#endif
