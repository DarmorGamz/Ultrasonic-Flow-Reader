/** INCLUDES ******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DcaApp.h"


typedef enum {
	LED_COLOR_NONE = 0,
	LED_COLOR_GREEN,
	LED_COLOR_RED,
	LED_COLOR_BOTH
} eLED_COLOR;

// Global buffers
char                g_acBuffer64[STRSM_BUFFER_NUM_BYTES];
char                g_acBuffer256[STRLG_BUFFER_NUM_BYTES];
char                g_acRxBuffer[RX_BUFFER_NUM_BYTES];
char                g_acTxBuffer[TX_BUFFER_NUM_BYTES];
char                g_acLocalRxBuffer[LOCAL_RX_BUFFER_NUM_BYTES];
char                g_acLocalTxBuffer[LOCAL_TX_BUFFER_NUM_BYTES];

// Command FIFO and reading storage
char                g_acCmdFifo[COMMAND_FIFO_NUM_BYTES];
//char                g_acReadingBuffer[READING_BUFF_NUM_BYTES];

// ------ TIMERS ------
static uint16_t     TIMER_APP_LED_HOLD;
static uint16_t     TIMER_APP_UPDATE_CLOCK;
static uint16_t     TIMER_APP_ETHERNET_LINK_TIMEOUT;
static uint16_t     TIMER_APP_HEARTBEAT;
static uint16_t     TIMER_APP_ENWI_LINK_CHECK;


// ------ Control logic ------
static uint8_t      s_u8StatusCode;
static uint16_t     s_u16HeartbeatRate;



char                g_acHostnameSerialString[9];    // e.g. "81234567"
volatile uint8_t    padding;
uint8_t             g_au8MacAddress[6];

/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/
#define LED_SEQUENCE_MAXCNT 4
const uint8_t STATUS_LED[STATUS_COUNT][LED_SEQUENCE_MAXCNT] =
{ // mapped with eStatusCode
    {LED_COLOR_RED,   LED_COLOR_RED,   LED_COLOR_RED,   LED_COLOR_RED},   // STATUS_DEVICE_ERROR
    {LED_COLOR_GREEN, LED_COLOR_GREEN, LED_COLOR_NONE,  LED_COLOR_NONE},  // STATUS_OK (provisioned)
	{LED_COLOR_RED,   LED_COLOR_GREEN,   LED_COLOR_RED,   LED_COLOR_GREEN},   // STATUS_DEVICE_ERROR
	{LED_COLOR_RED,   LED_COLOR_RED,   LED_COLOR_RED,   LED_COLOR_RED},   // STATUS_DEVICE_ERROR
};

#define TIMEOUT_LED_HOLD                     30

/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/
void _LedColorControl(eLED_COLOR color);
void _LedControl(void);
void _SetLed(uint8_t value);

bool _AddToFifo_EventStartup(void);

static bool         s_fUseWifi;

static void _One_Second_Timer_Callback(void) {
    bool fSkipPump = false;
    bool fDoublePump = false;


    // Pump the timer driver (decrements all non-zero timers)
    if (fSkipPump==false) Timer_Pump();
    if (fDoublePump==true) Timer_Pump();

	//_AddToFifo_EventStartup();
	
    // Toggle the LEDs based on the current status code if not currently holding the user set LED value (setvar&led={val})
    if (Timer_GetTimer(TIMER_APP_LED_HOLD)==0) _LedControl();
}

void DcaApp_Init(void) {
	int16_t u16Status;

    // Capture reset cause
    uint8_t u8ResetCause = PM->RCAUSE.reg;
	
	// Change BOD33 level to 2.84V(typ) from default 1.75V
    SYSCTRL->BOD33.bit.ENABLE = 0; // Disable
    SYSCTRL->BOD33.bit.LEVEL = 39; // 2.84V (from datasheet)
    SYSCTRL->BOD33.bit.ENABLE = 1; // Re-enable

	// Initialize the debug log
    Debug_InitLog();
    DEBUG_INFO("Debug log initialized. ");

    // Initialize the watchdog timer
    Watchdog_Init();
    DEBUG_INFO("Watchdog initialized. ");
	
	// Initialize the Timers
    Timer_Init();
    DEBUG_INFO("Timers initialized. ");
    // Create the timers required by the app - nothing to go wrong so intentionally ignoring return value
    Timer_AddTimer(&TIMER_APP_LED_HOLD);
	Timer_AddTimer(&TIMER_APP_HEARTBEAT);
    Timer_AddTimer(&TIMER_APP_UPDATE_CLOCK);
	Timer_AddTimer(&TIMER_APP_ENWI_LINK_CHECK);
	
	// Initialize Command FIFO - do here so we can queue up any events that occur
	EyedroCmdFifo_Init();
	DEBUG_INFO("FIFO initialized. ");
	
	// Initialize flash 
    Flash_Init();
    DEBUG_INFO("Flash initialized. ");
	
    int8_t s8Status;
    // Initialize Ethernet driver
    s8Status = Ethernet_Init();
    if (s8Status!=0) {
        _LedColorControl(LED_COLOR_RED);
        while (1) {} // Infinite loop 
    }
    DEBUG_INFO("Ethernet initialized. ");


    // Initialize WiFi driver
    s8Status = Wifi_Init();
    if (s8Status!=0) {
        _LedColorControl(LED_COLOR_RED);
        while (1) {} // Infinite loop
    }
    DEBUG_INFO("WiFi initialized. ");
	
	// Power down Ethernet and WiFi 
    Ethernet_PowerDown();
    //Wifi_PowerDown(); Not working

    DEBUG_INFO("Application init complete. ");
	
	// Add the startup event to the command FIFO
    //_AddToFifo_EventStartup();
	
	s_u16HeartbeatRate = EYEDRO_TIMEOUT_STARTUP_HEARTBEAT;
	
    // Register Timer callback
    Timer_Register_Callback(_One_Second_Timer_Callback);

}

void DcaApp_Entry(void) {
	static bool s_fCheckLink = true;

	DEBUG_INFO("Entering main state machine... ");
	
	// Initialize the status
	App_SetStatus(STATUS_OK);
	
	while(1) {
		// Feed the watchdog every time through this callback - if not, the device will reset after 8 seconds
		Watchdog_Feed();
		
		if (s_fCheckLink==true) {
            DEBUG_INFO("Power up Ethernet. ");
            Ethernet_PowerUp();
			
			// Check for Ethernet link
            bool fLinked = false;
            Timer_SetTimer(TIMER_APP_ETHERNET_LINK_TIMEOUT, EYEDRO_TIMEOUT_ETHERNET_LINK);
            while(Timer_GetTimer(TIMER_APP_ETHERNET_LINK_TIMEOUT)>0) {
                Watchdog_Feed();
                if (Ethernet_HasLink()==true) {
                    fLinked = true;
                    Timer_SetTimer(TIMER_APP_ETHERNET_LINK_TIMEOUT, 0);
                }
            }
			
			// If Ethernet link is found, use it
            if (fLinked==true) {
                DEBUG_INFO("Ethernet link up. ");
                // Power down WiFi module
                DEBUG_INFO("Power down Wifi. ");
                //Wifi_PowerDown();
                s_fUseWifi = false;

            // Otherwise try the Wi-Fi
            }  else {
                DEBUG_INFO("Ethernet link down. ");
                // Power down Ethernet controller
                DEBUG_INFO("Power down Ethernet. ");
                Ethernet_PowerDown();
                // Power up WiFi
                DEBUG_INFO("Power up WiFi. ");
                //Wifi_PowerUp();
                s_fUseWifi = true;
				while(true) {}
            }
			s_fCheckLink = false;
		}
		
		if (s_fUseWifi==true) {} // Wifi_PumpEvents();
		if (s_fUseWifi==false) Ethernet_PumpEvents();
		
		// Determine if it is time to send to the server
        bool fTimeToSend = false;
		
		if (Timer_GetTimer(TIMER_APP_HEARTBEAT)==0) {
            // Set the flag indicating that it's time to send (regardless if the heartbeat was added)
            fTimeToSend = true;
        }
		
		
		// It's time to send when... there is anything in the command FIFO
            if (EyedroCmdFifo_GetByteCount()>0) {
                fTimeToSend = true;
            }

			
		// Reset the link check timer
        Timer_SetTimer(TIMER_APP_ENWI_LINK_CHECK, 300);
		
		// Transmit any pending packets/responses to server if it has been determined we need to send
        if (fTimeToSend==true) {
            Ethernet_QueueServerSend();
			
            // Reset the heartbeat timer
            Timer_SetTimer(TIMER_APP_HEARTBEAT, s_u16HeartbeatRate);
        }
		
		
	} // End of application processing (while) loop... go back and do it all again!
}



void _LedColorControl(eLED_COLOR color) {
    // Set the LEDs according to input colour enumeration
    switch(color) {
        case LED_COLOR_NONE:
            gpio_set_pin_level(LED_RED, false);
            gpio_set_pin_level(LED_GREEN, false);
            break;
        case LED_COLOR_GREEN:
            gpio_set_pin_level(LED_RED, false);
            gpio_set_pin_level(LED_GREEN, true);
            break;
        case LED_COLOR_RED:
            gpio_set_pin_level(LED_RED, true);
            gpio_set_pin_level(LED_GREEN, false);
            break;
        case LED_COLOR_BOTH:
            gpio_set_pin_level(LED_RED, true);
            gpio_set_pin_level(LED_GREEN, true);
            break;
        default:
            break;
    }
}

void _LedControl(void) {
    static uint8_t index = 0;

    // Get current system status
    eStatusCode code = App_GetStatus();
    // Get LED color data
    eLED_COLOR color =	STATUS_LED[code][(index++)%LED_SEQUENCE_MAXCNT];
    // Push the color to the LEDs
    _LedColorControl(color);

}

void _SetLed(uint8_t value) {
    // Mask out all but the bottom 2 bits and push to the LEDs
    value&=0x03;
    _LedColorControl((eLED_COLOR)value);
    // Hold LED value for 30 seconds
    Timer_SetTimer(TIMER_APP_LED_HOLD, TIMEOUT_LED_HOLD);
}


void App_SetStatus(eStatusCode code) {
    s_u8StatusCode = code;
}

eStatusCode App_GetStatus(void) {
    return s_u8StatusCode;
}

bool _AddToFifo_EventStartup(void) {
    uint8_t u8PayloadLength, u8TotalLength;
    char* pCmd;

    // Compute length
    //u8PayloadLength = EVENT_PAYLOAD_LENGTH_STARTUP_V3 + 1;
    u8TotalLength = 4;

    // Make sure there is room in the FIFO before proceeding
    if (EyedroCmdFifo_GetByteRemaining() < u8TotalLength) return false;

    // Use the shared 64-byte buffer to build the command
    pCmd = &g_acBuffer64[0];

    // setevent command byte
    *pCmd++ = (char)'T';
	*pCmd++ = (char)'I';
	*pCmd++ = (char)'M';
	*pCmd++ = (char)'E';

    // Push to the command FIFO
    EyedroCmdFifo_Push(&g_acBuffer64[0], u8TotalLength);

    return true;
}