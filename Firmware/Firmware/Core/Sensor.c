/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdlib.h>
#include "driver_init.h"
#include "Sensor.h"
#include "component/eic.h"
#include "App/DcaApp.h"

/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/
bool Sensor1_Tx = true;

#define COUNT_NUM 4

void _Sensor1_Callback(void);
void _Sensor2_Callback(void);

/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
int8_t Sensor_Init(void) {
	// Init sensor.
	Sensor1_Tx = true;
	
	uint32_t u32Sense = EIC_CONFIG_SENSE4(EIC_NMICTRL_NMISENSE_FALL_Val); // EIC_CONFIG_SENSE4_FALL_Val
    uint32_t u32Filter = 0;//EIC_CONFIG_FILTEN4;
    uint32_t u32RegOrig = EIC->CONFIG[0].reg;
    EIC->CONFIG[0].reg = u32RegOrig | u32Sense | u32Filter;
			
    // Register the ISR and enable the interrupt
    ext_irq_register(SENSOR1_IN, _Sensor1_Callback);
    ext_irq_enable(SENSOR1_IN);
			
	return 1;
}

static uint32_t Sensor1Val;
static uint32_t Sensor2Val;

void _Sensor1_Callback(void) {
	// Init vars.
	static uint32_t ValBuffer[COUNT_NUM];
	static uint32_t Now;
	static uint32_t Prev;
	static int iCount;
	
	// Get Current ticks.
	Now = _get_tick_count();
	
	// Read and Now will be equal on first iteration.
	if(Prev == NULL) {
		 Prev = Now;
		 return;
	} 
		
	// Insert Tick value into buffer.
	ValBuffer[iCount] = Now - Prev;
	
	// Update Now with the Read value. Redundant and waste of time on first iteration.
	Now = Prev;
		
	// Increase count.
	iCount++;
	
	if(iCount == 4) { // Ready to Average Buffer and reset variables. Needs to be multiple of 2 to bit shift for division.
		if(Sensor1_Tx) {
			int i = 0;
			for(;i<iCount; i++) {
				Sensor1Val+=ValBuffer[i];
			}
			Sensor1Val = Sensor1Val >> iCount; // Faster than division.
			
			// Reset vars.
			memset(ValBuffer, 0, sizeof(ValBuffer));
			Now = 0; Prev = 0; iCount = 0;
			
			// Flip Rx and Tx.
			Sensor1_Tx = false; // Needs to handle the digital high somewhere.
			gpio_set_pin_level(SENSOR1_OUT, true);
		} else {
			int i = 0;
			for(;i<iCount; i++) {
				Sensor2Val+=ValBuffer[i];
			}
			Sensor2Val = Sensor2Val >> iCount; // Faster than division.
			
			// Reset vars.
			memset(ValBuffer, 0, sizeof(ValBuffer));
			Now = 0; Prev = 0; iCount = 0;
			
			// Flip Rx and Tx.
			Sensor1_Tx = true; // Needs to handle the digital high somewhere.
			gpio_set_pin_level(SENSOR1_OUT, false);
			
			
			if(!s_u8ReadingBufferUsed) {
				ReadingQueue1_Add((Sensor2Val - Sensor1Val));
				// Clear buffers.
				Sensor1Val = 0;
				Sensor2Val = 0;
			} else {
				ReadingQueue2_Add((Sensor2Val - Sensor1Val));
			}
			
		}
	}
}

void _Sensor2_Callback(void) {
	
}

