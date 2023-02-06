#ifndef FLASH_H_
#define FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif


/** INCLUDES ******************************************************************/
#include <compiler.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/


/** TYPEDEFS, STRUCTURES AND ENUMERATIONS *************************************/


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
void Flash_Init(void);

void Flash_ReadFlash(uint32_t byteAddr, char *buffer, uint32_t size);
void Flash_EraseFlash(uint32_t byteAddr, uint32_t numPages);
void Flash_WriteFlash(uint32_t byteAddr, char *buffer, uint32_t size);


/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* FLASH_H_ */