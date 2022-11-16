#include <atmel_start.h>
#include <sys/time.h>
#include <App/DcaApp.h>

int main(void) {
	/* Initializes MCU, drivers and middle ware */
	atmel_start_init();
	
	printf("\r\nSystem Boot\n");

	// Initialize the DCA specific functionality
	DcaApp_Init();

	// Main processing loop
	DcaApp_Entry();
	
	// Return Success.
	return true;
}
