/**
 ******************************************************************************
 * @file      rtc-board.c
 * @author    Dean Prince Agbodjan
 * @brief     Target board RTC implementation
 *
 ******************************************************************************
 */
/* includes */
#include <stdio.h>
#include <stdbool.h>

#include "rtc-board.h"
#include "systime.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "stm32f4xx_hal_rtc_ex.h"

/** 
 * @brief number of bits for the subseconds 
 * This is based on "RTC_HandleStruct.Init.SynchPrediv = 255" which is 2^8 - 1 = 255 ((1 << 8) - 1)
 */
#define N_PREDIV_S                                   8
#define PREDIV_S                                     255
#define PREDIV_A                                     127

#define WAKE_UP_TICK                                 3

#define DAYS_IN_LEAP_YEAR                            (( uint32_t )  366U )
#define DAYS_IN_YEAR                                 (( uint32_t )  365U )
#define SECONDS_IN_1DAY                              (( uint32_t )86400U )
#define SECONDS_IN_1HOUR                             (( uint32_t ) 3600U )
#define SECONDS_IN_1MINUTE                           (( uint32_t )   60U )
#define MINUTES_IN_1HOUR                             (( uint32_t )   60U )
#define HOURS_IN_1DAY                                (( uint32_t )   24U )

/* Sub-second mask definition */
#define ALARM_SUBSECOND_MASK                        ( N_PREDIV_S << RTC_ALRMASSR_MASKSS_Pos )

/**
 * @brief Correction factors
 */
#define  DAYS_IN_MONTH_CORRECTION_NORM               (( uint32_t )0x99AAA0 )
#define  DAYS_IN_MONTH_CORRECTION_LEAP               (( uint32_t )0x445550 )

/**
 * @brief Calculates ceiling( X / N )
 */
#define DIVC( X, N )                                 (( ( X ) + ( N ) -1 ) / ( N ) )

static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

RTC_HandleTypeDef RTC_HandleStruct;
RTC_AlarmTypeDef RTC_AlarmStruct;

RTC_TimeTypeDef RTC_TimeContext;
RTC_DateTypeDef RTC_DateContext;
uint64_t TimeTicks = 0;

// #define DEBUG_RTC

uint64_t RtcGetDateTime(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);

static bool RtcInitialized = false;

/**
 * @brief Initializes the RTC timer
 *
 * @note The timer is based on the RTC
 */
