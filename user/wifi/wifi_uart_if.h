#ifndef WIFI_UART_IF_H
#define WIFI_UART_IF_H

extern void WIFI_UART_ReceiveDmaInit(void);
extern void WIFI_UART_ReceiveIdle(void);
extern void WIFI_UART_TxCpltCallback(USART_TypeDef *huart);
extern void WIFI_UART_RxCpltCallback(USART_TypeDef *huart);
extern void PrintUsart2(char *format, ...);

#endif
