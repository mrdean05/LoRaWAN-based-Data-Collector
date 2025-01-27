#ifndef __BOARD_CONFIG_H_
#define __BOARD_CONFIG_H_

#include "pinName-board.h"

#define BOARD_TCXO_WAKEUP_TIME                      5

/* sx162 LoRa Module Pins */

#define LORA_SCK         PA_5
#define LORA_MISO        PA_6
#define LORA_MOSI        PA_7

#define RADIO_DIO_1      PB_0
#define RADIO_BUSY       PB_1
#define RADIO_NSS        PB_2
#define RADIO_RESET      PB_10

/* DHT Pin */
#define DHT_11_PIN       PB_3

/* ADC Pin */
#define ADC_PIN          PA_3

#endif