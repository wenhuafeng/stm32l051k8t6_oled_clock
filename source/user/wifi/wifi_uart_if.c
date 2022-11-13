#include "main.h"
#if defined(WIFI_GET_TIME) && WIFI_GET_TIME
#include "wifi_uart_if.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"
#include "clock.h"
#include "usart.h"
#include "esp8266_at.h"
#include "trace.h"
#include "wifi_uart.h"

#define LOG_TAG "wifi_uart_if"

#define TX_TIMEOUT 1000
#define TX_POLLING(condition)    \
    do {                         \
        uint16_t n = TX_TIMEOUT; \
        while (condition) {      \
            if (--n == 0) {      \
                break;           \
            }                    \
        }                        \
    } while (0)

#define RECEIVE_LENGTH     200
#define SEND_LENGTH        200
#define USART_DMA_SENDING  1
#define USART_DMA_SENDOVER 0

struct UsartReceiveType {
    uint8_t receiveFlag : 1;
    uint8_t sendFlag : 1;
    uint16_t rxLength;
    uint8_t buffer[RECEIVE_LENGTH];
};
static struct UsartReceiveType g_usartType;

void WIFI_UART_ReceiveDmaInit(void)
{
    // configure dma source & destination
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_RECEIVE),
                           (uint32_t)g_usartType.buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    // configure data length
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, sizeof(g_usartType.buffer));

    // enable interrupts
    LL_USART_EnableIT_IDLE(USART2);

    // enable DMA stream
    LL_USART_EnableDMAReq_RX(USART2);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
}

void WIFI_UART_ReceiveIdle(void)
{
    enum WifiReceiveInfo info;
    struct Esp8266GetTimeType *wifi = GetWifiData();

    if (LL_USART_IsActiveFlag_IDLE(USART2)) {
        LL_USART_ClearFlag_IDLE(USART2);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);

        g_usartType.rxLength = sizeof(g_usartType.buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);

        //clear DMA flags
        LL_DMA_ClearFlag_GI5(DMA1);
        LL_DMA_ClearFlag_HT5(DMA1);
        LL_DMA_ClearFlag_TC5(DMA1);
        LL_DMA_ClearFlag_TE5(DMA1);

        WIFI_UART_ReceiveDmaInit();

        info = WIFI_ReceiveProcess(wifi, g_usartType.buffer);
        if (info == WIFI_GET_TIME_COMPLETE) {
            CLOCK_Set(&wifi->time);
            CLOCK_Get();
        }
        memset(g_usartType.buffer, 0, sizeof(g_usartType.buffer));
    }
}

void WIFI_UART_TxCpltCallback(USART_TypeDef *huart)
{
}

void WIFI_UART_RxCpltCallback(USART_TypeDef *huart)
{
}

//static void USART_Printf(USART_TypeDef *USARTx, const uint8_t *buffer, size_t len)
//{
//    size_t idx = 0;
//
//    while (idx < len) {
//        LL_USART_TransmitData8(USARTx, *(buffer + idx));
//        TX_POLLING(!LL_USART_IsActiveFlag_TC(USARTx));
//        idx++;
//    }
//}

void PrintUsart2(char *format, ...)
{
    char buf[SEND_LENGTH];
    int length;

    va_list ap;
    va_start(ap, format);
    length = vsprintf(buf, format, ap);
    if (length > 0) {
        //USART_Printf(USART2, (uint8_t *)buf, length);
        uart_write(DEV_UART2, (uint8_t *)buf, length);
    }
    va_end(ap);
}

#else

void WIFI_UART_ReceiveIDLE(void)
{
}
void WIFI_UART_TxCpltCallback(USART_TypeDef *huart)
{
    (void)huart;
}
void WIFI_UART_RxCpltCallback(USART_TypeDef *huart)
{
    (void)huart;
}

#endif
