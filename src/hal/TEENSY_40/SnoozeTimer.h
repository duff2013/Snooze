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
 *  SnoozeTimer.h
 *  Teensy 4.0
 *
 * Purpose: Low Power Timer Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#ifndef SnoozeTimer_h
#define SnoozeTimer_h

#include "Arduino.h"
#include "common.h"
#include "SnoozeBlock.h"

class SnoozeTimer : public SnoozeBlock {
private:
    static void ( * return_gpt1_irq ) ( void );
    uint32_t gpt1_cr;
    uint32_t gpt1_pr;
    uint32_t gpt1_ocr1;
    uint32_t xtalosc24m_config0;
    uint32_t xtalosc24m_config1;
    uint32_t xtalosc24m_config2;
    uint32_t iomuxc_gpr_gpr5;
    uint8_t return_priority;
    uint8_t return_isr_enabled;
    bool CCM_CCGR1_clock_active;
    virtual void disableDriver( uint8_t mode );
    virtual void enableDriver( uint8_t mode );
    virtual void clearIsrFlags( uint32_t ipsr );
    static volatile uint32_t period;
public:
    SnoozeTimer( void ) : gpt1_cr( 0 ), gpt1_pr( 0 ), gpt1_ocr1( 0 ),
    xtalosc24m_config0( 0 ), xtalosc24m_config1( 0 ),
    xtalosc24m_config2( 0 ), iomuxc_gpr_gpr5( 0 ),
    return_priority( 0 ), return_isr_enabled( 0 ),
    CCM_CCGR1_clock_active( 0 )
    {
        isDriver = true;
        period = 0;
    }
    void setTimer( uint32_t newPeriod );
};
#endif /* SnoozeTimer_h */
#endif /* __IMXRT1062__ */
