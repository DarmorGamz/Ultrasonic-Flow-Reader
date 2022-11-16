/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

struct spi_m_sync_descriptor WIRELESS_SPI;
struct spi_m_sync_descriptor ETHERNET_SPI;
struct timer_descriptor      TIMER_0;
struct timer_descriptor      TICK_TIMER;

struct usart_sync_descriptor DEBUG_0;

struct wdt_descriptor WDT_0;

void EXTERNAL_IRQ_0_init(void)
{
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	// Set pin direction to input
	gpio_set_pin_direction(WIRELESS_IRQ, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(WIRELESS_IRQ,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(WIRELESS_IRQ, PINMUX_PB04A_EIC_EXTINT4);

	// Set pin direction to input
	gpio_set_pin_direction(ETHERNET_IRQ, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ETHERNET_IRQ,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ETHERNET_IRQ, PINMUX_PB14A_EIC_EXTINT14);

	ext_irq_init();
}

void WIRELESS_SPI_PORT_init(void)
{

	// Set pin direction to input
	gpio_set_pin_direction(WIRELESS_MISO, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(WIRELESS_MISO,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_UP);

	gpio_set_pin_function(WIRELESS_MISO, PINMUX_PA04D_SERCOM0_PAD0);

	gpio_set_pin_level(WIRELESS_MOSI,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(WIRELESS_MOSI, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(WIRELESS_MOSI, PINMUX_PA06D_SERCOM0_PAD2);

	gpio_set_pin_level(WIRELESS_SCK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(WIRELESS_SCK, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(WIRELESS_SCK, PINMUX_PA07D_SERCOM0_PAD3);
}

void WIRELESS_SPI_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

void WIRELESS_SPI_init(void)
{
	WIRELESS_SPI_CLOCK_init();
	spi_m_sync_init(&WIRELESS_SPI, SERCOM0);
	WIRELESS_SPI_PORT_init();
}

void ETHERNET_SPI_PORT_init(void)
{

	gpio_set_pin_level(ETHERNET_MISO,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ETHERNET_MISO, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETHERNET_MISO, PINMUX_PA16C_SERCOM1_PAD0);

	// Set pin direction to input
	gpio_set_pin_direction(ETHERNET_MOSI, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ETHERNET_MOSI,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ETHERNET_MOSI, PINMUX_PA18C_SERCOM1_PAD2);

	gpio_set_pin_level(ETHERNET_SCK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ETHERNET_SCK, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETHERNET_SCK, PINMUX_PA19C_SERCOM1_PAD3);
}

void ETHERNET_SPI_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM1);
	_gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC);
}

void ETHERNET_SPI_init(void)
{
	ETHERNET_SPI_CLOCK_init();
	spi_m_sync_init(&ETHERNET_SPI, SERCOM1);
	ETHERNET_SPI_PORT_init();
}

void DEBUG_0_PORT_init(void)
{

	gpio_set_pin_function(VIRTUAL_TX, PINMUX_PA22C_SERCOM3_PAD0);

	gpio_set_pin_function(VIRTUAL_RX, PINMUX_PA23C_SERCOM3_PAD1);
}

void DEBUG_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM3);
	_gclk_enable_channel(SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC);
}

void DEBUG_0_init(void)
{
	DEBUG_0_CLOCK_init();
	usart_sync_init(&DEBUG_0, SERCOM3, (void *)NULL);
	DEBUG_0_PORT_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_0_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, TC3);
	_gclk_enable_channel(TC3_GCLK_ID, CONF_GCLK_TC3_SRC);

	timer_init(&TIMER_0, TC3, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TICK_TIMER_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, TC4);
	_gclk_enable_channel(TC4_GCLK_ID, CONF_GCLK_TC4_SRC);

	timer_init(&TICK_TIMER, TC4, _tc_get_timer());
}

void WDT_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBA, WDT);
	_gclk_enable_channel(WDT_GCLK_ID, CONF_GCLK_WDT_SRC);
}

void WDT_0_init(void)
{
	WDT_0_CLOCK_init();
	wdt_init(&WDT_0, WDT);
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA02

	gpio_set_pin_level(ETHERNET_CS,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(ETHERNET_CS, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETHERNET_CS, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA20

	gpio_set_pin_level(ETHERNET_RESET,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ETHERNET_RESET, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETHERNET_RESET, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB05

	gpio_set_pin_level(WIRELESS_EN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(WIRELESS_EN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(WIRELESS_EN, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB06

	gpio_set_pin_level(WIRELESS_RESET,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(WIRELESS_RESET, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(WIRELESS_RESET, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB07

	gpio_set_pin_level(WIRELESS_CS,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(WIRELESS_CS, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(WIRELESS_CS, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_0_init();

	WIRELESS_SPI_init();

	ETHERNET_SPI_init();

	DEBUG_0_init();

	delay_driver_init();

	TIMER_0_init();
	TICK_TIMER_init();

	WDT_0_init();
}
