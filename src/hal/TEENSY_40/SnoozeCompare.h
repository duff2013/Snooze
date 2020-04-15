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
 *  SnoozeCompare.h
 *  Teensy 4.0
 *
 * Purpose: Compare Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#ifndef SnoozeCompare_h
#define SnoozeCompare_h

#include "SnoozeBlock.h"
#include "common.h"

class SnoozeCompare : public SnoozeBlock {
private:
    virtual void enableDriver( uint8_t mode );
    virtual void disableDriver( uint8_t mode );
    virtual void clearIsrFlags( uint32_t ipsr );
    static void ( * return_cmp_irq ) ( void );
    void ( * return_cmp1_irq ) ( void );
    void ( * return_cmp2_irq ) ( void );
    float  threshold_crossing;
    //uint8_t return_priority_cmp;
    uint8_t return_priority;
    uint8_t return_isr_enabled;
    uint8_t pin;
    uint8_t type;
    uint8_t CR0;
    uint8_t CR1;
    uint8_t SCR;
    uint8_t FPR;
    uint8_t MUXCR;
    uint8_t DACCR;
    uint32_t PSR;
    uint32_t CMR;
    uint32_t CSR;
    bool CCM_CCGR3_clock_active;
    volatile uint32_t return_core_pin_config[12];
    static volatile uint8_t *cmpx_cr0;//CMP0_CR0;
    static volatile uint8_t *cmpx_cr1;//CMP0_CR1;
    static volatile uint8_t *cmpx_scr;//CMP0_SCR;
    static volatile uint8_t *cmpx_fpr;//CMP0_FPR;
    static volatile uint8_t *cmpx_muxcr;//CMP0_MUXCR;
    static volatile uint8_t *cmpx_daccr;//CMP0_DACCR;
public:
    SnoozeCompare( void ) {
        PSR = CMR = CSR = 0;
        CR0 = CR1 = SCR = 0;
        FPR = MUXCR = DACCR = 0;
        CCM_CCGR3_clock_active = false;
        isDriver = true;
    }
    void pinMode( int _pin, int _type, float val );
};
#endif /* SnoozeCompare_h */
#endif /* __IMXRT1062__ */
