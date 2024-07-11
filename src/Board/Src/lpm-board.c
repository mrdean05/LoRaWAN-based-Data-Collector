/**
 ******************************************************************************
 * @file      lpm-board.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board Low Power Mode implementation
 *
 ******************************************************************************
 */
/* includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "lpm-board.h"

/* variables */
static LpmSetMode_t setMode = LPM_DISABLE;
static LpmGetMode_t getMode = LPM_SLEEP_MODE;

/**
 * @brief  This API returns the Low Power Mode selected that will be applied when the system will enter low power mode
 *         if there is no update between the time the mode is read with this API and the time the system enters
 *         low power mode.
 *
 * @return mode Selected low power mode
 */
LpmGetMode_t LpmGetMode( void ){
    if(setMode == LPM_ENABLE) return getMode;
    return LPM_SLEEP_MODE;
}

/**
 * @brief  This API notifies the low power manager if the specified user allows the Stop mode or not.
 *         When the application does not require the system clock, it enters Stop Mode if at least one user disallow
 *         Off Mode. Otherwise, it enters Off Mode.
 *         The default mode selection for all users is Off mode enabled
 *
 * @param [IN] id   Process Id
 * @param [IN] mode Selected mode
 */
void LpmSetStopMode( LpmId_t id, LpmSetMode_t mode ){
    setMode = LPM_ENABLE;
    getMode = LPM_STOP_MODE;
}

/**
 * @brief  This API notifies the low power manager if the specified user allows the Off mode or not.
 *         When the application does not require the system clock, it enters Stop Mode if at least one user disallow
 *         Off Mode. Otherwise, it enters Off Mode.
 *         The default mode selection for all users is Off mode enabled
 *
 * @param [IN] id   Process Id
 * @param [IN] mode Selected mode
 */
void LpmSetOffMode(LpmId_t id, LpmSetMode_t mode ){
    setMode = LPM_ENABLE;
    getMode = LPM_OFF_MODE;    
}

/**
 * @brief  This API shall be used by the application when there is no more code to execute so that the system may
 *         enter low-power mode. The mode selected depends on the information received from LpmOffModeSelection( ) and
 *         LpmSysclockRequest( )
 *         This function shall be called in critical section
 */
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

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Sleep Mode
 */
void LpmEnterSleepMode( void ){
    /* Suspends the system tick */
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

}

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Sleep Mode
 */
void LpmExitSleepMode( void ){
    /* Resume the system tick */
    HAL_ResumeTick();
}

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Stop Mode
 */
void LpmEnterStopMode( void ){
    /* Suspends the system clock */
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Stop Mode. This is where the application
 *         should reconfigure the clock tree when needed
 */
void LpmExitStopMode( void ){
    /* Resume the system tick */
    HAL_ResumeTick();
}

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Off mode. This is where the application could save
 *         data in the retention memory as the RAM memory content will be lost
 */
void LpmEnterOffMode( void ){
    HAL_PWR_EnterSTANDBYMode();
}
/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Off mode. This can only happen when the
 *         Off mode is finally not entered. In that case, the application may reverse some configurations done before
 *         entering Off mode. When Off mode is successful, the system is reset when getting out from this low-power mode
 */
__weak void LpmExitOffMode( void ){

}