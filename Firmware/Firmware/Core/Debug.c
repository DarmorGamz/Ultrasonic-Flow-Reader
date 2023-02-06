/** INCLUDES ******************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "Debug.h"

#include "driver_init.h"
#include "hpl_gclk_base.h"
#include "hpl_pm_base.h"


/** VARIABLES *****************************************************************/
static uint16_t s_u16LogBufferIndex;
static char     s_acLogBuffer[DEBUG_LOG_NUM_BYTES];


/** LOCAL (PRIVATE) STRUCTURES AND ENUMERATIONS *******************************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
/**************************************************************************//**
 *  Initialize the debug log (buffer in RAM)
 *  @param[in]  None
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void Debug_InitLog()
{
    // Clear the contents of the log
    memset(&s_acLogBuffer[0], 0, sizeof(s_acLogBuffer));
    // Reset the buffer index
    s_u16LogBufferIndex = 0;
}

/**************************************************************************//**
 *  Initialize the debug log (buffer in RAM)
 *  @param[in]  const char* format  sprintf formatter
 *  @param[in]  sprintf argument array
 *  @return     Nothing
 ******************************************************************************/
void Debug_StoreLog(const char *format, ...) {
    uint16_t bytesToWrite, offset, length;
    va_list args;
    char buffer[strlen(format)];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    // Dump to debug UART (if in debug)
#if DEBUG_STDIO_REDIRECT
    printf(buffer);
    printf("\r\n");
#endif

    bytesToWrite = strlen(buffer);
    offset = 0;

    // Write the bytes to the debug log
    while (bytesToWrite>0) {
        // Compute destination pointer in the debug log buffer
        char *pDst = &s_acLogBuffer[0] + s_u16LogBufferIndex;

        // Make sure there is room for the debug string before writing to avoid a buffer overrun
        if (DEBUG_LOG_NUM_BYTES > (s_u16LogBufferIndex + bytesToWrite)) {
            memcpy(pDst, &buffer[offset], bytesToWrite);
            s_u16LogBufferIndex += bytesToWrite;
            bytesToWrite = 0;

        // Not enough room... fill in remainder of buffer and roll over. Next time through while loop will complete
        } else {
            length = DEBUG_LOG_NUM_BYTES - s_u16LogBufferIndex;
            memcpy(pDst, &buffer[offset], length);
            s_u16LogBufferIndex = 0;
            bytesToWrite -= length;
            offset += length;
        }
    }

    // This should never happen (because of logic above)... just a sanity check
    if (s_u16LogBufferIndex >= DEBUG_LOG_NUM_BYTES) {
        s_u16LogBufferIndex = 0;
    }
}



/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/
