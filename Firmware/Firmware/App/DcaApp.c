/** INCLUDES ******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DcaApp.h"
	
void DcaApp_Init(void) {
	// Init Watchdog.
	Watchdog_Init();
	
	int8_t s8Status;
    // Initialize Ethernet driver
	
    // Initialize the TCP/IP stack
	printf("\nEthernet Power Up");
    s8Status = Ethernet_Init();
	printf("\nStatus Code %d", s8Status);
	printf("\nEthernet Power Down");
	Ethernet_PowerDown();
}

void DcaApp_Entry(void) {
	printf("\nMain Loop Started");
	while(1) {
		delay_ms(1000);
		printf("\nMain Loop");
		//Watchdog_Feed();
	}
}