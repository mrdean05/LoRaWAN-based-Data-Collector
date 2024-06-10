#include "eeprom-board.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"


LmnStatus_t EepromMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size ){
    
    //printf("EEPROM EepromMcuWriteBuffer\r\n");
    
    
    
    // if ((FLASH_BASE + addr + size) >= FLASH_END) return LMN_STATUS_ERROR;
    
    if ((FLASH_BASE + addr) >= FLASH_END) return LMN_STATUS_ERROR;
    if (HAL_FLASH_Unlock() == HAL_OK){
        for (int i = 0; i < size; i++){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (FLASH_BASE + addr + i), buffer[i]) != HAL_OK) break;
        }
    }

    HAL_FLASH_Lock();
    return LMN_STATUS_OK;
}


LmnStatus_t EepromMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size ){
    
    //printf("EEPROM EepromMcuReadBuffer\r\n");
    
    if ((FLASH_BASE + addr + size) >= FLASH_END) return LMN_STATUS_ERROR;
    for (int i = 0; i < size; i++){
        buffer[i] = *(__IO uint8_t*)(addr + i);
    }
    return LMN_STATUS_OK;
}