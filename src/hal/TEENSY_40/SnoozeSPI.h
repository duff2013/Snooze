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
 *  SnoozeSPI.h
 *  Teensy 4.0
 *
 *  Purpose: Low Power USB Serial Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#ifndef SnoozeSPI_h
#define SnoozeSPI_h

#include "SnoozeBlock.h"
#include "common.h"
#include "SPI.h"

#define BUILTIN_SD_CLK_PIN 254

class SnoozeSPI : public SnoozeBlock {
    private:
    IMXRT_LPSPI_t & port() { return *(IMXRT_LPSPI_t *)port_addr; }
    const SPIClass::SPI_Hardware_t & hardware() { return *(const SPIClass::SPI_Hardware_t *)hardware_addr; }
    uintptr_t port_addr;
    uintptr_t hardware_addr;
    //static SPIClass::SPI_Hardware_t *port_hw;
    virtual void enableDriver( uint8_t mode );
    virtual void disableDriver( uint8_t mode );
    virtual void clearIsrFlags( uint32_t ipsr ) {}
    static void isr( void ) { }
    //uintptr_t *port;
    public:
    SnoozeSPI( void ) {
        isDriver = true;
        isUsed = true;
    }
    uint32_t clk_pin;
    void setClockPin( uint8_t pin );
    void setSPIClass( SPIClass *p );
};
#endif /* SnoozeSPI_h */
#endif /* __IMXRT1062__ */
