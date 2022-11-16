#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif
	
/** INCLUDES ******************************************************************/
#include <compiler.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define WATCHDOG_TIMEOUT_MS 8000


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void Watchdog_Init(void);
void Watchdog_Feed(void);

#ifdef __cplusplus
}
#endif

#endif
