#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif
	
/** INCLUDES ******************************************************************/
#include <compiler.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define TIMER_MAX_COUNT 64


/** TYPEDEFS, STRUCTURES AND ENUMERATIONS *************************************/


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
int16_t  Timer_Init(void);
int16_t  Timer_AddTimer(uint16_t *u16Timer);
void     Timer_SetTimer(uint16_t timer, uint16_t sec);
uint16_t Timer_GetTimer(uint16_t timer);
void     Timer_Register_Callback(void* fpTimerCb);
void     Timer_Pump(void);


#ifdef __cplusplus
}
#endif

#endif
