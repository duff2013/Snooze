//
//  SnoozeWakeup.cpp
//  Teensyduino_1_27_rc1
//
//  Created by Colin on 12/22/15.
//  Copyright (c) 2015 Colin Duffy. All rights reserved.
//

#include "wake.h"

llwu_mask_t llwuMask;

//#include "Arduino.h"
//#include "SnoozeWake.h"

/*DMAMEM volatile int SnoozeWake::flag = -1;

void SnoozeWake::disableDriver( void ) {
    //Serial.println("SnoozeWakeup disable");
    if (  mode == LLS || mode == VLLS3 || mode == VLLS2 || mode == VLLS1 ) {
        __disable_irq( );
        LLWU_PE1 = 0;
        LLWU_PE2 = 0;
        LLWU_PE3 = 0;
        LLWU_PE4 = 0;
        LLWU_ME  = 0;
        if      ( flag & LLWU_F1_WUF0_MASK ) source = 26;
        else if ( flag & LLWU_F1_WUF3_MASK ) source = 33;
        else if ( flag & LLWU_F1_WUF4_MASK ) source = 4;
        else if ( flag & LLWU_F1_WUF5_MASK ) source = 16;
        else if ( flag & LLWU_F1_WUF6_MASK ) source = 22;
        else if ( flag & LLWU_F1_WUF7_MASK ) source = 9;
        
        else if ( ( flag >> 8 ) & LLWU_F2_WUF8_MASK  ) source = 10;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF9_MASK  ) source = 13;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF10_MASK ) source = 11;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF11_MASK ) source = 30;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF12_MASK ) source = 2;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF13_MASK ) source = 7;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF14_MASK ) source = 6;
        else if ( ( flag >> 8 ) & LLWU_F2_WUF15_MASK ) source = 21;
        
        else if ( ( flag >> 16 ) & LLWU_ME_WUME0_MASK ) source = 36;
        else if ( ( flag >> 16 ) & LLWU_ME_WUME1_MASK ) source = 34;
        else if ( ( flag >> 16 ) & LLWU_ME_WUME4_MASK ) source = 37;
#ifdef KINETISK
        else if ( ( flag >> 16 ) & LLWU_ME_WUME5_MASK ) source = 35;
#endif
        flag = 0;
        __enable_irq( );
    }
}

void SnoozeWake::enableDriver( void ) {
    //Serial.println("SnoozeWakeup enable");
    if ( mode == LLS || mode == VLLS3 || mode == VLLS2 || mode == VLLS1 ) {
        __disable_irq( );
        LLWU_PE1 = PE1;
        LLWU_PE2 = PE2;
        LLWU_PE3 = PE3;
        LLWU_PE4 = PE4;
        LLWU_ME  = ME;
        NVIC_ENABLE_IRQ( IRQ_LLWU ); // enable SnoozeWakeup isr
        __enable_irq( );
    }
}

void SnoozeWake::clearIsrFlags( void ) {
    isr( );
}

void SnoozeWake::pinClear( void ) {
    LLWU_PE1 = 0;
    LLWU_PE2 = 0;
    LLWU_PE3 = 0;
    LLWU_PE4 = 0;
}

void SnoozeWake::moduleClear( void ) {
    LLWU_ME = 0;
}

uint32_t SnoozeWake::clearFlags( void ) {
    uint32_t flag = ( LLWU_F1 | LLWU_F2<<8 | LLWU_F3<<16 );
    LLWU_F1 = 0xFF;
    LLWU_F2 = 0xFF;
    LLWU_F3 = 0xFF;
    LLWU_FILT1 = 0x80;
    LLWU_FILT2 = 0x80;
    return flag;
}

void SnoozeWake::isr( void ) {
    if ( mode == LLS ) {
        __disable_irq( );
        NVIC_DISABLE_IRQ( IRQ_LLWU ); // disable SnoozeWakeup isr
        flag = clearFlags( );
        __enable_irq( );
    }
}

void SnoozeWake::pinConfigure( uint8_t pin, uint8_t rise_fall ) {
    uint8_t detect_type;
    if ( rise_fall == RISING )       detect_type = LLWU_PIN_RISING;
    else if ( rise_fall == FALLING ) detect_type = LLWU_PIN_FALLING;
    else if ( rise_fall == CHANGE )  detect_type = LLWU_PIN_ANY;
    else                             detect_type = LLWU_PIN_ANY;
    
    if ( pin == 2 )       PE4 |= ( detect_type );
    else if ( pin == 4 )  PE2 |= LLWU_PE2_WUPE4 ( detect_type );
    else if ( pin == 6 )  PE4 |= LLWU_PE4_WUPE14( detect_type );
    else if ( pin == 7 )  PE4 |= LLWU_PE4_WUPE13( detect_type );
    else if ( pin == 9 )  PE2 |= LLWU_PE2_WUPE7 ( detect_type );
    else if ( pin == 10 ) PE3 |= LLWU_PE3_WUPE8 ( detect_type );
    else if ( pin == 11 ) PE3 |= LLWU_PE3_WUPE10( detect_type );
    else if ( pin == 13 ) PE3 |= LLWU_PE3_WUPE9 ( detect_type );
    else if ( pin == 16 ) PE2 |= LLWU_PE2_WUPE5 ( detect_type );
    else if ( pin == 21 ) PE4 |= LLWU_PE4_WUPE15( detect_type );
    else if ( pin == 22 ) PE2 |= LLWU_PE2_WUPE6 ( detect_type );
    else if ( pin == 26 ) PE1 |= LLWU_PE1_WUPE0 ( detect_type );
    else if ( pin == 30 ) PE3 |= LLWU_PE3_WUPE11( detect_type );
    else if ( pin == 33 ) PE1 |= LLWU_PE1_WUPE3 ( detect_type );
}

void SnoozeWake::moduleConfigure( uint8_t module ) {
    isUsed = true;
    if( module & LLWU_LPTMR_MOD )      ME |= LLWU_ME_WUME0_MASK;
#ifdef KINETISK
    else if ( module & LLWU_RTCA_MOD ) ME |= LLWU_ME_WUME5_MASK;
    else if ( module & LLWU_RTCS_MOD ) ME |= LLWU_ME_WUME7_MASK;
#endif
    else if ( module & LLWU_TSI_MOD )  ME |= LLWU_ME_WUME4_MASK;
    else if ( module & LLWU_CMP0_MOD ) ME |= LLWU_ME_WUME1_MASK;
    else if ( module & LLWU_CMP1_MOD ) ME |= LLWU_ME_WUME2_MASK;
#ifdef KINETISK
    else if( module & LLWU_CMP2_MOD )  ME |= LLWU_ME_WUME3_MASK;
#endif
}*/