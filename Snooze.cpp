/*******************************************************************************
 *  Snooze.cpp
 *  Teensy 3.x/LC
 *
 * Purpose:    Provides routines for configuring the Teensy for low power.
 *
 * NOTE:       None
 *******************************************************************************/

#include "Snooze.h"
#include "Arduino.h"
#include "util/atomic.h"
/**
 *  SnoozeBlock Constructor
 */
SnoozeBlock::SnoozeBlock( void ) {

}
/**
 *  pinMode - Configures either a digital pin or tsi pin as a wakeup source.
 *  
 *  Teensy 3.x (digital pin)
 *  @param pin  - sleep pins(1-33), deepSleep, hibernate pins (2,4,6,7,9,10,11,13,16,21,22,26,30,33)
 *  @param mode - |INPUT|INPUT_PULLUP|OUTPUT|
 *  @param val  - |HIGH|LOW|RISING|FALLING|CHANGE|
 *
 *  Teensy 3.x (tsi pin)
 *  @param pin  - sleep pins(does not work), deepSleep, hibernate pins (0,1,15,16,17,18,19,22,23,25,32,33)
 *  @param mode - |TSI|
 *  @param val  - threshold capacitance value, greater than this value wakes.
 *
 *  Teensy LC (digital pin)
 *  @param pin  - sleep pins(1-33), deepSleep, hibernate pins (2,4,6,7,9,10,11,13,16,21,22,26)
 *  @param mode - |INPUT|INPUT_PULLUP|OUTPUT|
 *  @param val  - |HIGH|LOW|RISING|FALLING|CHANGE|
 *
 *  Teensy LC (tsi pin)
 *  @param pin  - sleep pins(does not work), deepSleep, hibernate pins (0,1,3,4,15,16,17,18,19,22,23)
 *  @param mode - |TSI|
 *  @param val  - threshold capacitance value, greater than this value wakes.
 */
void SnoozeBlock::pinMode( int pin, int mode, int val ) {
    if ( mode == INPUT ) {
        digital_configure_pin_mask( pin, mode, val, &digital_mask );
        llwu_configure_pin_mask( pin, val, &llwu_mask );
    }
    else if ( mode == INPUT_PULLUP ) {
        digital_configure_pin_mask( pin, mode, val, &digital_mask );
        llwu_configure_pin_mask( pin, val, &llwu_mask );
    }
    else if ( mode == OUTPUT ) {
        digital_configure_pin_mask( pin, mode, val, &digital_mask );
        llwu_configure_pin_mask( pin, val, &llwu_mask );
    }
    else if ( mode == TSI ) {
        tsi_configure_pin_mask( pin, val, &tsi_mask );
        llwu_configure_modules_mask( LLWU_TSI_MOD, &llwu_mask );
    }
}
/**
 *  pinMode - Configures compare wakeup source.
 *
 *  @param pin  either pin 11 or 12 can be used
 *  @param mode |CMP|
 *  @param type |RISING|FALLING|
 *  @param val  threshold crossing value in volts.
 */
void SnoozeBlock::pinMode( int pin, int mode, int type, double val ) {
    if ( mode == CMP ) {
        cmp_configure_pin_mask( pin, mode, type, val, &cmp_mask );
        llwu_configure_modules_mask( LLWU_CMP0_MOD, &llwu_mask );
    }
}
/**
 *  setTimer - wake up period in milli seconds in 1ms granularity.
 *
 *  @param period 1ms to 65535ms
 */
void SnoozeBlock::setTimer( uint16_t period ) {
    lptmr_configure_period_mask( period, &lptmr_mask );
    llwu_configure_modules_mask( LLWU_LPTMR_MOD, &llwu_mask );
}
#ifdef KINETISK
/**
 *  setAlarm - Configures RTC wakeup source. hr, min and sec will be added for the total time.
 *
 *  @param hours   number of hours in the future.
 *  @param minutes number of minutes in the future.
 *  @param seconds number of seconds in the future.
 */
void SnoozeBlock::setAlarm( uint8_t hours, uint8_t minutes, uint8_t seconds ) {
    rtc_configure_alarm_mask( hours, minutes, seconds, &rtc_mask );
    llwu_configure_modules_mask( LLWU_RTCA_MOD, &llwu_mask );
}
#endif
/**
 *  setLowVoltge - Configure vin low voltage wakeup
 *
 *  @param threshold volts compared to vin, below this will wake the cpu.
 */
void SnoozeBlock::setLowVoltage( double threshold ) {
    
}
/*********************************************************************************************/
DMAMEM SLEEP_MODE SnoozeClass::sleep_mode;
CLOCK_MODE SnoozeClass::clock_mode;

/**
 *  SnoozeClass Constructor
 */
SnoozeClass::SnoozeClass( void ) {
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
    attachInterruptVector( IRQ_LLWU, wakeupISR );
    NVIC_SET_PRIORITY( IRQ_LLWU, 32 );
    clock_mode = mcg_mode( );
}
/**
 *  source - returns the wakeup source.
 *
 *  @return Digital pins return pin number, CMP - 34, RTC - 35, LPTMR - 36, TSI - 37.
 */
int SnoozeClass::source( void ) {
    return wakeupSource;
}

/**
 *  idle - puts processor into wait mode until next interrupt typically systick.
 */
void SnoozeClass::idle( void ) {
    enter_wait( );
}

/**
 *  sleep - most versatile sleep mode. SnoozeBlock configuration or any interrupt can wake the processor.
 *
 *  @param configuration SnoozeBlock class config.
 *
 *  @return wakeup source
 */
