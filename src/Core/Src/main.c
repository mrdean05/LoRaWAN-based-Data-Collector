/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "config.h"
#include "delay-board.h"
#include "rtc-board.h"
#include "lorawan.h"

#include "dht.h"

static void lora_task( void );
static void test_dht11 ( void );

// OTAA settings
const struct lorawan_otaa_settings otaa_settings = {
    .device_eui   = LORAWAN_DEVICE_EUI,
    .app_eui      = LORAWAN_APP_EUI,
    .app_key      = LORAWAN_APP_KEY,
    .channel_mask = LORAWAN_CHANNEL_MASK
};

// variables for receiving data
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

int main(void)
{
    BoardInitMcu();
    //test_dht11 ();
    //test_timer();
    printf("Task About to start\r\n");
    lora_task();
}

static void test_dht11 ( void )
{
    if (DHT_Init() == false)
    {
        printf("Failed to initialize DHT 11\n");
        return;
    }
    printf("Done initialization\n");

    while(1){
        if (DHT_ProcessValues() == false)
        {
            printf("Failed to process data from DHT 11\n");
            return;
        }

        printf("The temperature value is %d\n", DHT_GetTempValue());
        printf("The humidity value is %d\n", DHT_GetHumValue());

        HAL_Delay(5000);
    }
    
}

static void lora_task( void )
{
    printf("LoRaWAN - Hello OTAA\n");

    printf("Initializing LoRaWAN....\n");

    if (lorawan_init_otaa(LORAWAN_REGION, &otaa_settings) < 0) {
        printf("failed!!!\n");
        return ;
    } else {
        printf("success!!!!\n");
    }
    
    //Start the join process and wait
    printf("Joining the LoRaWAN network\n");
    lorawan_join();

    printf("Waiting to Join\n");

    while (!lorawan_is_joined())  
    {
        lorawan_process();
    }

    uint32_t timeNowTicks = RtcGetTimerValue();
    uint32_t timeoutTicks = RtcMs2Tick(5000);

    while (1)
    {
        // let the lorwan library process pending events
        lorawan_process();

        if ((RtcGetTimerValue() - timeNowTicks) > timeoutTicks) {
            const char* message = "hello world!";

            // try to send an unconfirmed uplink message
            printf("sending unconfirmed message '%s' ... ", message);
            if (lorawan_send_unconfirmed(message, strlen(message), 2) < 0) {
                printf("failed!!!\n");
            } else {
                printf("success!\n");
            }
            timeNowTicks = RtcGetTimerValue();
        }

        // check if a downlink message was received
        receive_length = lorawan_receive(receive_buffer, sizeof(receive_buffer), &receive_port);
        if (receive_length > -1) {
            printf("received a %d byte message on port %d: ", receive_length, receive_port);

            for (int i = 0; i < receive_length; i++) {
                printf("%02x", receive_buffer[i]);
            }
            printf("\n");
        }
    }
}


Adc_t adc_obj;

void StartSensorReading(void *argument){
    
    AdcMcuInit(&adc_obj, ADC_PIN);
    AdcMcuConfig();

    uint32_t value = 0;

    while (1)
    {   
        value = AdcMcuReadChannel(&adc_obj, 3);
        printf("The value of ADC: %ld\n", value);
    }
}

/* 
int main(void)
{
    BoardInitMcu();

    RtcInit();
    RtcStartAlarm(15300);

    while(1){

    }
}
*/

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
