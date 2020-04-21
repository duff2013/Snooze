/***********************************************************************************
 *  SnoozeAlarm.h
 *  Teensy 3.2
 *
 *  Purpose: RTC Driver
 *
 **********************************************************************************/
#if defined(__MK20DX256__)
#include "SnoozeAlarm.h"
//#include "TimeLib.h"

#define LLWU_RTCA_MOD       0x02

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_modules_mask( uint8_t module );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif
/*******************************************************************************
 *  <#Description#>
 *
 *  @return current time
 *******************************************************************************/
time_t rtc_set_sync_provider( void ) {
    return Teensy3Clock.get( );
}

/*******************************************************************************
 *  <#Description#> Sets an alarm for a specific amount of time once the driver
 *                  is enabled. Is dependent of when the driver is enabled.
 *
 *  @param hours   0 - 24
 *  @param minutes 0 - 60
 *  @param seconds 0 - 60
 *******************************************************************************/
void SnoozeAlarm::setRtcTimer( uint8_t hours, uint8_t minutes, uint8_t seconds ) {
    isUsed = true;
    alarm = ( hours * 3600 ) + ( minutes * 60 ) + seconds;
    timer_ = true;
}

/*******************************************************************************
 *  <#Description#> Override of original setAlarm function. Works more like a
 *                  standard alarm. Is independent of when the driver is enabled.
 *                  (So long as it is enabled before the alarmTime)
 *
 *  @param alarmTime <#time_t number of specific date & time to set alarm#>
 *******************************************************************************/
void SnoozeAlarm::setAlarm( time_t alarmTime ){
    isUsed = true;
    alarm = alarmTime;
    timer_ = false;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::disableDriver( uint8_t mode ) {
    if (mode == 0) return;
    if ( mode == 1 ) {
        if ( return_isr_enabled == 0 )  NVIC_DISABLE_IRQ( IRQ_RTC_ALARM ); //disable irq
        NVIC_SET_PRIORITY( IRQ_RTC_ALARM, return_priority );// return priority
        __disable_irq( );
        attachInterruptVector( IRQ_RTC_ALARM, return_rtc_irq );// return prev interrupt
        __enable_irq( );
    }
    RTC_IER = IER;
    //setSyncProvider( rtc_set_sync_provider );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::enableDriver( uint8_t mode ) {
    if (mode == 0) return;
    if ( mode == 1 ) {
        return_priority = NVIC_GET_PRIORITY( IRQ_RTC_ALARM );//get current priority
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler set priority higher than current handler
        priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_RTC_ALARM, priority );//set priority to new level
        __disable_irq( );
        return_rtc_irq = _VectorsRam[IRQ_RTC_ALARM+16];// save prev isr
        attachInterruptVector( IRQ_RTC_ALARM, wakeup_isr );
        __enable_irq( );
    }
    
    if ( SIM_SCGC6 & SIM_SCGC6_RTC ) SIM_SCGC6_clock_active = true;
    else SIM_SCGC6 |= SIM_SCGC6_RTC;
    
    
    if ( mode == 1 ) {
        return_isr_enabled = NVIC_IS_ENABLED( IRQ_RTC_ALARM );
        if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_RTC_ALARM );
    } else {
        llwu_configure_modules_mask( LLWU_RTCA_MOD );
    }
    
    IER = RTC_IER;
    if( timer_ ) // If setting timer style alarm
        RTC_TAR = rtc_get( ) + ( alarm - 1 );
    else        // else, setting true alarm
        RTC_TAR = alarm - 1;
    RTC_IER = RTC_IER_TAIE;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::clearIsrFlags( uint32_t ipsr ) {
    if ( !( SIM_SCGC6 & SIM_SCGC6_RTC ) ) return;
    RTC_TAR = RTC_TSR + 1;
    RTC_SR |= RTC_SR_TAF;
}
#endif /* __MK20DX256__ */
