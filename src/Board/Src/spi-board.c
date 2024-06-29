#include <stdio.h>
#include <stdbool.h>
#include "spi-board.h"
#include "gpio-board.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

SPI_HandleTypeDef hspi;

#define SPI_DATASIZE                    8      /* 8 bits or 16 bits */
#define SPI_CPOL                        0      
#define SPI_CPHA                        0
#define SPI_MODE                        0      /* 0 - Slave, 1 - Master */ 
#define SPI_PRESCALER_BAUDRATE          SPI_BAUDRATEPRESCALER_16

void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss ){

    obj->SpiId = spiId;
    
    if (spiId == SPI_1){
        /* Initialize and set all gpio of the spi as alternate functions */
        hspi.Instance = SPI1;
        __HAL_RCC_SPI1_CLK_ENABLE();

        GpioMcuInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
        GpioMcuInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
        GpioMcuInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
    }
    else if (spiId == SPI_2){
        hspi.Instance = SPI2;
        __HAL_RCC_SPI2_CLK_ENABLE();

        GpioMcuInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
        GpioMcuInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
        GpioMcuInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_NO_PULL, GPIO_AF5_SPI1);
    }

    //GpioMcuInit(&obj->Nss, nss, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1); 

    SpiFormat(obj, SPI_DATASIZE, SPI_CPOL, SPI_CPHA, SPI_MODE);

    hspi.Init.Direction = SPI_DIRECTION_2LINES;
    hspi.Init.BaudRatePrescaler = SPI_PRESCALER_BAUDRATE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial = 10;

    HAL_SPI_Init(&hspi);
}

void SpiDeInit( Spi_t *obj ){
    
    if (obj->SpiId == SPI_1) __HAL_RCC_SPI1_CLK_DISABLE();
    else __HAL_RCC_SPI2_CLK_DISABLE();

    HAL_SPI_DeInit(&hspi);
}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave ){

    if (slave == 1) hspi.Init.Mode = SPI_MODE_SLAVE;
    else if (slave == 0) hspi.Init.Mode = SPI_MODE_MASTER;
    else return;

    if (bits == 8) hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    else hspi.Init.DataSize = SPI_DATASIZE_16BIT;

    if (cpol == 0) hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    else hspi.Init.CLKPhase = SPI_POLARITY_HIGH;

    if (cpha == 0) hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    else hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
}

/*
void SpiFrequency( Spi_t *obj, uint32_t hz ){
    uint32_t sysClk = SystemCoreClock;
    uint32_t baudRate, divisor;
    int prescaler = 0;
    while (sysClk > hz){
        prescaler++;
        sysClk = sysClk >> 1;

        if (prescaler >= 7) break;
    }

    baudRate = (( ( divisor & 0x4 ) == 0 ) ? 0x0 : SPI_CR1_BR_2 ) |
               (( ( divisor & 0x2 ) == 0 ) ? 0x0 : SPI_CR1_BR_1 ) |
               (( ( divisor & 0x1 ) == 0 ) ? 0x0 : SPI_CR1_BR_0 );
    hspi.Init.BaudRatePrescaler = baudRate;
}
*/

uint16_t SpiInOut( Spi_t *obj, uint16_t outData ){
    uint16_t recvData;

    __HAL_SPI_ENABLE( &hspi );
    while(__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXE) == RESET){};
    hspi.Instance->DR = (uint32_t)(outData & 0xFF);
    
    while(__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_RXNE) == RESET){};
     recvData = hspi.Instance->DR;
    //printf("Data Out: %d\t Data In: %d\r\n", outData, recvData);

    return recvData;
}

