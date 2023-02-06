#ifndef WIFI_H_
#define WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif
	
/** INCLUDES ******************************************************************/
#include <compiler.h>


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
int8_t Wifi_Init(void);

void Wifi_PowerDown(void);
void Wifi_PowerUp(void);

#ifdef __cplusplus
}
#endif

#endif
