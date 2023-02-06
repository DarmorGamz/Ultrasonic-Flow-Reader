/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include "Flash.h"


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
/**************************************************************************//**
 *  Initialize flash program memory access
 *  @param[in]  None
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void Flash_Init(void) {
    // Initialization of FLASH_0 already done in driver_init.c
}

/**************************************************************************//**
 *  Read contents from NVM in 64-byte (or less) pages
 *  @param[in]  uint32_t  byteAddress  Flash page to begin operation
 *  @param[out] char*   buffer      Pointer to destination buffer
 *  @param[in]  uint32_t  size        Number of bytes to read
 *  @return     Nothing
 ******************************************************************************/
void Flash_ReadFlash(uint32_t byteAddress, char *buffer, uint32_t size) {
    flash_read(&FLASH_0, byteAddress, (uint8_t*)buffer, size);
}

/**************************************************************************//**
 *  Erase contents of NVM in 256-byte rows
 *  @param[in]  uint32_t  pageOffset  Flash page to begin operation
 *  @param[in]  uint32_t  size        Number of bytes to erase. Will round up to pages!
 *  @return     Nothing
 ******************************************************************************/
void Flash_EraseFlash(uint32_t byteAddress, uint32_t numPages) {
    flash_erase(&FLASH_0, byteAddress, numPages);
}

/**************************************************************************//**
 *  Write contents to NVM in 64-byte pages.
 *    Note before writing page(s), the row(s) must be erased!
 *  @param[in]  uint32_t  pageOffset  Flash page to begin operation
 *  @param[in]  char*   buffer      Pointer to source buffer
 *  @param[in]  uint32_t  size        Number of bytes to write
 *  @return     Nothing
 ******************************************************************************/
void Flash_WriteFlash(uint32_t byteAddress, char *buffer, uint32_t size) {
    flash_write(&FLASH_0, byteAddress, (uint8_t*)buffer, size);
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/
