/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "utils.h"
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include "Uart.h"

void Uart_Init() {
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);
}