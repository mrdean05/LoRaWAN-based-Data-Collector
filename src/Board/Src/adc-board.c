/**
 ******************************************************************************
 * @file      adc-board.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board ADC driver implementation
 *
 ******************************************************************************
 */

/* Includes */
#include <stdio.h>
#include <stdint.h>

#include "adc-board.h"
#include "gpio-board.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"

/* ADC Handler definition */
ADC_HandleTypeDef hadc1;

/**
 * @brief Initializes the ADC object 
 *
 * @param [IN] obj      ADC object
 * @param [IN] adcInput ADC input pin
 */
void AdcMcuInit( Adc_t *obj, PinNames adcInput ){
    GpioMcuInit(&(obj->AdcInput), adcInput, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}

/**
 * @brief Initializes the ADC internal parameters
 */
void AdcMcuConfig( void )
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance                      = ADC1;
    hadc1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution               = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode             = DISABLE;
    hadc1.Init.ContinuousConvMode       = DISABLE;
    hadc1.Init.DiscontinuousConvMode    = DISABLE;
    hadc1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv         = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign                = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion          = 1;
    hadc1.Init.DMAContinuousRequests    = DISABLE;
    hadc1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        printf("ADC Initialization Error\n");
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        printf("Error configuring the Channel 3\n");
        return 0;
    }
    
}

/**
 * @brief Reads the value of the given channel
 *
 * @param [IN] obj     ADC object
 * @param [IN] channel ADC input channel
 * @retval ADC value
 */
uint16_t AdcMcuReadChannel( Adc_t *obj, uint32_t channel ){
    uint32_t adcValue = 0;
    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc1);  
    }
    else
    {
        printf("ADC Poll for conversion failed\r\n");
    }
    return adcValue;
}