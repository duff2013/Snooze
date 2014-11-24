/*******************************************************************************
 *  Snooze.cpp
 *  Teensy3
 *
 * Purpose:    Provides routines for configuring the Teensy for low power.
 *
 * NOTE:       None
 *******************************************************************************/

#include "Snooze.h"
#include "Arduino.h"
#include "util/atomic.h"

void SnoozeBlock::pinMode( int pin, int mode, int val ) {
    if ( mode == INPUT ) {
        digital_configure_pin_mask( pin, mode, val, &digital_mask );
        llwu_configure_pin_mask( pin, val, &llwu_mask );
    }
    else if ( mode == INPUT_PULLUP ) {
        digital_configure_pin_mask( pin, mode, val, &digital_mask );
        llwu_configure_pin_mask( pin, val, &llwu_mask );
    }
    else if ( mode == TSI ) {
        tsi_configure_pin_mask( pin, val, &tsi_mask );
        llwu_configure_modules_mask( LLWU_TSI_MOD, &llwu_mask );
    }
}

void SnoozeBlock::pinMode( int pin, int mode, int type, double val ) {
    if ( mode == CMP ) {
        cmp_configure_pin_mask( pin, mode, type, val, &cmp_mask );
        llwu_configure_modules_mask( LLWU_CMP0_MOD, &llwu_mask );
    }
}

void SnoozeBlock::setTimer( uint32_t period ) {
    lptmr_configure_period_mask( period, &lptmr_mask );
    llwu_configure_modules_mask( LLWU_LPTMR_MOD, &llwu_mask );
}

void SnoozeBlock::setAlarm( uint8_t hours, uint8_t minutes, uint8_t seconds ) {
    rtc_configure_alarm_mask( hours, minutes, seconds, &rtc_mask );
    llwu_configure_modules_mask( LLWU_RTCA_MOD, &llwu_mask );
}

void SnoozeBlock::setLowVoltage( double threshold ) {
    
}
/*********************************************************************************************/
SLEEP_MODE SnoozeClass::sleep_mode;
CLOCK_MODE SnoozeClass::clock_mode;

