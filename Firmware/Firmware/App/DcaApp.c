/** INCLUDES ******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DcaApp.h"
	
void DcaApp_Init(void) {
	// Init Watchdog.
	Watchdog_Init();
}

void DcaApp_Entry(void) {
	while(1) {
		delay_ms(1000);
		printf("\nWatchdog Timer 1 second");
		Watchdog_Feed();
		
		delay_ms(2000);
		printf("\nWatchdog Timer 2 second");
		Watchdog_Feed();
		
		delay_ms(4000);
		printf("\nWatchdog Timer 4 second");
		Watchdog_Feed();
		
		delay_ms(8000);
		printf("\nWatchdog Timer 8 second");
		Watchdog_Feed();
		
		delay_ms(9000);
		printf("\nWatchdog Timer 9 second");
		Watchdog_Feed();
	}
}