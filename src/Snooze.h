/******************************************************************************
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
 *******************************************************************************
 *  Snooze.cpp
 *  Supported Chips:
 *         1. Teensy LC/3.2/3.5/3.6/4.0
 *
 * Purpose:    Provides routines for configuring low power modes.
 *
 * NOTE:       None
 *******************************************************************************/
#ifndef Snooze_h
#define Snooze_h
/***************************************************************************/
#include "Arduino.h"
#include "SnoozeBlock.h"
#if defined(__MKL26Z64__)
#include "hal/TEENSY_LC/hal.h"
#elif defined(__MK20DX256__)
#include "hal/TEENSY_32/hal.h"
#elif defined(__MK64FX512__)
#include "hal/TEENSY_35/hal.h"
#elif defined(__MK66FX1M0__)
#include "hal/TEENSY_36/hal.h"
#elif defined(__IMXRT1062__)
#include "hal/TEENSY_40/hal.h"
#endif
/***************************************************************************/
class SnoozeClass {
private:
    static volatile uint32_t PCR3;
public:
    constexpr SnoozeClass( void ) { }
    static void idle ( SNOOZE_BLOCK );
    /* sleep functions */
    static int sleep    ( SNOOZE_BLOCK );
    //static int deepSleep( SNOOZE_BLOCK, SLEEP_MODE mode = LLS );
    //static int hibernate( SNOOZE_BLOCK,  SLEEP_MODE mode = LLS );
    static int deepSleep( SNOOZE_BLOCK );
    static int hibernate( SNOOZE_BLOCK );
};
extern SnoozeClass Snooze;
/***************************************************************************/
#endif /* Snooze_h */
