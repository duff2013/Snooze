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
 *  SnoozeSPI.h
 *  Teensy 3.x/LC
 *
 * Purpose: Low Power USB Serial Driver
 *
 ***********************************************************************************/
#ifndef SnoozeSPI_h
#define SnoozeSPI_h

#include "SnoozeBlock.h"

#define BUILTIN_SD_CLK_PIN 254

class SnoozeSPI : public SnoozeBlock {
    private:
    virtual void enableDriver( void );
    virtual void disableDriver( void );
    virtual void clearIsrFlags( void ) {}
    static void isr( void ) {}
    volatile uint32_t return_core_pin_config;
    public:
    SnoozeSPI( void ) {
        isDriver = true;
        isUsed = true;
    }
    uint32_t clk_pin;
    void spiClockPin( uint8_t pin );
};
#endif /* defined(SnoozeSPI_h) */
