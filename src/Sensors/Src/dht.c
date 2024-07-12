/**
 ******************************************************************************
 * @file      dht.c
 * @author    Dean Prince Agbodjan
 * @brief     DHT Sensor Driver implementation
 *
 ******************************************************************************
 */

/* Include */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"

#include "dht.h"
#include "board-config.h"

/**
 * DHT Sensor type
 */
typedef enum{
    DHT11,
    DHT12,
    DHT22
}dht_types;

/**
 * DHT Sensor type definition
 */
typedef struct{
    Gpio_t *obj;
    TIM_HandleTypeDef *htim;
    dht_types dht_t;
    uint8_t temperature;
    uint8_t humidity;
} DHTTypedef_t;

/* Variables */
Gpio_t dht_GPIO_obj;
DHTTypedef_t dht_DHT11;
TIM_HandleTypeDef htim2;

/* Private functions */
static bool dhtInit( Gpio_t *obj, DHTTypedef_t *dht, dht_types dht_t,  PinNames pin );
static void TIM_2_Init( TIM_HandleTypeDef *tim );
static void TIM_2_DeInit( void );
static bool setReadDHT( DHTTypedef_t *dht );

/**
 * @brief Initializes DHT sensor
 *
 * @return bool, return the status of initialization
 */
bool DHT_Init( void )
{
    return dhtInit(&dht_GPIO_obj, &dht_DHT11, DHT11, DHT_11_PIN);
}

/**
 * @brief Process and reads DHT sensor data
 *
 * @return bool, return the status after processing and reading
 */
bool DHT_ProcessValues( void ) {
    return setReadDHT( &dht_DHT11 );
}

/**
 * @brief Get temperature value
 *
 * @return returns temp value
 */
uint8_t DHT_GetTempValue( void ){
    return dht_DHT11.temperature;
}

/**
 * @brief Get humidity value
 *
 * @return returns hum value
 */
uint8_t DHT_GetHumValue( void ){
    return dht_DHT11.humidity;
}

/**
 * @brief Initializes DHT sensor
 * @param [IN] obj pointer to Gpio_t
 * @param [IN] obj pointer to DHTTypedef_t
 * @param [IN] dht_types (refer to dht.c)
 * @param [IN] pin names (refer to board-config.h)
 * @return bool, return the status of initialization
 */
static bool dhtInit( Gpio_t *obj, DHTTypedef_t *dht, dht_types dht_t, PinNames pin )
{
    if ((obj == NULL) || (dht == NULL)) 
    {
        printf("GPIO obj and DHT obj null\n");
        return false;
    }
    /* Initialize the gpio for dht */
    GpioMcuInit(obj, pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0xFF);
    dht->obj = obj;
    dht->dht_t = dht_t;
    dht->htim = &htim2;

    /* Initialize and seup TIM 2 */
    TIM_2_Init(&htim2);

    return true;
}

/**
 * @brief read dht sensor values
 * @param [IN] pointer to dht
 * @return status of the read process
 * Reading temp and hum values from DHT 11 involves 1. Initialization 2. Response 3. Data Transmission
 * 1. Initialization
 *  Pull the pin LOW for 18 ms. (set gpio output for this).
 * 
 * 2. Response
 *  DHT 11 will pull the line(pin) LOW for 80 us and the HIGH for 80us (set gpio input for this).
 * 
 * 3. Data Transmission
 *  DHT sends 40 bits of data. Each bit begins with a low signal that last 50us 
 *  The next high logic level length decides whether the bit is "1" or a "0"
 *  Bit is "0" when high logic signal is 26 - 28us
 *  Bit is "1" when low logic signal is around 70us
 *  Data = 8 bit integral Hum data + 8 bit decimal Hum data + 8 bit integral Temp data + 8 bit decimal Temp data + 8 bit checksum
*/

