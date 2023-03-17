/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdint.h>
#include "App/DcaApp.h"

/** VARIABLES *****************************************************************/
static uint16_t   s_u16ElementRamSizeBytes;

void DataFifo_Init(void) {
	// Clear the contents of the Data FIFO
	memset(&g_acReadingBuffer[0], 0, READING_BUFF_NUM_BYTES);
	// Reset the buffer read/write indexes and byte count
	s_u16ElementRamSizeBytes = 0;
}

uint16_t DataFifo_GetByteCount(void) {
	return s_u16ElementRamSizeBytes;
}

uint16_t DataFifo_GetByteRemaining(void) {
	int remainig = READING_BUFF_NUM_BYTES - s_u16ElementRamSizeBytes;
	return remainig;
}

bool DataFifo_Push(char *pCmd, uint16_t length) {
	uint16_t bytesToWrite, trimmed;

	// Write the bytes to the command FIFO
	bytesToWrite = length;
	char *pDst = &g_acReadingBuffer[0] + s_u16ElementRamSizeBytes;
	memcpy(pDst, pCmd, bytesToWrite);
	s_u16ElementRamSizeBytes += bytesToWrite;
	bytesToWrite = 0;

	return true;
}

uint16_t DataFifo_Send(char *pDst, uint16_t maxBytes) {
	uint16_t bytesToSend;
	uint16_t bytesSent = 0;

	bytesToSend = (uint16_t)s_u16ElementRamSizeBytes;

	while (bytesToSend>0) {
		// Compute source pointer within the command FIFO
		char *pSrc = &g_acReadingBuffer[0];

		memcpy(pDst, pSrc, bytesToSend);
		// Increment bytes sent and decrement bytes in FIFO
		bytesSent += bytesToSend;
		s_u16ElementRamSizeBytes -= bytesToSend;
		// Adjust read index for next time
		s_u16ElementRamSizeBytes += bytesToSend;
		// Increment destination pointer by number of bytes sent
		pDst += bytesToSend;
		bytesToSend = 0;
	};

	return bytesSent;
}