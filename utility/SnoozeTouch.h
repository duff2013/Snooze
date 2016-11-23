/***********************************************************************************
 * Low Power Library for Teensy LC/3.x
 * Copyright (c) 2016, Colin Duffy https://github.com/duff2013
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
 ************************************************************************************
 *  SnoozeTimer.h
 *  Teensy 3.x/LC
 *
 * Purpose: Touch (TSI) Driver
 *
 ***********************************************************************************/
#ifndef SnoozeTouch_h
#define SnoozeTouch_h

#include "Arduino.h"
#include "SnoozeBlock.h"

#if defined(HAS_KINETIS_TSI) || defined(HAS_KINETIS_TSI_LITE)
class SnoozeTouch : public SnoozeBlock {
private:
    virtual void enableDriver( void );
    virtual void disableDriver( void );
    virtual void clearIsrFlags( void );
    static void isr( void );
    void ( * return_tsi0_irq ) ( void );
    uint8_t  pin;
    uint16_t threshold;
    uint32_t GENCS;
    uint32_t THRESHOLD;
#if defined(HAS_KINETIS_TSI)
    uint32_t SCANC;
    uint32_t PEN;
#elif defined(HAS_KINETIS_TSI_LITE)
    uint32_t DATA;
#endif
    
#if defined(HAS_KINETIS_TSI_LITE)
    uint32_t PSR;
    uint32_t CMR;
    uint32_t CSR;
    bool timer_clock_active;
#endif
    //uint8_t return_priority;
    //uint8_t return_isr_enabled;
    bool SIM_SCGC5_clock_active;
    volatile uint32_t return_core_pin_config;
    //bool OSC_clock_active;
public:
    SnoozeTouch( void ) : pin( 0x03 ), threshold( 0 ), GENCS( 0 ),
                          THRESHOLD( 0 ),
#if defined(HAS_KINETIS_TSI)
                          SCANC( 0 ), PEN( 0 )
#elif defined(HAS_KINETIS_TSI_LITE)
                          DATA( 0 ),PSR( 0 ), CMR( 0 ), CSR( 0 ),
                          timer_clock_active( 0 )
#endif
    {
        isDriver = true;
    }
    void pinMode( int _pin, int thresh );
};
#else
/***********************************************************************************/
class SnoozeTouch : public SnoozeBlock {
private:
    virtual void enableDriver( void )   { }
    virtual void disableDriver( void )  { }
    virtual void clearIsrFlags( void )  { }
    static void isr( void )             { }
    void ( * return_tsi0_irq ) ( void );
public:
    SnoozeTouch( void ) {
        isDriver = true;
    }
    void pinMode( int _pin, int thresh ) { }
};
#endif
#endif /* defined(SnoozeTouch_h) */
