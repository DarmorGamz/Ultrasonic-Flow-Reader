#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** INCLUDES ******************************************************************/
#include <compiler.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define DEBUG_STDIO_REDIRECT    0 // Adding the WINC driver breaks the stdio_redirect for some reason...


/** TYPEDEFS, STRUCTURES AND ENUMERATIONS *************************************/
// When debugging, enable and reserve 1k RAM for log
#define DEBUG_LOG           1
#define DEBUG_LOG_NUM_BYTES 1024



#define DEBUG_INFO(fmt, ...) Debug_StoreLog (fmt, ##__VA_ARGS__)



/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void Debug_InitLog(void);
void Debug_StoreLog(const char *format, ...); //__attribute__((format (gnu_printf, 1, 2)));


/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */

