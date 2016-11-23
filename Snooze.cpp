/******************************************************************************
 * Low Power Library for Teensy LC/3.x
 * Copyright (c) 2014, Colin Duffy https://github.com/duff2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************
 *  Snooze.cpp
 *  Teensy 3.x/LC
 *
 * Purpose:    Provides routines for configuring the Teensy for low power.
 *
 * NOTE:       None
 *******************************************************************************/
#include "Snooze.h"
#include "SnoozeBlock.h"
#include "utility/wake.h"
#include "utility/sleep.h"
#include "utility/clocks.h"

volatile uint32_t SnoozeClass::PCR3 = 0;
/**
 *  SnoozeClass Constructor
 */
SnoozeClass::SnoozeClass( void ) {
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
}
/*******************************************************************************
 *  source - returns the wakeup source.
 *
 *  @return Digital pins return pin number, CMP - 34, RTC - 35, LPTMR - 36, TSI - 37.
 *******************************************************************************/
int SnoozeClass::source( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    return p->source;
}
/*******************************************************************************
 *  idle - puts processor into wait mode until next interrupt typically systick.
 *******************************************************************************/
void SnoozeClass::idle( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    p->mode = WAIT;
    wait( );
    p->mode = RUN;
}
/*******************************************************************************
 *  sleep - most versatile sleep mode. SnoozeBlock configuration or any interrupt 
 *          can wake the processor.
 *
 *  @param configuration SnoozeBlock class config.
 *
 *  @return wakeup source
 *******************************************************************************/
int SnoozeClass::sleep( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    p->mode = VLPW;
    //p->mode = VLPS;
    p->enableDriver( );
    pee_blpi( );
    enter_vlpr( );
    //vlps( );
    vlpw( );
    exit_vlpr( );
    blpi_pee( );
    p->disableDriver( );
    p->mode = RUN;
    return p->source;
}
/*******************************************************************************
 *  deepSleep - LLWU is used to handle interrupts that wake. USB regulator is 
 *              enabled and 3.3V output pin can supply full current (100mA MAX).
 *
 *  @param configuration SnoozeBlock class config.
 *  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
 *
 *  @return wakeup source
 *******************************************************************************/
int SnoozeClass::deepSleep( SNOOZE_BLOCK, SLEEP_MODE mode ) {
    int priority = nvic_execution_priority( );// get current priority
    priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    NVIC_SET_PRIORITY( IRQ_LLWU, priority );//set priority to new level
    SnoozeBlock *p = &configuration;
    p->mode = LLS;
    p->enableDriver( );
    llwu_set( );
    switch ( mode ) {
        case LLS:
            lls( );
            break;
        case VLLS3:
            vlls3( );
            break;
        case VLLS2:
            vlls2( );
            break;
        case VLLS1:
            vlls1( );
            break;
        case VLLS0:
            vlls0( );
            break;
        default:
            break;
    }
#if defined(__MK66FX1M0__) || defined(__MKL26Z64__)
    p->source = llwu_disable( );
#else 
    llwu_disable( );
#endif
    p->disableDriver( );
    p->mode = RUN;
    return p->source;
}
/*******************************************************************************
 *  hibernate - LLWU is used to handle interrupts that wake. USB regulator is 
 *              disabled and 3.3V output pin can only supply limited current and 
 *              voltage drops to ~2.7V.
 *
 *  @param configuration SnoozeBlock class config.
 *  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
 *
 *  @return wakeup source
 *******************************************************************************/
int SnoozeClass::hibernate( SNOOZE_BLOCK, SLEEP_MODE mode ) {
    int priority = nvic_execution_priority( );// get current priority
    priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    NVIC_SET_PRIORITY( IRQ_LLWU, priority );//set priority to new level
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 |= SIM_SOPT1_USBSSTBY;
    PCR3 = PORTA_PCR3;
    PORTA_PCR3 = PORT_PCR_MUX( 0 );
    SnoozeBlock *p = &configuration;
    p->mode = mode;
    p->enableDriver( );
    llwu_set( );
    switch ( mode ) {
        case LLS:
            lls( );
            break;
        case VLLS3:
            vlls3( );
            break;
        case VLLS2:
            vlls2( );
            break;
        case VLLS1:
            vlls1( );
            break;
        case VLLS0:
            vlls0( );
            break;
        default:
            break;
    }
#if defined(__MK66FX1M0__) || defined(__MKL26Z64__)
    p->source = llwu_disable( );
#else
    llwu_disable( );
#endif
    p->disableDriver( );
    p->mode = RUN;
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
    PORTA_PCR3 = PCR3;
    return p->source;
}