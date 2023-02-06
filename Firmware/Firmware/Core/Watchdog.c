/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "Watchdog.h"
#include <App/DcaApp.h>

void Watchdog_Init() {
	// Configure the dog.
	uint16_t timeout_period;
	timeout_period = WATCHDOG_TIMEOUT_MS; // 8s
	wdt_set_timeout_period(&WDT_0, RT_CLOCK_FREQUENCY_HZ, timeout_period);

	// Enable the dog.
	wdt_enable(&WDT_0);
}

void Watchdog_Feed(void) {
	wdt_feed(&WDT_0);
}