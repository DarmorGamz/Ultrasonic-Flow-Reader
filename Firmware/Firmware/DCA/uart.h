#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include "asf.h"
#include "common/include/nm_common.h"

/** Wifi Init Settings **/
static struct usart_module cdc_uart_module;
struct usart_config usart_conf;

void UART_Init();

#endif
