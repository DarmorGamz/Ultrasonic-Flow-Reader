/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void button_on_PB04_pressed(void)
{
}

static void button_on_PB14_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{

	ext_irq_register(PIN_PB04, button_on_PB04_pressed);
	ext_irq_register(PIN_PB14, button_on_PB14_pressed);
}

/**
 * Example of using WIRELESS_SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_WIRELESS_SPI[12] = "Hello World!";

void WIRELESS_SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&WIRELESS_SPI, &io);

	spi_m_sync_enable(&WIRELESS_SPI);
	io_write(io, example_WIRELESS_SPI, 12);
}

/**
 * Example of using ETHERNET_SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_ETHERNET_SPI[12] = "Hello World!";

void ETHERNET_SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&ETHERNET_SPI, &io);

	spi_m_sync_enable(&ETHERNET_SPI);
	io_write(io, example_ETHERNET_SPI, 12);
}

/**
 * Example of using DEBUG_0 to write "Hello World" using the IO abstraction.
 */
void DEBUG_0_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&DEBUG_0, &io);
	usart_sync_enable(&DEBUG_0);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

void delay_example(void)
{
	delay_ms(5000);
}

static struct timer_task TIMER_0_task1, TIMER_0_task2;

/**
 * Example of using TIMER_0.
 */
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_0_example(void)
{
	TIMER_0_task1.interval = 100;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 200;
	TIMER_0_task2.cb       = TIMER_0_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}

static struct timer_task TICK_TIMER_task1, TICK_TIMER_task2;

/**
 * Example of using TICK_TIMER.
 */
static void TICK_TIMER_task1_cb(const struct timer_task *const timer_task)
{
}

static void TICK_TIMER_task2_cb(const struct timer_task *const timer_task)
{
}

void TICK_TIMER_example(void)
{
	TICK_TIMER_task1.interval = 100;
	TICK_TIMER_task1.cb       = TICK_TIMER_task1_cb;
	TICK_TIMER_task1.mode     = TIMER_TASK_REPEAT;
	TICK_TIMER_task2.interval = 200;
	TICK_TIMER_task2.cb       = TICK_TIMER_task2_cb;
	TICK_TIMER_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TICK_TIMER, &TICK_TIMER_task1);
	timer_add_task(&TICK_TIMER, &TICK_TIMER_task2);
	timer_start(&TICK_TIMER);
}

/**
 * Example of using WDT_0.
 */
void WDT_0_example(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_0, clk_rate, timeout_period);
	wdt_enable(&WDT_0);
}
