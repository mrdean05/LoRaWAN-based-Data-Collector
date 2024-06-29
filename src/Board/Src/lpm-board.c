#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "lpm-board.h"


static LpmSetMode_t setMode = LPM_DISABLE;
static LpmGetMode_t getMode = LPM_SLEEP_MODE;

LpmGetMode_t LpmGetMode( void ){
    if(setMode == LPM_ENABLE) return getMode;
    return LPM_SLEEP_MODE;
}

void LpmSetStopMode( LpmId_t id, LpmSetMode_t mode ){
    setMode = LPM_ENABLE;
    getMode = LPM_STOP_MODE;
}

void LpmSetOffMode(LpmId_t id, LpmSetMode_t mode ){
    setMode = LPM_ENABLE;
    getMode = LPM_OFF_MODE;    
}

void LpmEnterLowPower( void ){
    if (getMode == LPM_STOP_MODE) 
    {
        LpmEnterStopMode();
        LpmExitStopMode();
    }
    else if (getMode == LPM_OFF_MODE) 
    {
        LpmEnterOffMode();
        LpmExitOffMode();

    }
    else {
        LpmEnterSleepMode();
        LpmExitSleepMode();
    }
}

void LpmEnterSleepMode( void ){
    /* Suspends the system tick */
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

}

void LpmExitSleepMode( void ){
    /* Resume the system tick */
    HAL_ResumeTick();
}

void LpmEnterStopMode( void ){
    /* Suspends the system clock */
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void LpmExitStopMode( void ){
    /* Resume the system tick */
    HAL_ResumeTick();
}

void LpmEnterOffMode( void ){
    HAL_PWR_EnterSTANDBYMode();
}

__weak void LpmExitOffMode( void ){

}