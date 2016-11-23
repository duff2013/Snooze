/***********************************************************************************
 * Low Power Library for Teensy 3.x
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
 *  SnoozeAudio.h
 *  Teensy 3.x
 *
 * Purpose: Teensy 3.x Audio Library Driver Snooze compatibility
 *
 ***********************************************************************************/
#ifndef SnoozeAudio_h
#define SnoozeAudio_h

#include "Arduino.h"
#include "SnoozeBlock.h"

class SnoozeAudio : public SnoozeBlock {
private:
    virtual void disableDriver( void );
    virtual void enableDriver( void );
public:
    SnoozeAudio( void )
    {
        isDriver = true;
    }
};
#endif /* defined(SnoozeAudio_h) */