void RtcInit( void ){
    
    #ifdef DEBUG_RTC
    printf("RTC initilization\r\n");
    #endif
    
    if (RtcInitialized == false){

    __HAL_RCC_RTC_ENABLE();

    /* Initialize the RTC */
    RTC_HandleStruct.Instance = RTC;
    RTC_HandleStruct.Init.HourFormat = RTC_HOURFORMAT_24;
    RTC_HandleStruct.Init.AsynchPrediv = PREDIV_A;
    RTC_HandleStruct.Init.SynchPrediv = PREDIV_S;
    RTC_HandleStruct.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTC_HandleStruct.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTC_HandleStruct.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    // RTC_HandleStruct.Lock = HAL_UNLOCKED;
    // RTC_HandleStruct.State = HAL_RTC_STATE_RESET;

    if ( HAL_RTC_Init(&RTC_HandleStruct) != HAL_OK ) printf("Error Initializing the RTC\n");
    
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    /* Setting up Time and Date */
    RTC_TimeStruct.Hours = 0;
    RTC_TimeStruct.Minutes = 0;
    RTC_TimeStruct.Seconds = 0;
    RTC_TimeStruct.SubSeconds = 0;
    RTC_TimeStruct.TimeFormat = 0;
    RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

    if ( HAL_RTC_SetTime(&RTC_HandleStruct, &RTC_TimeStruct, RTC_FORMAT_BIN) != HAL_OK ) printf("Error Setting up RTC Time\n");
    
    RTC_DateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
    RTC_DateStruct.Month = RTC_MONTH_JANUARY;
    RTC_DateStruct.Date = 1;
    RTC_DateStruct.Year = 0;
    if ( HAL_RTC_SetDate(&RTC_HandleStruct, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK) printf("Error Setting up RTC Date \n");
    
    HAL_RTCEx_EnableBypassShadow(&RTC_HandleStruct);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    HAL_RTC_DeactivateAlarm( &RTC_HandleStruct, RTC_ALARM_A );

    #ifdef DEBUG_RTC
    printf("Setting time.........: \n");
    printf("SubSeconds: %ld\n", RTC_TimeStruct.SubSeconds);
    printf("Seconds: %d\n", RTC_TimeStruct.Seconds);
    printf("Minutes: %d\n", RTC_TimeStruct.Minutes);
    printf("Hours: %d\n", RTC_TimeStruct.Hours);

    printf("WeekDay: %d\n", RTC_DateStruct.WeekDay);
    printf("Month: %d\n", RTC_DateStruct.Month);
    printf("Date: %d\n", RTC_DateStruct.Date);
    printf("Year: %d\n", RTC_DateStruct.Year);
    #endif

    RtcSetTimerContext();
    RtcInitialized = true;
    }
}

/**
 * @brief Returns the minimum timeout value
 *
 * @return minTimeout Minimum timeout value in in ticks
 */
uint32_t RtcGetMinimumTimeout( void ){

    #ifdef DEBUG_RTC
    printf("RTC RtcGetMinimumTimeout\r\n");
    #endif

    return WAKE_UP_TICK;
}

/**
 * @brief converts time in ms to time in ticks
 *
 * @param[IN] milliseconds Time in milliseconds
 * @return returns time in timer ticks
 */
uint32_t RtcMs2Tick( TimerTime_t milliseconds ){
    /* In this case we consider the ticks as subseconds.
     * 1 tick is equivalent to PREDIV_S (which is the subseconds) thereby we multiply PREDIV_S * millisecs and divide by a 1000
     * seconds = millisecs / 1000
    */
    #ifdef DEBUG_RTC
    printf("RTC RtcMs2Tick\r\n");
    #endif
    
    return ((milliseconds * PREDIV_S) / 1000);
    //return ( uint32_t )( ( ( ( uint64_t )milliseconds ) * ( 1 << ( 10 - 3 ) ) ) / ( ( 1000000 / 1000 ) >> 3 ) );
}

/**
 * @brief converts time in ticks to time in ms
 *
 * @param[IN] time in timer ticks
 * @return returns time in milliseconds
 */
TimerTime_t RtcTick2Ms( uint32_t tick ){
   
   #ifdef DEBUG_RTC
   printf("RTC RtcTick2Ms\r\n");
   #endif
   
   uint32_t seconds = tick >> N_PREDIV_S;
   tick = tick & PREDIV_S;

   return ((seconds * 1000) + ((tick * 1000) >> N_PREDIV_S));
}

/**
 * @brief Performs a delay of milliseconds by polling RTC
 *
 * @param[IN] milliseconds Delay in ms
 */
void RtcDelayMs( TimerTime_t milliseconds ){

    #ifdef DEBUG_RTC
    printf("RTC RtcDelayMs\r\n");
    #endif
    
    /* Get the time now in ticks */
    uint32_t timeNowTicks = RtcGetTimerValue();

    /* Get the milliseconds in ticks */
    uint32_t delayTimeTicks = RtcMs2Tick(milliseconds);

    while ((RtcGetTimerValue() - timeNowTicks) < delayTimeTicks);
}

/**
 * @brief Sets the alarm
 *
 * \note The alarm is set at now (read in this funtion) + timeout
 *
 * @param timeout [IN] Duration of the Timer ticks
 */
void RtcSetAlarm( uint32_t timeout ){

    #ifdef DEBUG_RTC
    printf("RTC RtcSetAlarm\r\n");
    #endif
    RtcStartAlarm( timeout );
}

/**
 * @brief Stops the Alarm
 */
void RtcStopAlarm( void ){
    
    #ifdef DEBUG_RTC
    printf("RTC RtcStopAlarm\r\n");
    #endif
    
    HAL_RTC_DeactivateAlarm(&RTC_HandleStruct, RTC_ALARM_A);

    __HAL_RTC_ALARM_CLEAR_FLAG( &RTC_HandleStruct, RTC_FLAG_ALRAF );
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG( );

}

    
/** 
 * @brief Starts wake up alarm
 *
 * @note  Alarm in RtcTimerContext.Time + timeout
 * @param [IN] timeout Timeout value in ticks
 * In the function below, we will first calculate the future time based on the current date and time and the timeout.
 * The alarm is set by adding the timeout (int ticks) to the current data and time. 
 * timeout which is in ticks is converted to the equivalence in subseconds, seconds, mintues, hours, days, week, month
 */

void RtcStartAlarm( uint32_t timeout ){
    
    RtcStopAlarm();
    
    #ifdef DEBUG_RTC
    printf("RTC RtcStartAlarm\r\n");
    #endif

    uint16_t rtcAlarmDays = 0;
    uint16_t rtcAlarmHours = 0;
    uint16_t rtcAlarmMinutes = 0;
    uint16_t rtcAlarmSeconds = 0;
    uint16_t rtcAlarmSubseconds = 0;

    RTC_TimeTypeDef RTC_TimeNow = RTC_TimeContext;
    RTC_DateTypeDef RTC_DateNow = RTC_DateContext;

    /* The size of the subseconds is determined by the sync prescaler
     * To determine the subsecond of the timeout in ticks, timeout AND(&) sync prescaler
     * To determine the future time(timeout and current subseconds) for subsecond: 
     *  1. Substract the current sunseconds from the sync prescale.
     *  2. Add the result to the subsecond of the timeout in ticks (timeout AND(&) sync prescaler)
    */

    rtcAlarmSubseconds = PREDIV_S - RTC_TimeNow.SubSeconds;
    rtcAlarmSubseconds += (timeout & PREDIV_S);

    /* To subtract the subseconds from the timeout, right shift by N_PREDIV_S(subsecond num of bits)*/
    timeout >>= N_PREDIV_S;    // timeout = timeout >> N_PREDIV_S; 

    rtcAlarmDays = RTC_DateNow.Date;
    while (timeout >= TM_SECONDS_IN_1DAY){
        timeout -= TM_SECONDS_IN_1DAY;
        rtcAlarmDays++;
    }

    /* Calculate the hours */
    rtcAlarmHours = RTC_TimeNow.Hours;
    while (timeout >= TM_SECONDS_IN_1HOUR){
        timeout -= TM_SECONDS_IN_1HOUR;
        rtcAlarmHours++;
    }

    /* Calculate the minutes */
    rtcAlarmMinutes = RTC_TimeNow.Minutes;
    while (timeout >= TM_SECONDS_IN_1MINUTE){
        timeout -= TM_SECONDS_IN_1MINUTE;
        rtcAlarmMinutes++;
    }
    rtcAlarmSeconds = timeout + RTC_TimeNow.Seconds; // + currenttime

    /* Corrections */
    while (rtcAlarmSubseconds >= (PREDIV_S + 1)){
        rtcAlarmSubseconds -= (PREDIV_S + 1);
        rtcAlarmSeconds++;
    }

    while (rtcAlarmSeconds >= TM_SECONDS_IN_1MINUTE){
        rtcAlarmSeconds -= TM_SECONDS_IN_1MINUTE;
        rtcAlarmMinutes++;
    }

    while (rtcAlarmMinutes >= TM_MINUTES_IN_1HOUR){
        rtcAlarmMinutes -= TM_MINUTES_IN_1HOUR;
        rtcAlarmMinutes++;
    }

    while (rtcAlarmHours >= TM_HOURS_IN_1DAY){
        rtcAlarmHours -= TM_HOURS_IN_1DAY;
        rtcAlarmDays++;
    }

    
    if( RTC_DateNow.Year % 4 == 0 ) 
    {
        if( rtcAlarmDays > DaysInMonthLeapYear[RTC_DateNow.Month - 1] )
        {
            rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[RTC_DateNow.Month - 1];
        }
    }
    else
    {
        if( rtcAlarmDays > DaysInMonth[RTC_DateNow.Month - 1] )
        {   
            rtcAlarmDays = rtcAlarmDays % DaysInMonth[RTC_DateNow.Month - 1];
        }
    }

    RTC_AlarmStruct.AlarmTime.Hours = rtcAlarmHours;
    RTC_AlarmStruct.AlarmTime.Minutes = rtcAlarmMinutes;
    RTC_AlarmStruct.AlarmTime.Seconds = rtcAlarmSeconds;
    RTC_AlarmStruct.AlarmTime.SubSeconds = PREDIV_S - rtcAlarmSubseconds;
    RTC_AlarmStruct.AlarmSubSecondMask = ALARM_SUBSECOND_MASK;
    RTC_AlarmStruct.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_AlarmStruct.AlarmTime.TimeFormat = RTC_TimeNow.TimeFormat;
    RTC_AlarmStruct.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    RTC_AlarmStruct.AlarmMask = RTC_ALARMMASK_NONE;
    RTC_AlarmStruct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    RTC_AlarmStruct.AlarmDateWeekDay = (uint8_t) rtcAlarmDays;
    RTC_AlarmStruct.Alarm = RTC_ALARM_A;

    #ifdef DEBUG_RTC
    printf("Setting Alarm time.......\n");
    printf("SubSeconds: %ld\n",  RTC_AlarmStruct.AlarmTime.SubSeconds);
    printf("Seconds: %d\n", RTC_AlarmStruct.AlarmTime.Seconds);
    printf("Minutes: %d\n", RTC_AlarmStruct.AlarmTime.Minutes);
    printf("Hours: %d\n", RTC_AlarmStruct.AlarmTime.Hours);
    #endif

    if ( HAL_RTC_SetAlarm_IT(&RTC_HandleStruct, &RTC_AlarmStruct, RTC_FORMAT_BIN) != HAL_OK) printf("Error Setting up RTC Alarm \n");
}

/**
 * @brief Sets the RTC timer reference
 *
 * @return value Timer reference value in ticks
 */
uint32_t RtcSetTimerContext( void ){
    
    #ifdef DEBUG_RTC
    printf("RTC RtcSetTimerContext\r\n");
    #endif
    
    TimeTicks = RtcGetDateTime(&RTC_DateContext, &RTC_TimeContext);
    return (uint32_t)TimeTicks;
}

/**
 * @brief Sets the RTC timer reference
 *
 * @return value Timer reference value in ticks
 */
uint32_t RtcGetTimerContext( void ){

    #ifdef DEBUG_RTC
    printf("RTC RtcGetTimerContext\r\n");
    #endif
    return TimeTicks;
}

/**
 * @brief Get the RTC timer elapsed time since the last Alarm was set
 *
 * @return RTC Elapsed time since the last alarm in ticks.
 */
uint32_t RtcGetTimerElapsedTime( void ){
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    #ifdef DEBUG_RTC
    printf("RTC RtcGetTimerElapsedTime\r\n");
    #endif
    uint32_t timeInTicks = (uint32_t)RtcGetDateTime(&date, &time);
    return((uint32_t)(timeInTicks  - TimeTicks));    
}

/**
 * @brief Gets the system time with the number of seconds elapsed since epoch
 *
 * @param [OUT] milliseconds Number of milliseconds elapsed since epoch
 * @return seconds Number of seconds elapsed since epoch
 */
uint32_t RtcGetCalendarTime( uint16_t *milliseconds ){
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    
    #ifdef DEBUG_RTC
    printf("RTC RtcGetCalendarTime\r\n");
    #endif
    
    uint64_t timeInTicks = RtcGetDateTime(&date, &time);
    *milliseconds = RtcTick2Ms((timeInTicks & PREDIV_S));
    return (uint32_t)(timeInTicks >> N_PREDIV_S);
}

/**
 * @brief Get the RTC timer value
 *
 * @return RTC Timer value
 */
uint32_t RtcGetTimerValue( void ){
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    #ifdef DEBUG_RTC
    printf("RTC RtcGetTimerValue\r\n");
    #endif
    
    uint32_t timeInTicks = (uint32_t)RtcGetDateTime(&date, &time);
    return timeInTicks;
}

/**
 * @brief Writes data0 and data1 to the RTC backup registers
 *
 * @param [IN] data0 1st Data to be written
 * @param [IN] data1 2nd Data to be written
 */
void RtcBkupWrite( uint32_t data0, uint32_t data1){
    
    #ifdef DEBUG_RTC
    printf("RTC RtcBkupWrite\r\n");
    #endif
    
    HAL_RTCEx_BKUPWrite(&RTC_HandleStruct, RTC_BKP_DR0, data0);
    HAL_RTCEx_BKUPWrite(&RTC_HandleStruct, RTC_BKP_DR1, data1);
}

/**
 * @brief Reads data0 and data1 from the RTC backup registers
 *
 * @param [OUT] data0 1st Data to be read
 * @param [OUT] data1 2nd Data to be read
 */
void RtcBkupRead ( uint32_t* data0, uint32_t* data1 ){
    *data0 = HAL_RTCEx_BKUPRead(&RTC_HandleStruct, RTC_BKP_DR0);
    *data1 = HAL_RTCEx_BKUPRead(&RTC_HandleStruct, RTC_BKP_DR1);
}

/**
 * @brief Returns in ticks the present days and time since epoch
 *
 * @param [IN] date Pointer to RTC_DateStruct
 * @param [IN] time Pointer to RTC_TimeStruct
 */
uint64_t RtcGetDateTime(RTC_DateTypeDef *date, RTC_TimeTypeDef *time){

    #ifdef DEBUG_RTC
    printf("RTC RtcGetDateTime\r\n");
    #endif
    
    uint64_t calendarValue = 0;
    uint32_t correction    = 0;
    uint32_t seconds       = 0;
    uint32_t firstRead;

    do
    {
        firstRead = RTC->SSR;
        HAL_RTC_GetTime(&RTC_HandleStruct, time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate (&RTC_HandleStruct, date, RTC_FORMAT_BIN);
    } while (firstRead != RTC->SSR);
    

    
    // Calculte amount of elapsed days since 01/01/2000
    seconds = DIVC( ( DAYS_IN_YEAR * 3 + DAYS_IN_LEAP_YEAR ) * date->Year , 4 );

    correction = ( ( date->Year % 4 ) == 0 ) ? DAYS_IN_MONTH_CORRECTION_LEAP : DAYS_IN_MONTH_CORRECTION_NORM;

    seconds += ( DIVC( ( date->Month-1 ) * ( 30 + 31 ), 2 ) - ( ( ( correction >> ( ( date->Month - 1 ) * 2 ) ) & 0x03 ) ) );

    seconds += ( date->Date -1 );

    // Convert from days to seconds
    seconds *= SECONDS_IN_1DAY;

    seconds += ( ( uint32_t )time->Seconds + 
                 ( ( uint32_t )time->Minutes * SECONDS_IN_1MINUTE ) +
                 ( ( uint32_t )time->Hours * SECONDS_IN_1HOUR ) ) ;
    
    calendarValue = ((uint64_t)(seconds << N_PREDIV_S) + ( PREDIV_S - time->SubSeconds ));
    return calendarValue;
}

/**
 * @brief RTC Alarm Handler
 */
void RTC_Alarm_IRQHandler (void){
    #ifdef DEBUG_RTC
    printf("RTC RTC_Alarm_IRQHandler\r\n");
    #endif
    HAL_RTC_AlarmIRQHandler(&RTC_HandleStruct);
}

void HAL_RTC_AlarmAEventCallback( RTC_HandleTypeDef *hrtc )
{
    #ifdef DEBUG_RTC
    printf("RTC HAL_RTC_AlarmAEventCallback\r\n");
    #endif
    TimerIrqHandler( );
}
