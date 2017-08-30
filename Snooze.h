/******************************************************************************
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
 ******************************************************************************
 *  Snooze.cpp
 *  Teensy 3.x/LC
 *
 * Purpose:    Provides routines for configuring the Teensy for low power.
 *
 * NOTE:       None
 *******************************************************************************/
#ifndef Snooze_h
#define Snooze_h

/***************************************************************************/
#include "SnoozeBlock.h"
#include "utility/SnoozeTimer.h"
#include "utility/SnoozeAlarm.h"
#include "utility/SnoozeTouch.h"
#include "utility/SnoozeCompare.h"
#include "utility/SnoozeDigital.h"
#include "utility/SnoozeAudio.h"
#include "utility/SnoozeUSBSerial.h"
#include "utility/Snoozelc5vBuffer.h"
#include "utility/SnoozeSPI.h"
/***************************************************************************/
class SnoozeClass {
private:
    static volatile uint32_t PCR3;
public:
    constexpr SnoozeClass( void );// {}
    static void idle ( SNOOZE_BLOCK );
    static int source( SNOOZE_BLOCK );
    /* sleep functions */
    static int sleep    ( SNOOZE_BLOCK );
    static int deepSleep( SNOOZE_BLOCK, SLEEP_MODE mode = LLS );
    static int hibernate( SNOOZE_BLOCK,  SLEEP_MODE mode = LLS );
};
extern SnoozeClass Snooze;
/***************************************************************************/
#endif /* defined(Snooze_h) */
