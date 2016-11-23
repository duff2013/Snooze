/***********************************************************************************
 *  SnoozeAlarm.h
 *  Teensy 3.x/LC
 *
 * Purpose: RTC Driver
 *
 ***********************************************************************************/
#include "Arduino.h"
#include "SnoozeAlarm.h"
#include "wake.h"
//#include "TimeLib.h"
#define RTC_IER_TAIE_MASK 0x4u
/*******************************************************************************
 *  <#Description#>
 *
 *  @return <#return value description#>
 *******************************************************************************/
time_t rtc_set_sync_provider( void ) {
    return Teensy3Clock.get( );
}

/*******************************************************************************
 *  <#Description#>
 *
 *  @param hours   <#hours description#>
 *  @param minutes <#minutes description#>
 *  @param seconds <#seconds description#>
 *******************************************************************************/
void SnoozeAlarm::setAlarm(  uint8_t hours, uint8_t minutes, uint8_t seconds ) {
    isUsed = true;
    alarm = hours*3600 + minutes*60 + seconds;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::disableDriver( void ) {
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
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
void SnoozeAlarm::enableDriver( void ) {
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
        return_priority = NVIC_GET_PRIORITY( IRQ_RTC_ALARM );//get current priority
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler set priority higher than current handler
        priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_RTC_ALARM, priority );//set priority to new level
        __disable_irq( );
        return_rtc_irq = _VectorsRam[IRQ_RTC_ALARM+16];// save prev isr
        attachInterruptVector( IRQ_RTC_ALARM, isr );
        __enable_irq( );
    }
    
    if ( SIM_SCGC6 & SIM_SCGC6_RTC ) SIM_SCGC6_clock_active = true;
    else SIM_SCGC6 |= SIM_SCGC6_RTC;
    
    
    if ( mode == VLPW || mode == VLPS) {
        return_isr_enabled = NVIC_IS_ENABLED( IRQ_RTC_ALARM );
        if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_RTC_ALARM );
    } else {
        llwu_configure_modules_mask( LLWU_RTCA_MOD );
    }
    
    IER = RTC_IER;
    RTC_TAR = rtc_get( ) + ( alarm - 1 );
    RTC_IER = RTC_IER_TAIE_MASK;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::clearIsrFlags( void ) {
    isr( );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::isr( void ) {
    if ( !( SIM_SCGC6 & SIM_SCGC6_RTC ) ) return;
    RTC_TAR = RTC_TSR + 1;
    if ( mode == VLPW || mode == VLPS ) source = 35;
}