SnoozeClass::SnoozeClass( void ) {
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE_MASK;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY_MASK;
    attachInterruptVector( IRQ_LLWU, wakeupISR );
    lptmr_init( );
    clock_mode = mcg_mode( );
#if defined( USE_HIBERNATE )
    volatile int n;
    for (n = 0; n < F_CPU*1000000; n++);
#endif
}
//--------------------------------------source-------------------------------------------
int SnoozeClass::source( void ) {
    return wakeupSource;
}
//---------------------------------------idle--------------------------------------------
void SnoozeClass::idle( void ) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_wait(); }
}
//---------------------------------------Sleep-------------------------------------------
int SnoozeClass::sleep( SnoozeBlock &configuration ) {
    SnoozeBlock *p = &configuration;
    cmp_set( &p->cmp_mask );
    digital_set( &p->digital_mask );
    lptmr_set( &p->lptmr_mask );
    rtc_alarm_set( &p->rtc_mask );
    
    if ( mcg_mode( ) == BLPI ) {
        peripheral_disable( &p->setPeripheral.periph_off_mask );
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_wait( ); }
        peripheral_set( &p->setPeripheral.periph_off_mask );
    }
    else if ( mcg_mode( ) == BLPE ) {
        peripheral_set( &p->setPeripheral.periph_off_mask );
        blpe_blpi( );
        enter_vlpr( 0 );// now safe to enter vlpr
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_wait( ); }
        exit_vlpr( );
        blpi_blpe( );
        peripheral_set( &p->setPeripheral.periph_off_mask );
    }
    else {
        usbDisable( );
        peripheral_set( &p->setPeripheral.periph_off_mask );
        pee_blpi( );
        enter_vlpr( 0 );// now safe to enter vlpr
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_wait( ); }
        exit_vlpr( );
        blpi_pee( );
        peripheral_set( &p->setPeripheral.periph_off_mask );
        usbEnable( );
    }
    rtc_disable( &p->rtc_mask );
    cmp_disable( &p->cmp_mask );
    lptmr_disable( &p->lptmr_mask );
    digital_disable( &p->digital_mask );
    
    /*Serial.println( p->setPeripheral.periph_off_mask.SCGC4, BIN );
    Serial.println( p->setPeripheral.periph_off_mask.SCGC5, BIN );
    Serial.println( p->setPeripheral.periph_off_mask.SCGC6, BIN );
    Serial.println();
    Serial.println( SIM_SCGC4, BIN );
    Serial.println( SIM_SCGC5, BIN );
    Serial.println( SIM_SCGC6, BIN );
    
    Serial.println();
    Serial.println( SIM_SCGC4, BIN );
    Serial.println( SIM_SCGC5, BIN );
    Serial.println( SIM_SCGC6, BIN );*/
}
//--------------------------------------DeepSleep----------------------------------------
int SnoozeClass::deepSleep( SnoozeBlock &configuration, SLEEP_MODE mode ) {
    SnoozeBlock *p = &configuration;
    sleep_mode = mode;
    cmp_set( &p->cmp_mask );
    digital_set( &p->digital_mask );
    lptmr_set( &p->lptmr_mask );
    rtc_alarm_set( &p->rtc_mask );
    tsi_set( &p->tsi_mask );
    llwu_set( &p->llwu_mask );

    if ( mode == LLS ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_lls( ); }
    }
    else if ( mode == VLLS3 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls3( ); }
    }
    else if ( mode == VLLS2 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls2( ); }
    }
    else if ( mode == VLLS1 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls1( ); }
    }
    else if ( mode == VLLS0 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls0( ); }
    }
    
    llwu_disable( );
    tsi_disable( &p->tsi_mask );
    rtc_disable( &p->rtc_mask );
    lptmr_disable( &p->lptmr_mask );
    digital_disable( &p->digital_mask );
    cmp_disable( &p->cmp_mask );
}
//--------------------------------------Hibernate----------------------------------------
#if defined( USE_HIBERNATE )
int SnoozeClass::hibernate( SnoozeBlock &configuration, SLEEP_MODE mode ) {
    SnoozeBlock *p = &configuration;
    sleep_mode = mode;
    cmp_set( &p->cmp_mask );
    digital_set( &p->digital_mask );
    lptmr_set( &p->lptmr_mask );
    rtc_alarm_set( &p->rtc_mask );
    tsi_set( &p->tsi_mask );
    llwu_set( &p->llwu_mask );

    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE_MASK;
    SIM_SOPT1 |= SIM_SOPT1_USBSSTBY_MASK;
    uint32_t PCR3 = PORTA_PCR3;
    PORTA_PCR3 = PORT_PCR_MUX( 0 );
    if ( mode == LLS ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_lls( ); }
    }
    else if ( mode == VLLS3 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls3( ); }
    }
    else if ( mode == VLLS2 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls2( ); }
    }
    else if ( mode == VLLS1 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls1( ); }
    }
    else if ( mode == VLLS0 ) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { enter_vlls0( ); }
    }
    PORTA_PCR3 = PCR3;
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE_MASK;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY_MASK;
    
    tsi_disable( &p->tsi_mask );
    rtc_disable( &p->rtc_mask );
    lptmr_disable( &p->lptmr_mask );
    digital_disable( &p->digital_mask );
    cmp_disable( &p->cmp_mask );
}
#endif
//----------------------------------------wakeup------------------------------------------
void SnoozeClass::wakeupISR( void ) {
    NVIC_DISABLE_IRQ( IRQ_LLWU ); // disable wakeup isr
    if ( sleep_mode == LLS ) {
        __disable_irq( );
        llwuFlag = llwu_clear_flags( );// clear flags
        __enable_irq( );
        lptmrISR( );
        cmp0ISR( );
        rtcISR( );
        tsiISR( );
        /************************************
         * back to PEE if in PBE, else it is
         * in either BLPI/BLPE, if so nothing
         * to do.
         ************************************/
        pbe_pee( );
    }
}
/*********************************************************************************************/
SnoozeClass Snooze;