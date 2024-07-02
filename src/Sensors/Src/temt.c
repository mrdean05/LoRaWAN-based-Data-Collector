#include <stdio.h>
#include <stdint.h>

#include "adc.h"
#include "adc-board.h"
#include "board-config.h"

#include "temt.h"

Adc_t adc_obj;

void Temt_Init( void ){
    AdcMcuInit(&adc_obj, ADC_PIN);
}

void Temt_Config( void ){
    AdcMcuConfig();
}

uint16_t Temt_ReadData( void ){
    return  (AdcMcuReadChannel(&adc_obj, 3));
}


