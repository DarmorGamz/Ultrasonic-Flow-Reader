/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "Timestamp.h"
#include <sys\_timeval.h>
#include "hpl_pm_base.h"
#include "hpl_gclk_base.h"

/** VARIABLES *****************************************************************/
static eRtcStatus s_eRtcStatus;


/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
/**************************************************************************//**
 *  Initialization of system time functionality (using the RTC peripheral).
 *  @param[in]  None
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void Timestamp_Init(void) {
    // Configure the clock for the RTC
    _pm_enable_bus_clock(PM_BUS_APBA, RTC);
    // Coming from clock generator 1 output (1024Hz from OSC32K source)
    _gclk_enable_channel(RTC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK1_Val);

    // Disable the RTC peripheral
    RTC->MODE0.CTRL.bit.ENABLE = 0;
    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}

    // Initiate software reset
    RTC->MODE0.CTRL.bit.SWRST = 1;
    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}

    // Set mode (32-bit) and prescaler (1024)
    //RTC->MODE0.CTRL.bit.MODE = RTC_MODE0_CTRL_MODE_COUNT32_Val;
    //RTC->MODE0.CTRL.bit.PRESCALER = RTC_MODE0_CTRL_PRESCALER_DIV1024_Val;
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32 | RTC_MODE0_CTRL_PRESCALER_DIV1024;

    // Set the initial value
    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}
    RTC->MODE0.COUNT.reg = RTC_MINIMUM_TIMESTAMP;
    //// Set continuous update
    //RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ | RTC_READREQ_RCONT;

    // Enable the peripheral
    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}
    RTC->MODE0.CTRL.bit.ENABLE = 1;

    // Set status flag
    s_eRtcStatus = RTC_STATUS_INIT;
}

/**************************************************************************//**
 *  Retrieve the current status of the RTC time setting.
 *  @param[in]  None
 *  @param[out] None
 *  @return     eRtcStatus  Current status of the RTC time setting.
 ******************************************************************************/
eRtcStatus Timestamp_GetStatus(void) {
    return s_eRtcStatus;
}

/**************************************************************************//**
 *  Retrieve the current status of the RTC time setting.
 *  @param[in]  None
 *  @param[out] None
 *  @return     boolean indicating if the RTC has been set.
 ******************************************************************************/
bool Timestamp_IsSynchronized(void) {
    if(s_eRtcStatus <= RTC_STATUS_ERROR) return false;
    else return true;
    return true;
}


/**************************************************************************//**
 *  Set the RTC value using a timestamp (seconds from Unix epoch).
 *  @param[in]  uint32  Unix timestamp
 *  @param[out] None
 *  @return     true=success, false=failure
 ******************************************************************************/
bool Timestamp_SetTimestamp(uint32_t timestamp) {
    // Determine if the timestamp passed in is valid
    if (timestamp<RTC_MINIMUM_TIMESTAMP) {
        s_eRtcStatus = RTC_STATUS_ERROR;
        return false;
    }

    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}
    RTC->MODE0.COUNT.reg = timestamp;

    s_eRtcStatus = RTC_STATUS_SYNC_SUCCESS;
    return true;
}
/**************************************************************************//**
 *  Get the RTC value in an epoch timestamp format.
 *  @param[in]  None
 *  @param[out] uint32  Current system time in timestamp (seconds from Unix epoch)
 *  @return     Nothing
 ******************************************************************************/
void Timestamp_GetTimestamp(uint32_t *timestamp) {
    // Request read
    RTC->MODE0.READREQ.bit.RREQ = 1;
    while(RTC->MODE0.STATUS.bit.SYNCBUSY){/* Wait for sync */}

    // Pull the current count (in seconds) directly from the counter
    *timestamp = RTC->MODE0.COUNT.reg;
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/

