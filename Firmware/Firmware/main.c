#include <atmel_start.h>
#include <sys/time.h>
#include <App/DcaApp.h>

int main(void) {
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	// Initialize specific functionality
	DcaApp_Init();

	// Main processing loop
	DcaApp_Entry();
	
	// Return Success.
	return true;
}
