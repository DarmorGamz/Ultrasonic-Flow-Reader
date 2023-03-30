/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "App/DcaApp.h"

/** VARIABLES *****************************************************************/
static uint16_t   s_u16Queue1Count;
static uint16_t   s_u16Queue2Count;

/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) STRUCTURES AND ENUMERATIONS *******************************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
void ReadingQueue_Init(void) {
	// Clear the contents of the reading queue
	memset(&g_acReadingBuffer1[0], 0, READING_BUFF_NUM_BYTES);
	memset(&g_acReadingBuffer2[0], 0, READING_BUFF_NUM_BYTES);
	
	// Clear Queue count
	s_u16Queue1Count = 0;
	s_u16Queue2Count = 0;
}

uint16_t ReadingQueue1_GetReadingCount(void) {
	return s_u16Queue1Count;
}

uint16_t ReadingQueue2_GetReadingCount(void) {
	return s_u16Queue2Count;
}

void ReadingQueue1_GetReadingRemaining(void) {
	uint16_t remaining = READING_BUFF_NUM_BYTES - s_u16Queue1Count;
	return remaining;
}

void ReadingQueue2_GetReadingRemaining(void) {
	uint16_t remaining = READING_BUFF_NUM_BYTES - s_u16Queue2Count;
	return remaining;
}

bool ReadingQueue1_Add(uint16_t val) {
	// Bail out if there isn't room for readings
	if (s_u16Queue1Count >= READING_BUFF_NUM_BYTES) return false;
	
	g_acReadingBuffer1[s_u16Queue1Count] = val;
	
	// Increment Queue Count.
	s_u16Queue1Count++;
	
	 DEBUG_INFO("Queue1 Item Added: %luus ", (val*0.125));
	// Return Success.
	return true;
}

bool ReadingQueue2_Add(uint16_t val) {
	// Bail out if there isn't room for readings
	if (s_u16Queue2Count >= READING_BUFF_NUM_BYTES) return false;
	
	g_acReadingBuffer2[s_u16Queue2Count] = val;
	
	// Increment Queue Count.
	s_u16Queue2Count++;
	
	uint16_t temp = val*0.125;
	 DEBUG_INFO("Queue2 Item Added: %luus ", temp);
	 
	// Return Success.
	return true;
}