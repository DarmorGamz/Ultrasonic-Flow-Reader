/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdlib.h>
#include "driver_init.h"
#include "winc_init.h"
#include "socket/include/socket.h"
#include "driver/include/m2m_wifi.h"
#include "driver/include/m2m_ota.h"
#include "Wifi.h"
#include "App/DcaApp.h" 


/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/



/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
int8_t Wifi_Init(void) {
    return 0;
}

void Wifi_PowerDown(void) {
	m2m_wifi_deinit(NULL);
	//nm_bsp_deinit();
}

void Wifi_PowerUp(void) {
	Wifi_Init();
}