/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdlib.h>
#include "driver_init.h"
#include "Sensor.h"
#include "component/eic.h"
#include "App/DcaApp.h"

/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/
bool Sensor1_Tx = true;

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

void _Sensor1_Callback(void) {
	if(Sensor1_Tx) {
		_AddToFifo_EventData(0);
	} else {
		_AddToFifo_EventData(1);
	}
	
	// FLip sensor.
	Sensor1_Tx = !Sensor1_Tx;
}

void _Sensor2_Callback(void) {
	
}

