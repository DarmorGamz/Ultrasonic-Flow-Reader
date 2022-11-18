#include <atmel_start.h>
#include <sys/time.h>
#include <App/DcaApp.h>

/** VARIABLES *****************************************************************/


/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) STRUCTURES AND ENUMERATIONS *******************************/

static uint32_t   ms_tick = 0;
struct timer_task ms_tick_task;


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/
static void timer_task_cb(const struct timer_task *const t) {
	(void)t;
	ms_tick++;
}


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/


uint32_t get_ms_tick(void) {
	return ms_tick;
}

void _startTickTimer() {
	timer_stop(&TICK_TIMER);
	ms_tick_task.interval = 1;
	ms_tick_task.cb = timer_task_cb;
	ms_tick_task.mode = TIMER_TASK_REPEAT;
	timer_add_task(&TICK_TIMER, &ms_tick_task);
	timer_start(&TICK_TIMER);
}

int main(void) {
	/* Initializes MCU, drivers and middle ware */
	atmel_start_init();
	
	printf("\r\nSystem Boot\n");

	/* Start Timer Task - used by TCPIP Lite */
	printf("\r\nTick Timer Started\n");
	_startTickTimer();
	
	// Initialize the DCA specific functionality
	DcaApp_Init();

	// Main processing loop
	DcaApp_Entry();
	
	// Return Success.
	return true;
}
