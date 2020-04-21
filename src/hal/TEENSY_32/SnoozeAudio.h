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
 *  SnoozeAudio.h
 *  Teensy 3.2
 *
 *  Purpose: Teensy 3.x Audio Library Driver Snooze compatibility
 *
 **********************************************************************************/
#if defined(__MK20DX256__)

#ifndef SnoozeAudio_h
#define SnoozeAudio_h

#include "Arduino.h"
#include "common.h"
#include "SnoozeBlock.h"

/*
 #define CORE_PIN58_CONFIG	PORTE_PCR0
 #define CORE_PIN59_CONFIG	PORTE_PCR1
 #define CORE_PIN60_CONFIG	PORTE_PCR2
 #define CORE_PIN61_CONFIG	PORTE_PCR3
 #define CORE_PIN62_CONFIG	PORTE_PCR4
 #define CORE_PIN63_CONFIG	PORTE_PCR5
 */

class SnoozeAudio : public SnoozeBlock {
private:
    virtual void disableDriver( uint8_t mode );
    virtual void enableDriver( uint8_t mode );
    virtual void clearIsrFlags( uint32_t ipsr ) {}
    bool audioADC;
    bool audioDAC;
public:
    SnoozeAudio( void ) : audioADC( false ), audioDAC( false )
    {
        isDriver = true;
    }
    void usingADC( uint8_t ADC_pin );
    void usingDAC( uint8_t DAC_pin );
};
#endif /* SnoozeAudio_h */
#endif /* __MK20DX256__ */
