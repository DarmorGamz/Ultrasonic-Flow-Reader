/** Include **/
#include "asf.h"
#include "DCA/wifi.h"
#include "DCA/uart.h"

/** Prototypes **/

/** Data Members **/

/** Methods **/
int main(void) {
	system_init();
	UART_Init();
	Wifi_Init();

	//Wifi_Init_Default();
	Wifi_Init_AP();
	while (1) {
		 while(m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {}
			 
			 
	}
	// Return Success.
	return true;
}
