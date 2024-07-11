/**
 ******************************************************************************
 * @file      watchdog.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board IWD implementation
 *
 ******************************************************************************
 */
/* includes */
#include <stdio.h>
#include <stdbool.h>
#include "main.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_iwdg.h"

#define IWDG_TIMEOUT        10    /* seconds */

IWDG_HandleTypeDef hiwdg;
/**
 * @brief Initializes Watchdog struct
 *
 */
void IWDG_Init( void ){
    hiwdg.Instance              = IWDG;
    hiwdg.Init.Prescaler        = IWDG_PRESCALER_32;
    hiwdg.Init.Prescaler        = IWDG_TIMEOUT * 999;
    if ( HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        printf("IWDG initializaion failed\n");
    }
}

/**
 * @brief Refresh IWD timer
 *
 */
bool IWDG_Referesh( void ){
    if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK){
        printf("Error Refreshing IWDG\n");
        return false;
    }
    return true;
} 