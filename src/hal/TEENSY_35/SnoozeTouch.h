/***********************************************************************************
 * Low Power Library
 * Copyright (c) 2019, Colin Duffy https://github.com/duff2013
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
 ***********************************************************************************
 *  SnoozeTouch.h
 *  Teensy 3.5
 *
 * Purpose: Touch (TSI) Driver
 *
 **********************************************************************************/
#if defined(__MK64FX512__)

#ifndef SnoozeTouch_h
#define SnoozeTouch_h

#include "Arduino.h"
#include "common.h"
#include "SnoozeBlock.h"

class SnoozeTouch : public SnoozeBlock {
private:
    virtual void enableDriver( uint8_t type );
    virtual void disableDriver( uint8_t type );
    virtual void clearIsrFlags( void );
    static void isr( void );
public:
    SnoozeTouch( void )
    {
        isDriver = true;
    }
    void pinMode( int _pin, int thresh ) {}
};
#endif /* defined(SnoozeTouch_h) */

#endif
