/**
 ******************************************************************************
 * @file      temt.c
 * @author    Dean Prince Agbodjan
 * @brief     TEMT600 Sensor Driver implementation
 *
 ******************************************************************************
 */

/* Include */
#include <stdio.h>
#include <stdint.h>

#include "adc.h"
#include "adc-board.h"
#include "board-config.h"

#include "temt.h"

Adc_t adc_obj;

/**
 * @brief Initializes TEMT600 sensor
 *
 */
void Temt_Init( void ){
    AdcMcuInit(&adc_obj, ADC_PIN);
}

/**
 * @brief Configure TEMT600 sensor
 *
 */
void Temt_Config( void ){
    AdcMcuConfig();
}

/**
 * @brief Read TEMT600 data values
 * @return sensor data
 */
uint16_t Temt_ReadData( void ){
    return  (AdcMcuReadChannel(&adc_obj, 3));
}