static bool setReadDHT( DHTTypedef_t *dht )
{
    uint32_t rTimer1, rTimer2;
    uint8_t dBit = 0, dBits[40];
    uint8_t humValue = 0, tempValue = 0, checksum = 0, checksumValue;
    
    /* Pull the pin LOW for 18 ms. (set gpio output for this) */
    GpioMcuWrite(dht->obj, 0);
    HAL_Delay(18);
    __disable_irq();

    HAL_TIM_Base_Start(dht->htim);
    __HAL_TIM_SET_COUNTER(dht->htim, 0);

    /* DHT 11 will pull the line(pin) LOW for 80 us and the HIGH for 80us (set gpio input for this) */
    GpioMcuInit(dht->obj, dht->obj->pin, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);   
    while(GpioMcuRead(dht->obj) == 1)
    {
        if((uint32_t) __HAL_TIM_GET_COUNTER(dht->htim)> 500)
        {
            __enable_irq();
            printf("Pin is high\n");
            return false;
        }
    }
     
    /* DHT 11 pulling low for 80us */
    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while(GpioMcuRead(dht->obj) == 0)
    {
        if((uint32_t) __HAL_TIM_GET_COUNTER(dht->htim)> 500)
        {
            __enable_irq();
            return false;
        }
    }

    rTimer1 = (uint32_t) __HAL_TIM_GET_COUNTER(dht->htim);
    if (rTimer1 < 75 || rTimer1 > 88){
        __enable_irq();
        printf("DHT11 Response for low pin: %ld\n", rTimer1);
        return false;
    }
    

    /* DHT 11 pulling high for 80us */
    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while(GpioMcuRead(dht->obj) == 1)
    {
        if((uint32_t) __HAL_TIM_GET_COUNTER(dht->htim)> 500)
        {
            __enable_irq();
            return false;
        }
    }
    rTimer2 = (uint32_t) __HAL_TIM_GET_COUNTER(dht->htim);

    if (rTimer2 < 75 || rTimer2 > 88){
        __enable_irq();
        printf("DHT11 Response for high pin: %ld\n", rTimer2);
        return false;
    }
  
    /* DHT sends 40 bits of data */
    for(int i = 0; i < 40; i++){
        __HAL_TIM_SET_COUNTER(dht->htim, 0);

        /* Each bit begins with 50 us*/
        while(GpioMcuRead(dht->obj) == 0)
        {
            if((uint32_t) __HAL_TIM_GET_COUNTER(dht->htim)> 500)
            {
                __enable_irq();
                return false;
            }
        }

        /* Reading the logic length */
        __HAL_TIM_SET_COUNTER(dht->htim, 0);
        while(GpioMcuRead(dht->obj) == 1)
        {
            if((uint32_t) __HAL_TIM_GET_COUNTER(dht->htim)> 500)
            {
                __enable_irq();
                return false;
            }
        }

        rTimer1 = (uint32_t) __HAL_TIM_GET_COUNTER(dht->htim);
        if (rTimer1 > 20 && rTimer1 < 30 ) dBit = 0;
        else if (rTimer1 > 60 && rTimer1 < 80 ) dBit = 1;
        else 
        {
            return false; 
        }
        dBits[i] = dBit;
    }

    HAL_TIM_Base_Stop(dht->htim);  
    __enable_irq(); 

    /* Computing the hum and temp data */
	for(int i = 0; i < 8; i++)
	{
		humValue += dBits[i];
		humValue = humValue << 1;
	}    

	for(int i = 16; i < 24; i++)
	{
		tempValue += dBits[i];
		tempValue = tempValue << 1;
	}    

	for(int i = 32; i < 40; i++)
	{
		checksum += dBits[i];
		checksum = checksum << 1;
	}

	checksum = checksum >> 1;
	humValue = humValue >> 1;
	tempValue = tempValue >> 1;

	checksumValue = humValue + tempValue;

//	if(genParity == checksum)

	dht->temperature = tempValue;
	dht->humidity = humValue;

    return true;
}

/**
 * @brief Initialize and configure TIM2 
 * @param [IN] pointer to TIM_HandleTypeDef
 */
static void TIM_2_Init(TIM_HandleTypeDef *tim)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* Initialize the timer peripheral */
    tim->Instance = TIM2;
    tim->Init.Prescaler = 42 - 1;
    tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    tim->Init.Period = 4294967295;
    tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(tim) != HAL_OK)
    {
        printf("Timer Initialization Failed\n");
    }

    /* Selecting the timer clock source*/
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(tim, &sClockSourceConfig) != HAL_OK)
    {
        printf("Failed timer clock source selection\n");
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(tim, &sMasterConfig) != HAL_OK){
        printf("Clock Master Configuraion Synchronization Error \n");
    }
}

/**
 * @brief DeInitialize TIM2 
 *
 */
static void TIM_2_DeInit(void)
{
    /* Disable TIM 2 clk */
    __HAL_RCC_TIM2_CLK_DISABLE();
}

