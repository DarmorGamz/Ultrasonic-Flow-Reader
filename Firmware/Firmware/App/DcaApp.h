#ifndef DCAAPP_H_
#define DCAAPP_H_

#ifdef __cplusplus
extern "C"	{
#endif

/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "FwVersion.h"

#include "Core/Watchdog.h"
#include "Core/Wifi.h"
#include "Core/Ethernet.h"
#include "Core/Flash.h"
#include "Core/Http.h"
#include "Core/Common.h"
#include "Core/Timer.h"
#include "Core/Timestamp.h"
#include "Core/Debug.h"


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define RT_CLOCK_FREQUENCY_HZ                1024    //  1.024kHz RTC clock (WDT and Timers)

#define STRSM_BUFFER_NUM_BYTES                      64      // Block of shared RAM - happens to be a page size
#define STRLG_BUFFER_NUM_BYTES                      256     // Block of shared RAM - happens to be a row size

#define RX_BUFFER_NUM_BYTES                         2048    // Block of RAM containing data received from remote connection
#define TX_BUFFER_NUM_BYTES                         1024    // Block of RAM containing data to be transmitted upstream (to server or gateway)

#define LOCAL_RX_BUFFER_NUM_BYTES                   1024    // Block of RAM containing data received from local socket connection
#define LOCAL_TX_BUFFER_NUM_BYTES                   5120    // Block of RAM containing data to be transmitted on local socket connection

#define COMMAND_FIFO_NUM_BYTES                  4096    // Block of RAM containing commands (and responses) going to server - large size to support gateway requirements

#define EYEDRO_TIMEOUT_WIFI_CONNECTION              30
#define EYEDRO_TIMEOUT_DHCP_RESPONSE                3
#define EYEDRO_TIMEOUT_WIFI_CONNECTION_RETRY        180
#define EYEDRO_TIMEOUT_SOCKET_LOCAL_INIT            900
#define EYEDRO_TIMEOUT_SOCKET_REMOTE_INIT           10
#define EYEDRO_TIMEOUT_SOCKET_REMOTE_RESPONSE       10
#define EYEDRO_TIMEOUT_DNS_RESOLUTION               3
#define EYEDRO_TIMEOUT_DNS_RERESOLVE                3600    // Re-resolve the DNS every hour
#define EYEDRO_TIMEOUT_LED_HOLD                     30
#define EYEDRO_TIMEOUT_READ_INTERNAL_SENSORS        300
#define EYEDRO_TIMEOUT_SOFT_RESET_DELAY             3
#define EYEDRO_TIMEOUT_UPDATE_RSSI                  60
#define EYEDRO_TIMEOUT_TEMP_HEARTBEAT_HOLD          900
#define EYEDRO_TIMEOUT_STARTUP_HEARTBEAT            2
#define EYEDRO_TIMEOUT_STARTUP_HEARTBEAT_HOLD       300
#define EYEDRO_TIMEOUT_FWUP_PAGE_RESPONSE           3
#define EYEDRO_TIMEOUT_COS_RESEND_INTERVAL          300
#define EYEDRO_TIMEOUT_ESTABLISH_COMM               600
#define EYEDRO_TIMEOUT_PROCESS_DATA                 120     // 2 minutes of consecutive non-processing of data will reset device
#define EYEDRO_TIMEOUT_ETHERNET_LINK                3
#define EYEDRO_TIMEOUT_MESH_NODE_CONNECTIVITY       120

extern char                 g_acHostnameSerialString[];
extern uint8_t              g_au8MacAddress[];

#define CFG_DEFAULT_HOST                      "hw.darmorgamz.ca"
#define CFG_DEFAULT_PATH                      "data"

typedef enum {
	STATUS_ERROR, 
	STATUS_OK,        
	STATUS_NO_SERVER_COMM,                   // Set when supposed to be in station mode but can't reach server                      
	STATUS_COUNT
} eStatusCode;

/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void DcaApp_Init(void);
void DcaApp_Entry(void);

void App_SetStatus(eStatusCode code);
eStatusCode App_GetStatus(void);
#ifdef __cplusplus
}
#endif

#endif
