/**
 ******************************************************************************
 * @file      eeprom-board.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board Flash Write/Read implementation
 *
 ******************************************************************************
 */

/* Includes */
#include "eeprom-board.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

/**
 * @brief Writes the given buffer to the EEPROM at the specified address.
 *
 * @param[IN] addr EEPROM address to write to
 * @param[IN] buffer Pointer to the buffer to be written.
 * @param[IN] size Size of the buffer to be written.
 * @retval status [LMN_STATUS_OK, LMN_STATUS_ERROR]
 */
LmnStatus_t EepromMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size ){
  
    if ((FLASH_BASE + addr) >= FLASH_END) return LMN_STATUS_ERROR;
    if (HAL_FLASH_Unlock() == HAL_OK){
        for (int i = 0; i < size; i++){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (FLASH_BASE + addr + i), buffer[i]) != HAL_OK) break;
        }
    }

    HAL_FLASH_Lock();
    return LMN_STATUS_OK;
}

/**
 * @brief Sets the device address.
 * @remark Useful for I2C external EEPROMS
 * @param[IN] addr External EEPROM address
 */
LmnStatus_t EepromMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size ){
    
    if ((FLASH_BASE + addr + size) >= FLASH_END) return LMN_STATUS_ERROR;
    for (int i = 0; i < size; i++){
        buffer[i] = *(__IO uint8_t*)(addr + i);
    }
    return LMN_STATUS_OK;
}