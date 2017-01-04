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
 * Purpose: Low Power Timer Driver
 *
 ***********************************************************************************/
#ifndef SnoozeTimer_h
#define SnoozeTimer_h

#include "Arduino.h"
#include "SnoozeBlock.h"

class SnoozeTimer : public SnoozeBlock {
private:
    static void ( * return_lptmr_irq ) ( void );
    uint8_t return_priority;
    uint8_t return_isr_enabled;
    virtual void disableDriver( void );
    virtual void enableDriver( void );
    virtual void clearIsrFlags( void );
    static void isr( void );
    uint16_t period;
    uint32_t PSR;
    uint32_t CMR;
    uint32_t CSR;
    bool SIM_SCGC5_clock_active;
    bool OSC_clock_active;
    static volatile uint16_t lptmrUpdateSystick;
public:
    SnoozeTimer( void ) : PSR( 0 ), CMR( 0 ), CSR( 0 ),
                  SIM_SCGC5_clock_active(false ),
                  OSC_clock_active( false )
    {
        lptmrUpdateSystick = 0;
        isDriver = true;
        period = 0;
    }
    void setTimer( uint16_t newPeriod );
};
#endif /* defined(SnoozeTimer_h) */
