#ifndef __DHT_H
#define __DHT_H

/* Current support has only been made for dht11 */

#include <stdbool.h>
#include <stdint.h>
#include "gpio-board.h"


bool DHT_Init( void );
bool DHT_ProcessValues( void );
uint8_t DHT_GetTempValue( void );
uint8_t DHT_GetHumValue( void );
void test_timer( void );

#endif