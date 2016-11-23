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
 *  SnoozeCompare.h
 *  Teensy 3.x/LC
 *
 * Purpose: Compare Driver
 *
 ***********************************************************************************/

#ifndef SnoozeCompare_h
#define SnoozeCompare_h

#include "Arduino.h"
#include "SnoozeBlock.h"

class SnoozeCompare : public SnoozeBlock {
private:
    virtual void enableDriver( void );
    virtual void disableDriver( void );
    virtual void clearIsrFlags( void );
    static void isr( void );
    void ( * return_cmp0_irq ) ( void );
    float  threshold_crossing;
    uint8_t return_priority_cmp0;
    uint8_t pin;
    uint8_t type;
    uint8_t CR0;
    uint8_t CR1;
    uint8_t SCR;
    uint8_t FPR;
    uint8_t MUXCR;
    uint8_t DACCR;
    bool    SIM_SCGC4_clock_active;
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    uint32_t PSR;
    uint32_t CMR;
    uint32_t CSR;
    bool SIM_SCGC5_clock_active;
#endif
    volatile uint32_t return_core_pin_config[2];
public:
    SnoozeCompare( void ) {
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
        PSR = CMR = CSR = 0;
        SIM_SCGC5_clock_active = false;
#endif
        SIM_SCGC4_clock_active = false;
        CR0 = CR1 = SCR = 0;
        FPR = MUXCR = DACCR = 0;
        isDriver = true;
    }
    void pinMode( int _pin, int _type, float val );
};
#endif /* defined(SnoozeCompare_h) */
