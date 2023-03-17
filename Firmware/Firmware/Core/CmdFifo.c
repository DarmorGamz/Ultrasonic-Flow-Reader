/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdint.h>
#include "App/DcaApp.h"

/** VARIABLES *****************************************************************/
static uint16_t   s_u16FifoWriteIndex;
static uint16_t   s_u16FifoReadIndex;
static uint16_t   s_u16FifoByteCount;

void CmdFifo_Init(void) {
	// Clear the contents of the command FIFO
	memset(&g_acCmdFifo[0], 0, COMMAND_FIFO_NUM_BYTES);
	// Reset the buffer read/write indexes and byte count
	s_u16FifoWriteIndex = 0;
	s_u16FifoReadIndex = 0;
	s_u16FifoByteCount = 0;
}

uint16_t CmdFifo_GetByteCount(void) {
	return s_u16FifoByteCount;
}

uint16_t CmdFifo_GetByteRemaining(void) {
    int remainig = COMMAND_FIFO_NUM_BYTES - s_u16FifoByteCount;
    return remainig;
}

bool CmdFifo_Push(char *pCmd, uint16_t length) {
    uint16_t bytesToWrite, trimmed;

    // Write the bytes to the command FIFO
    bytesToWrite = length;
    char *pDst = &g_acCmdFifo[0] + s_u16FifoByteCount;
    memcpy(pDst, pCmd, bytesToWrite);
    s_u16FifoByteCount += bytesToWrite;
    s_u16FifoWriteIndex += bytesToWrite;
    bytesToWrite = 0;

    return true;
}

uint16_t CmdFifo_Send(char *pDst, uint16_t maxBytes) {
    uint16_t bytesToSend;
    uint16_t bytesSent = 0;

    bytesToSend = (uint16_t)s_u16FifoByteCount;

    while (bytesToSend>0) {
        // Compute source pointer within the command FIFO
        char *pSrc = &g_acCmdFifo[0];

        memcpy(pDst, pSrc, bytesToSend);
        // Increment bytes sent and decrement bytes in FIFO
        bytesSent += bytesToSend;
        s_u16FifoByteCount -= bytesToSend;
        // Adjust read index for next time
        s_u16FifoReadIndex += bytesToSend;
        // Increment destination pointer by number of bytes sent
        pDst += bytesToSend;
        bytesToSend = 0;
    };

    return bytesSent;
}