int SnoozeClass::sleep( SnoozeBlock &configuration ) { 
    SnoozeBlock *p = &configuration;
    enable_periph_irq = true;
#ifdef KINETISL
    tsi_set( &p->tsi_mask );
#endif
    cmp_set( &p->cmp_mask );
    lptmr_set( &p->lptmr_mask );
#ifdef KINETISK
    rtc_alarm_set( &p->rtc_mask );
#endif
    digital_set( &p->digital_mask );
#ifdef KINETISL
    pinMode( 17, OUTPUT );
    digitalWriteFast( 17, LOW );
#endif
    if ( mcg_mode( ) == BLPI ) {
        peripheral_disable( &p->setPeripheral.periph_off_mask );
        enter_wait( );
        peripheral_set( &p->setPeripheral.periph_off_mask );
    }
    else if ( mcg_mode( ) == BLPE ) {
        peripheral_set( &p->setPeripheral.periph_off_mask );
        blpe_blpi (   );
        enter_vlpr( 0 );// now safe to enter vlpr
        enter_wait(   );
        exit_vlpr (   );
        blpi_blpe (   );
        peripheral_set( &p->setPeripheral.periph_off_mask );
    }
    else {
        peripheral_set( &p->setPeripheral.periph_off_mask );
        usbDisable(   );
        pee_blpi  (   );
        enter_vlpr( 0 );// now safe to enter vlpr
        enter_wait(   );
        exit_vlpr (   );
        blpi_pee  (   );
        usbEnable (   );
        peripheral_set( &p->setPeripheral.periph_off_mask );
    }
    digital_disable( &p->digital_mask );
#ifdef KINETISK
    rtc_disable( &p->rtc_mask );
#endif
    lptmr_disable( &p->lptmr_mask );
    cmp_disable( &p->cmp_mask );
#ifdef KINETISL
    tsi_disable( &p->tsi_mask );
#endif
    return wakeupSource;
}

/**
 *  deepSleep - LLWU is used to handle interrupts that wake. USB regulator is enabled and 3.3V output pin can supply full current (100mA MAX).
 *
 *  @param configuration SnoozeBlock class config.
 *  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
 *
 *  @return wakeup source
 */
int SnoozeClass::deepSleep( SnoozeBlock &configuration, SLEEP_MODE mode ) {
    SnoozeBlock *p = &configuration;
    enable_periph_irq = false;
    sleep_mode = mode;
    cmp_set( &p->cmp_mask );
    digital_set( &p->digital_mask );
    lptmr_set( &p->lptmr_mask );
#ifdef KINETISK
    rtc_alarm_set( &p->rtc_mask );
#endif
#ifdef KINETISL
    pinMode( 17, OUTPUT );
    digitalWriteFast( 17, LOW );
#endif
    tsi_set( &p->tsi_mask );
    llwu_set( &p->llwu_mask );
    if ( mode == LLS )        { enter_lls( ); }
    else if ( mode == VLLS3 ) { enter_vlls3( ); }
    else if ( mode == VLLS2 ) { enter_vlls2( ); }
    else if ( mode == VLLS1 ) { enter_vlls1( ); }
    else if ( mode == VLLS0 ) { enter_vlls0( ); }
    llwu_disable( );
    tsi_disable( &p->tsi_mask );
#ifdef KINETISK
    rtc_disable( &p->rtc_mask );
#endif
    lptmr_disable( &p->lptmr_mask );
    cmp_disable( &p->cmp_mask );
    return wakeupSource;
}

#if defined( USE_HIBERNATE )

/**
 *  hibernate - LLWU is used to handle interrupts that wake. USB regulator is disabled and 3.3V output pin can only supply limited current and voltage drops to ~2.7V.
 *
 *  @param configuration SnoozeBlock class config.
 *  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
 *
 *  @return wakeup source
 */
int SnoozeClass::hibernate( SnoozeBlock &configuration, SLEEP_MODE mode ) {
    SnoozeBlock *p = &configuration;
    enable_periph_irq = false;
    sleep_mode = mode;
    cmp_set( &p->cmp_mask );
    digital_set( &p->digital_mask );
    lptmr_set( &p->lptmr_mask );
#ifdef KINETISK
    rtc_alarm_set( &p->rtc_mask );
#endif
#ifdef KINETISL
    pinMode( 17, OUTPUT );
    digitalWriteFast( 17, LOW );
#endif
    tsi_set( &p->tsi_mask );
    llwu_set( &p->llwu_mask );
    enableHibernate( );
    if ( mode == LLS )        { enter_lls( ); }
    else if ( mode == VLLS3 ) { enter_vlls3( ); }
    else if ( mode == VLLS2 ) { enter_vlls2( ); }
    else if ( mode == VLLS1 ) { enter_vlls1( ); }
    else if ( mode == VLLS0 ) { enter_vlls0( ); }
    disableHibernate( );
    llwu_disable( );
    tsi_disable( &p->tsi_mask );
#ifdef KINETISK
    rtc_disable( &p->rtc_mask );
#endif
    lptmr_disable( &p->lptmr_mask );
    cmp_disable( &p->cmp_mask );
    return wakeupSource;
}
#endif

/**
 *  wakeupISR - handle LLWU interrupts after wakeup.
 */
void SnoozeClass::wakeupISR( void ) {
    NVIC_DISABLE_IRQ( IRQ_LLWU ); // disable wakeup isr
    if ( sleep_mode == LLS ) {
        __disable_irq( );
        llwuFlag = llwu_clear_flags( );// clear flags
        lptmrISR( );
        cmp0ISR( );
#ifdef KINETISK
        rtcISR( );
#endif
        tsiISR( );
        /************************************
         * back to PEE if in PBE, else it is
         * in either BLPI/BLPE, if so nothing
         * to do.
         ************************************/
        pbe_pee( );
        __enable_irq( );
    }
}
/*********************************************************************************************/
SnoozeClass Snooze;