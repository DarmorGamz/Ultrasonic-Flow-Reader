/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include <atmel_start.h>
#include <tcpip_lite_start.h>

#include <sys/time.h>

static uint32_t   ms_tick = 0;
struct timer_task ms_tick_task;

static void tcpip_lite_timer_task_cb(const struct timer_task *const t)
{
	(void)t;
	ms_tick++;
}

int _gettimeofday(struct timeval *tv, void *tzvp)
{
	if (!tv)
		return -1;

	tv->tv_sec  = ms_tick / 1000;
	tv->tv_usec = ms_tick * 1000;

	return 0;
}

char SPI_ExchangeByte(uint8_t dir, char v)
{

	struct spi_xfer xfer;
	uint8_t         buf = v;

	if (dir == 0) {
		xfer.rxbuf = &buf;
		xfer.txbuf = 0;
		xfer.size  = 1;
	} else {
		xfer.rxbuf = 0;
		xfer.txbuf = &buf;
		xfer.size  = 1;
	}

	spi_m_sync_transfer(&ETHERNET_SPI, &xfer);
	return (char)buf;
}

void tcpip_lite_stack_init(void)
{

	spi_m_sync_enable(&ETHERNET_SPI);

	Network_Init();
	SYSLOG_Init();

	/* Start Timer Task */

	ms_tick_task.interval = 1;
	ms_tick_task.cb       = tcpip_lite_timer_task_cb;
	ms_tick_task.mode     = TIMER_TASK_REPEAT;
	timer_add_task(&TICK_TIMER, &ms_tick_task);
	timer_start(&TICK_TIMER);
}
