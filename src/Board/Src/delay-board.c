#include <stdint.h>
#include "delay-board.h"
#include "stm32f4xx_hal.h"

/* delay in milliseconds */
void DelayMsMcu( uint32_t ms ){
   HAL_Delay(ms);
}