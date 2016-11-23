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
 *  SnoozeAlarm.h
 *  Teensy 3.x/LC
 *
 * Purpose: RTC Driver
 *
 ***********************************************************************************/
#ifndef SnoozeAlarm_h
#define SnoozeAlarm_h

#include "Arduino.h"
#include "SnoozeBlock.h"

class SnoozeAlarm : public SnoozeBlock {
private:
    virtual void disableDriver( void );
    virtual void enableDriver( void );
    virtual void clearIsrFlags( void );
    static void isr( void );
    //time_t rtc_set_sync_provider( void );
    void ( * return_rtc_irq ) ( void );
    time_t alarm;
    uint8_t return_priority;
    uint8_t return_isr_enabled;
    uint32_t TAR;
    uint32_t IER;
    bool SIM_SCGC6_clock_active;
public:
    SnoozeAlarm( void ) : TAR( false ), IER( false ),
                        SIM_SCGC6_clock_active( false )
    {
        isDriver = true;
    }
    void setAlarm( uint8_t hours, uint8_t minutes, uint8_t seconds );
};
#endif /* defined(SnoozeAlarm_h) */
