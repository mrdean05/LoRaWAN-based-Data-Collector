/**
 ******************************************************************************
 * @file      main.c
 * @author    Dean Prince Agbodjan
 * @brief     Contains the Application Logic
 *
 ******************************************************************************
 */

/* Includes */
#include "main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "adc.h"
#include "adc-board.h"
#include "board.h"
#include "board-config.h"
#include "config.h"
#include "delay-board.h"
#include "rtc-board.h"
#include "lorawan.h"
#include "lpm-board.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "stm32f4xx_hal_rtc_ex.h"
#include "stm32f4xx_hal_iwdg.h"

#include "cJSON.h"
#include "dht.h"
#include "temt.h"
#include "watchdog.h"

/* Private Functions */
static void app_main( void );
static void EnterLowMode();

/* variables */
static bool enterSleepMode = true;

/* OTAA settings */
const struct lorawan_otaa_settings otaa_settings = {
    .device_eui   = LORAWAN_DEVICE_EUI,
    .app_eui      = LORAWAN_APP_EUI,
    .app_key      = LORAWAN_APP_KEY,
    .channel_mask = LORAWAN_CHANNEL_MASK
};

/* variables for receiving data */
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

extern RTC_HandleTypeDef RTC_HandleStruct;

/* Main Function */
int main(void)
{
    BoardInitMcu();
    IWDG_Init();
    app_main();
}

/**
 * @brief Application Logic
 *
 * @note Initializes the sensors, registers/connects to The Things Network via OTAA,
 *       reads and uploads data in CJSON format, and then goes to sleep mode.
 *       Wakes up from sleep and send data and go to sleep mode.
 *       Implemented a watchdog to improve reliability
 */


static void app_main( void )
{
    int tempValue = 0, humValue , sunlightLevel = 0;
    int receive_length = 0;

    /* Initializing DHT 11 sensor */
    if (DHT_Init() == false)
    {
        printf("Failed to initialize DHT 11\n");
        return;
    }

    /* Initializing light sensor attached to an adc pin */
    Temt_Init();
    Temt_Config();

    printf("Initializing LoRaWAN....\n");

    if (lorawan_init_otaa(LORAWAN_REGION, &otaa_settings) < 0) {
        printf("failed!!!\n");
        return ;
    } else {
        printf("success!!!!\n");
    }
    
    /* Start the join process and wait to join the things network */
    printf("Joining the LoRaWAN network\n");
    lorawan_join();

    printf("Waiting to Join\n");

    while (!lorawan_is_joined())  
    {
        //lorawan_process();
        lorawan_process_timeout_ms(1000);
    }

    while (1)
    {
        if (enterSleepMode == true)
        {
            enterSleepMode = false;

            /* Read humidity and temperature */
            if (DHT_ProcessValues() == false)
            {
                printf("Failed to process data from DHT 11\n");
                humValue = 0;
                tempValue = 0;
            } else {
                humValue = DHT_GetHumValue();
                tempValue = DHT_GetTempValue();
            }

            /* Read sunlight level */
            sunlightLevel = Temt_ReadData();

            /* Create JSON Object */
            cJSON *dataObject = cJSON_CreateObject();

            /* Add temperature, humidity, and sunlight to the JSON object */
            cJSON_AddNumberToObject(dataObject, "Temperature", tempValue);
            cJSON_AddNumberToObject(dataObject, "Humidity", humValue);
            cJSON_AddNumberToObject(dataObject, "Sunlight", sunlightLevel);

            char *json_string = cJSON_Print(dataObject);

            /* Send cjson data to the things network*/
            printf("Sending unconfirmed data\n");
            if (lorawan_send_unconfirmed((const char*)json_string, strlen(json_string), 2) < 0)
            {
                printf("Unconfirmed sending message failed\n");
            } else {
                printf("Unconfirmed message sent\n");
            }

            /* Wait for up to 30 seconds for an event */
            if (lorawan_process_timeout_ms(30000) == 0) {

                /* check if a downlink message was received */
                receive_length = lorawan_receive(receive_buffer, sizeof(receive_buffer), &receive_port);
                if (receive_length > -1) {
                    printf("received a %d byte message on port %d: ", receive_length, receive_port);

                    for (int i = 0; i < receive_length; i++) {
                        printf("%02x", receive_buffer[i]);
                    }
                    printf("\n");
                }
            }

            /* Enter sleep mode */
            EnterLowMode();
        }
    }
}

/**
  * @brief This function handles Low Entry in Low Mode.
  */
static void EnterLowMode(){
    /* Set and Enable Interrupt Priority */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

    /* Setting up wake timer  */
    if (HAL_RTCEx_SetWakeUpTimer_IT(&RTC_HandleStruct, 20 * 1024, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
    {
        printf("Error Setting up Low Power Wakeup Timer\n");
        return;
    }

    /* Enter Low Power Mode */
    LpmEnterLowPower(); 

    /* Disable IRQ */
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
}

/* RTC Wake up Handler */
/**
  * @brief This function is a RTC Wake up handler.
  */
void RTC_WKUP_IRQHandler( void )
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_HandleStruct);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    enterSleepMode = true;
    /* Refreshes WatchDog timer */
    IWDG_Referesh();
}




