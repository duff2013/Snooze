//
//  Example Driver Class for the REDUCED_CPU_BLOCK.
//  This Class reconfigures Serial1 to work at 2 MHz cpu
//  and back to F_CPU.
//

#define BAUD2DIV_2MHZ(baud) (((TWO_MHZ * 2) + ((baud) >> 1)) / (baud))
#define BAUD2DIV(baud) (((F_CPU * 2) + ((baud) >> 1)) / (baud))
/***************************************************************************************************/
// C language implementation
//
#ifdef __cplusplus
extern "C" {
#endif
    void serial1_begin_lp(uint32_t divisor) {
#if defined(HAS_KINETISK_UART0)
        UART0_BDH = (divisor >> 13) & 0x1F;
        UART0_BDL = (divisor >> 5) & 0xFF;
        UART0_C4 = divisor & 0x1F;
#elif defined(HAS_KINETISL_UART0)
        UART0_BDH = (divisor >> 8) & 0x1F;
        UART0_BDL = divisor & 0xFF;
#endif
    }
#ifdef __cplusplus
}
#endif
/***************************************************************************************************/
#ifndef HardwareSerial1_LP_h
#define HardwareSerial1_LP_h

#include "Arduino.h"
#include "SnoozeBlock.h"

class HardwareSerial1_LP : public SnoozeBlock {
private:
    virtual void disableDriver( void ) {
        serial1_begin_lp( BAUD2DIV( baudRate ) );
    }
    virtual void enableDriver( void ) {
        serial1_begin_lp( BAUD2DIV_2MHZ( baudRate ) );
    }
    uint32_t baudRate;
public:
    
    HardwareSerial1_LP( uint32_t baud ) : baudRate( baud ) {
        // isUsed variable is needed to be defined and set
        // to true for the driver to be called.
        isUsed = true;
    }
};
#endif /* defined(HardwareSerial1_LP_h) */