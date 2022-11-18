#ifndef DCAAPP_H_
#define DCAAPP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "FwVersion.h"

#include "Core/Watchdog.h"
#include "Core/Wifi.h"
#include "Core/Ethernet.h"
#include "Core/Timer.h"

/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define MASTER_CLOCK_FREQUENCY_HZ            8000000 // 8MHz master clock
#define RT_CLOCK_FREQUENCY_HZ                1024    //  1.024kHz RTC clock (WDT and Timers)

/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void DcaApp_Init(void);
void DcaApp_Entry(void);

#ifdef __cplusplus
}
#endif

#endif