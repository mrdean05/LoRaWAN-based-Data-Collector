/**
 ******************************************************************************
 * @file      delay-board.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board Delay implementation
 *
 ******************************************************************************
 */

/* Includes */
#include <stdint.h>
#include "delay-board.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Blocking delay of "ms" milliseconds
 *
 * @param [IN] ms    delay in milliseconds
 */
void DelayMsMcu( uint32_t ms ){
   HAL_Delay(ms);
}