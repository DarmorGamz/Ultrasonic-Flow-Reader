/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "Timer.h"
#include <string.h>
#include <App/DcaApp.h>

/** VARIABLES *****************************************************************/
static uint16_t s_au16Timers[TIMER_MAX_COUNT];
static uint16_t s_u16TimerCount;


/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
/**************************************************************************//**
 *  Initialization of timer. Used to maintain system timers and generate periodic
 *  callback on (one second) roll over.
 *  @param[in]  None
 *  @param[out] None
 *  @return     0 indicates success. Anything else is an error.
 ******************************************************************************/
int16_t Timer_Init(void) {
    // Initialization of TIMER_0 already done in driver_init.c

    // Clear the contents of the timer array
    memset(&s_au16Timers[0], 0, sizeof(s_au16Timers));
    // Reset timer counter
    s_u16TimerCount = 0;
	
	// Configure the timer for 32-bit mode and a tick period of 0.125 us
	TC2->COUNT32.CTRLA.reg = TC_CTRLA_MODE_COUNT32 | TC_CTRLA_PRESCALER_DIV1;
	TC2->COUNT32.CC[0].reg = 63; // Match value for a period of 0.125 us
	TC2->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP; // Stop the timer
	TC2->COUNT32.COUNT.reg = 0; // Reset the timer count to 0

	// Start the timer
	timer_start(&SAMPLE_TIMER);
	
    return 0;
}

/**************************************************************************//**
 *  Adds a new timer index and reset the timer to 0. Timer_Pump decrements until it hits 0.
 *  @param[in]  None
 *  @param[out] uint16_t    Index of the new timer
 *  @return     0 indicates success. Anything else is an error.
 ******************************************************************************/
int16_t Timer_AddTimer(uint16_t *u16Timer) {
    // Bail out with error if there are no timers left
    if (s_u16TimerCount>=TIMER_MAX_COUNT) return -1;

    // Assign current timer count as the new index;
    *u16Timer = s_u16TimerCount;
    // Increment the timer count
    s_u16TimerCount++;

    // Return success
    return 0;
}

/**************************************************************************//**
 *  Set timer value. Timer_Pump decrements until it hits 0.
 *  @param[in]  uint16_t    Index of timer to set
 *  @param[in]  uint16_t    Value (in seconds) to set
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void Timer_SetTimer(uint16_t timer, uint16_t sec) {
    s_au16Timers[timer] = sec;
}
/**************************************************************************//**
 *  Get timer value.
 *  @param[in]  uint16_t    Index of timer to retrieve
 *  @param[out] None
 *  @return     uint16_t    Current timer value (in seconds)
 ******************************************************************************/
uint16_t Timer_GetTimer(uint16_t timer) {
    return s_au16Timers[timer];
}


/**************************************************************************//**
 *  Registers the user callback function to handle timer (one second) overflow.
 *  @param[in]  void*   Pointer to user callback function
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
static struct timer_task task;
void Timer_Register_Callback(void* fpTimerCb) {
    task.interval = 1; // Timer output configured for one second
    task.cb = fpTimerCb;
    task.mode = TIMER_TASK_REPEAT;
    timer_add_task(&TIMER_0, &task);
	
	TC2->COUNT32.COUNT.reg = 0; // Reset the timer count to 0
	timer_start(&TIMER_0);
}


/**************************************************************************//**
 *  Pumps all timers. Decrements all non-zero timers. MUST be called from user callback
 *  @param[in]  None
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void Timer_Pump(void) {
    for (uint16_t index=0; index<s_u16TimerCount; index++) {
        uint16_t count = Timer_GetTimer(index);
        if (count>0) Timer_SetTimer(index, count-1);
    }
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/

