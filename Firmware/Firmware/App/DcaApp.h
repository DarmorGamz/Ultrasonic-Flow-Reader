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
#include "Core/Sensor.h"

#include <string.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
typedef struct {
	uint16_t  u16InitCode;                              // Gets set to 0x5A5A once structure is valid
    uint8_t   au8Timestamp[4];                          // Used to retrieve a sane timestamp on startup (soft reset will be pretty close)
} tsUserConfig;

#define RT_CLOCK_FREQUENCY_HZ                1024    //  1.024kHz RTC clock (WDT and Timers)

#define STRSM_BUFFER_NUM_BYTES                      64      // Block of shared RAM - happens to be a page size
#define STRLG_BUFFER_NUM_BYTES                      256     // Block of shared RAM - happens to be a row size

#define RX_BUFFER_NUM_BYTES                         2048    // Block of RAM containing data received from remote connection
#define TX_BUFFER_NUM_BYTES                         1024    // Block of RAM containing data to be transmitted upstream (to server or gateway)

#define LOCAL_RX_BUFFER_NUM_BYTES                   1024    // Block of RAM containing data received from local socket connection
#define LOCAL_TX_BUFFER_NUM_BYTES                   5120    // Block of RAM containing data to be transmitted on local socket connection

#define COMMAND_FIFO_NUM_BYTES                  4096    // Block of RAM containing commands (and responses) going to server - large size to support gateway requirements
#define READING_BUFF_NUM_BYTES					128

// Memory mapping - address offsets
#define BOOTLOADER_SPACE_ORIGIN                     0x00000000
#define NVMSTATUS_SPACE_ORIGIN                      0x00000400
#define STORAGESTATUS_SPACE_ORIGIN                  0x00000500
#define USER_CONFIG_SPACE_ORIGIN                    0x0000CCCC
#define APPLICATION_SPACE_ORIGIN                    0x00000C00
#define STORAGE_SPACE_ORIGIN                    0x00020600

// Memory mapping - size in bytes
#define BOOTLOADER_SPACE_SIZE_BYTES                 0x00000400
#define NVMSTATUS_SPACE_SIZE_BYTES                  0x00000100
#define STORAGESTATUS_SPACE_SIZE_BYTES              0x00000100
#define USER_CONFIG_SPACE_SIZE_BYTES                0x00000200
#define APPLICATION_SPACE_SIZE_BYTES            (uint32_t)0x0001FA00
#define STORAGE_SPACE_SIZE_BYTES                (uint32_t)0x0001FA00

// Most NVM access is done via (64 byte) pages or (256 byte) rows
// Memory mapping - size in pages
#define BOOTLOADER_SPACE_SIZE_PAGES                 (BOOTLOADER_SPACE_SIZE_BYTES/0x40)
#define NVMSTATUS_SPACE_SIZE_PAGES                  (NVMSTATUS_SPACE_SIZE_BYTES/0x40)
#define STORAGESTATUS_SPACE_SIZE_PAGES              (STORAGESTATUS_SPACE_SIZE_BYTES/0x40)
#define USER_CONFIG_SPACE_SIZE_PAGES                (USER_CONFIG_SPACE_SIZE_BYTES/0x40)
#define MESH_CONFIG_SPACE_SIZE_PAGES                (MESH_CONFIG_SPACE_SIZE_BYTES/0x40)
#define APPLICATION_SPACE_SIZE_PAGES                (APPLICATION_SPACE_SIZE_BYTES/0x40)
#define STORAGE_SPACE_SIZE_PAGES                    (STORAGE_SPACE_SIZE_BYTES/0x40)
// NVM storage will work in rows (256 bytes)
#define STORAGE_SPACE_SIZE_ROWS                     (STORAGE_SPACE_SIZE_BYTES/0x100)

#define TIMEOUT_WIFI_CONNECTION              30
#define TIMEOUT_DHCP_RESPONSE                3
#define TIMEOUT_WIFI_CONNECTION_RETRY        180
#define TIMEOUT_SOCKET_LOCAL_INIT            900
#define TIMEOUT_SOCKET_REMOTE_INIT           10
#define TIMEOUT_SOCKET_REMOTE_RESPONSE       10
#define TIMEOUT_DNS_RESOLUTION               3
#define TIMEOUT_DNS_RERESOLVE                3600    // Re-resolve the DNS every hour
#define TIMEOUT_LED_HOLD                     30
#define TIMEOUT_READ_INTERNAL_SENSORS        300
#define TIMEOUT_SOFT_RESET_DELAY             3
#define TIMEOUT_UPDATE_RSSI                  60
#define TIMEOUT_TEMP_HEARTBEAT_HOLD          900
#define TIMEOUT_STARTUP_HEARTBEAT            2
#define TIMEOUT_STARTUP_HEARTBEAT_HOLD       60 //300
#define TIMEOUT_FWUP_PAGE_RESPONSE           3
#define TIMEOUT_COS_RESEND_INTERVAL          300
#define TIMEOUT_ESTABLISH_COMM               600
#define TIMEOUT_PROCESS_DATA                 120     // 2 minutes of consecutive non-processing of data will reset device
#define TIMEOUT_ETHERNET_LINK                3
#define TIMEOUT_MESH_NODE_CONNECTIVITY       120

extern char                 g_acHostnameSerialString[];
extern uint8_t              g_au8MacAddress[];

static volatile uint8_t	    s_u8ReadingBufferUsed;

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

void App_ProcessBinaryPayload(char *pSrc, int nSrcBytes);

uint32_t _get_tick_count(void);

#ifdef __cplusplus
}
#endif

#endif
