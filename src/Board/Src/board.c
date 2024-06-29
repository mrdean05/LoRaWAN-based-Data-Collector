#include "board.h"
#include "main.h"

#define BOARD_VERSION           1

#define ID_BASE_ADDR            0x1FFF7A10
#define ID_OFFSET_1             0x04
#define ID_OFFSET_2             0x08

#define ID_0                    ID_BASE_ADDR
#define ID_1                    ID_BASE_ADDR + ID_OFFSET_1
#define ID_2                    ID_BASE_ADDR + ID_OFFSET_2


UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_USART1_UART_Init(void);

int _write(int file, char *ptr, int len){
	HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
	return len;
}

/* Initializes the mcu. */
void BoardInitMcu( void ){

    HAL_Init();
    
    /* */
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Configure the system clock */
    SystemClock_Config();

    /* Setting up UART1 for debugging */
    MX_USART1_UART_Init();

}

/* Resets MCU */
void BoardResetMcu( void ){
    /* Restart system */
    HAL_NVIC_SystemReset();
}

/* Initializes the boards peripherals. */
void BoardInitPeriph( void ){

}

/* Deintializes some peripherals to decrease power consumption */
void BoardDeInitMcu( void ){

}

/* Manages entry into deep-sleep mode */
void BoardGetUniqueId( uint8_t *id ){
    id[0] = (*(uint32_t*) ID_0 + *(uint32_t*) ID_1) >> 24; 
    id[1] = (*(uint32_t*) ID_0 + *(uint32_t*) ID_2) >> 24;
    id[2] = (*(uint32_t*) ID_0 + *(uint32_t*) ID_0) >> 24;
    id[3] = (*(uint32_t*) ID_1 + *(uint32_t*) ID_1) >> 24;
    id[4] = (*(uint32_t*) ID_2 + *(uint32_t*) ID_2) >> 24;
    id[5] = (*(uint32_t*) ID_1 + *(uint32_t*) ID_2) >> 24;
    id[6] = (*(uint32_t*) ID_1 + *(uint32_t*) ID_2) >> 24;
    id[7] = (*(uint32_t*) ID_0 + *(uint32_t*) ID_2) >> 24;
}

/* Manages entry into deep sleep */
void BoardLowPowerHandler( void ){

}

/* Get board version */
Version_t BoardGetVersion( void ){
    Version_t boardVersion;
    boardVersion.Value = BOARD_VERSION;
    return boardVersion;
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0; //  Battery level [0: node is connected to an external power source ...
}

uint32_t BoardGetRandomSeed( void )
{
    return 0;
}


void BoardCriticalSectionBegin( uint32_t *mask ){
    /* Set the current state of interrupt from PRIMASK register */
    *mask = __get_PRIMASK();

    /* disable interrupt */
    __disable_irq();
}

void BoardCriticalSectionEnd( uint32_t *mask ){
    __set_PRIMASK(*mask);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 84;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

    PeriphClkStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkStruct);
}

static void MX_USART1_UART_Init( void )
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

void HAL_UART_MspDeInit( UART_HandleTypeDef* huart )
{
  if(huart->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
  }
}


