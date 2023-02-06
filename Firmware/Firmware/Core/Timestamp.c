/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "Timestamp.h"
#include <sys\_timeval.h>

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
    // Esp initalizes RTC at boot.

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
    if (timestamp < RTC_MINIMUM_TIMESTAMP) {
        s_eRtcStatus = RTC_STATUS_ERROR;
        return false;
    }

    // Create timeval struct and settime.
    struct timeval tv_now;
    tv_now.tv_sec = timestamp;
    tv_now.tv_usec = 0;
    settimeofday(&tv_now, NULL);

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
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);

    // Pull the current count (in seconds) directly from the counter
    *timestamp = (uint32_t)tv_now.tv_sec;
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/

