#include <stdio.h>
#include <stdint.h>

#include "gpio-board.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"

static uint16_t GpioPinIndex (PinNames pin);
static Gpio_t *GpioIrq[16];

/* Initializes the given GPIO */
void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value ){
    obj->pin = pin;
    
    if (obj->pin == NC) return;

    GPIO_InitTypeDef GPIO_ConfigStruct;

    if (obj->pin < IOE_0){
        if ((obj->pin >= PA_0) && (obj->pin <= PA_15)){
            __HAL_RCC_GPIOA_CLK_ENABLE();
            obj->port = GPIOA;
            // printf("GPIOA: | ");
        }
        else if ((obj->pin >= PB_0) && (obj->pin <= PB_15)){
            __HAL_RCC_GPIOB_CLK_ENABLE();
            obj->port = GPIOB;
            // printf("GPIOB: | ");
        }
        else if ((obj->pin >= PC_0) && (obj->pin <= PC_15)){
            __HAL_RCC_GPIOC_CLK_ENABLE();
            obj->port = GPIOC;
            // printf("GPIOC: | ");
        }
        else if ((obj->pin >= PD_0) && (obj->pin <= PD_9)){
            __HAL_RCC_GPIOD_CLK_ENABLE();
            obj->port = GPIOD;
            // printf("GPIOD: | ");
        }
        else if ((obj->pin >= PF_0) && (obj->pin <= PF_2)){
            __HAL_RCC_GPIOD_CLK_ENABLE();
            obj->port = GPIOE;
            // printf("GPIOE: |");
        }
        else {
            return;
        }
    }

    obj->pinIndex = GpioPinIndex(pin);
    obj->pull = type;

    GPIO_ConfigStruct.Pin = obj->pinIndex;
    GPIO_ConfigStruct.Pull = obj->pull;
    

    if (mode == PIN_INPUT) GPIO_ConfigStruct.Mode = GPIO_MODE_INPUT;    
    else if (mode == PIN_OUTPUT) {
        if (config == PIN_PUSH_PULL) GPIO_ConfigStruct.Mode = GPIO_MODE_OUTPUT_PP;
        else { GPIO_ConfigStruct.Mode = GPIO_MODE_OUTPUT_OD; }
    }
    else if (mode == PIN_ALTERNATE_FCT){
        if (config == PIN_PUSH_PULL) GPIO_ConfigStruct.Mode = GPIO_MODE_AF_PP;
        else { GPIO_ConfigStruct.Mode = GPIO_MODE_AF_OD; }
        GPIO_ConfigStruct.Alternate = value; 
    }

    else if (mode == PIN_ANALOGIC){
        GPIO_ConfigStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(obj->port, &GPIO_ConfigStruct);
        return;
    }

    else {
        if (config == PIN_PUSH_PULL) GPIO_ConfigStruct.Mode = GPIO_MODE_OUTPUT_PP;
        else { GPIO_ConfigStruct.Mode = GPIO_MODE_OUTPUT_OD; }        
    }

    GPIO_ConfigStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(obj->port, &GPIO_ConfigStruct);

    if ((mode == PIN_OUTPUT) && ((value == 0) || (value == 1))) HAL_GPIO_WritePin(obj->port, obj->pinIndex, value);   
}

/* Sets a user defined object pointer */
void GpioMcuSetContext( Gpio_t *obj, void* context ){
    obj->Context = context;
}

/* GPIO IRQ Initialization */
void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler ){

    if (obj->pin == NC) return;

    if (irqHandler == NULL) return;

    obj->IrqHandler = irqHandler;
    GPIO_InitTypeDef GPIO_ConfigStruct;

    if (obj->pin < IOE_0){

        GPIO_ConfigStruct.Pin = obj->pinIndex;

        if (irqMode == IRQ_RISING_EDGE){
            GPIO_ConfigStruct.Mode = GPIO_MODE_IT_RISING;
        }
        else if (irqMode == IRQ_FALLING_EDGE){
            GPIO_ConfigStruct.Mode = GPIO_MODE_IT_FALLING;
        }
        else if (irqMode == IRQ_RISING_FALLING_EDGE){
            GPIO_ConfigStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
        }

        GPIO_ConfigStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(obj->port, &GPIO_ConfigStruct);

        IRQn_Type IRQn;

        switch (obj->pinIndex){

            case GPIO_PIN_0:
                IRQn = EXTI0_IRQn;
                break;
            case GPIO_PIN_1:
                IRQn = EXTI1_IRQn;
                break;
            case GPIO_PIN_2:
                IRQn = EXTI2_IRQn;
                break;
            case GPIO_PIN_3:
                IRQn = EXTI3_IRQn;
                break;            
            case GPIO_PIN_4:
                IRQn = EXTI4_IRQn;
                break;
            case GPIO_PIN_5:
            case GPIO_PIN_6:
            case GPIO_PIN_7:
            case GPIO_PIN_8:
            case GPIO_PIN_9:
                IRQn = EXTI9_5_IRQn;
                break;
            case GPIO_PIN_10:
            case GPIO_PIN_11:
            case GPIO_PIN_12:
            case GPIO_PIN_13:
            case GPIO_PIN_14:
            case GPIO_PIN_15:
                IRQn =  EXTI15_10_IRQn;
                break;
        }

        uint32_t priority;

        switch (irqPriority){
            case IRQ_VERY_HIGH_PRIORITY:
                priority = 0;
                break;
            case IRQ_HIGH_PRIORITY:
                priority = 1;
                break;
            case IRQ_MEDIUM_PRIORITY:
                priority = 2;
                break;
            case IRQ_LOW_PRIORITY:
            case IRQ_VERY_LOW_PRIORITY:
                priority = 3;
                break;
        }

        HAL_NVIC_SetPriority (IRQn, priority, 0x00);
        HAL_NVIC_EnableIRQ(IRQn);

        GpioIrq[( obj->pin & 0x0F)] = obj;
        printf("GPIO GpioMcuSetInterrupt successful priority: %ld\r\n", priority);
    }
}

/* Removes the interrupt from the object */
void GpioMcuRemoveInterrupt( Gpio_t *obj ){
    if(obj->pin > IOE_0) return;
    obj->IrqHandler = NULL;
    GPIO_InitTypeDef GPIO_ConfigStruct = {0};
    GPIO_ConfigStruct.Pin = obj->pinIndex;
    GPIO_ConfigStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(obj->port, &GPIO_ConfigStruct);
}

/* Writes the given value to the GPIO output. */
void GpioMcuWrite( Gpio_t *obj, uint32_t value ){
    HAL_GPIO_WritePin(obj->port, obj->pinIndex, value);
}

/* Toggle the value to the GPIO output */
void GpioMcuToggle( Gpio_t *obj ){
    HAL_GPIO_TogglePin (obj->port, obj->pinIndex);
}

/* Reads the current GPIO input value */
uint32_t GpioMcuRead( Gpio_t *obj ){
    return (uint32_t)HAL_GPIO_ReadPin(obj->port, obj->pinIndex);
}

static uint16_t GpioPinIndex (PinNames pin){
    uint16_t pinIndex = (pin & 0x0F);
    return (1 << pinIndex);
}

void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    int calback_index;
    while (GPIO_Pin != 0x00){
        GPIO_Pin = GPIO_Pin >> 1;
        calback_index++;
    }

    if ((GpioIrq[calback_index] != NULL)&&(GpioIrq[calback_index]->IrqHandler != NULL)){
        GpioIrq[calback_index]->IrqHandler(GpioIrq[calback_index]->Context);
    }

}