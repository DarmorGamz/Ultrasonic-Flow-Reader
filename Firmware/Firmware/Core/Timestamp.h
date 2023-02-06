#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** INCLUDES ******************************************************************/


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define RTC_MINIMUM_TIMESTAMP   1577836800  // 01-Jan-2020 00:00:00
#define RTC_MAXIMUM_TIMESTAMP   2524608000  // 01-Jan-2050 00:00:00


/** TYPEDEFS, STRUCTURES AND ENUMERATIONS *************************************/
// RTC status
typedef enum {
    RTC_STATUS_INIT = 0,    // Initial status
    RTC_STATUS_ERROR,       // Time synchronization with server failed
    RTC_STATUS_SYNC_SUCCESS // Time synchronization successful
} eRtcStatus;


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void Timestamp_Init(void);
//eRtcStatus Timestamp_GetStatus(void);
bool Timestamp_IsSynchronized(void);
bool Timestamp_SetTimestamp(uint32_t timestamp);
void Timestamp_GetTimestamp(uint32_t *timestamp);


/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* TIMESTAMP_H_ */