//
//  Example Driver Class for the REDUCED_CPU_BLOCK.
//  This Class reconfigures Serial1 to work at 2 MHz cpu
//  and back to F_CPU.
//

#if defined(HAS_KINETISK_UART0)
#define BAUD2DIV_2MHZ(baud) (((TWO_MHZ * 2) + ((baud) >> 1)) / (baud))
#elif defined(HAS_KINETISL_UART0)
// LC uses OSCERCLK, which is 16 MHz. Oversample rate is 4.
#define BAUD2DIV_2MHZ(baud) (((SIXTEEN_MHZ / 4) + ((baud) >> 1)) / (baud))

#define SIM_SOPT2_UART0SRC_MASK 0x0C000000
#define UART0_C4_OSR_MASK 0x1F
#define UART0_C4_OSR(n) (((n) & UART0_C4_OSR_MASK))  //  Oversample rate - 1
#endif
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
 
#if defined(HAS_KINETISL_UART0)
        // Restore UART source and oversample rate (OSR)
        SIM_SOPT2 = SIM_SOPT2 & ~SIM_SOPT2_UART0SRC_MASK | previousUARTSRC;
        UART0_C4 = UART0_C4 & ~UART0_C4_OSR_MASK | previousOSR;
#endif
    }
    virtual void enableDriver( void ) {
        serial1_begin_lp( BAUD2DIV_2MHZ( baudRate ) );

#if defined(HAS_KINETISL_UART0)
        // Save current values for restoring later
        previousUARTSRC = SIM_SOPT2 & SIM_SOPT2_UART0SRC_MASK;
        previousOSR = UART0_C4 & UART0_C4_OSR_MASK;
        
        // Change UART source to OSCERCLK
        SIM_SOPT2 = SIM_SOPT2 & ~SIM_SOPT2_UART0SRC_MASK | SIM_SOPT2_UART0SRC(2);

        // Set oversample rate (OSR) to 4 (value = 3 = n - 1)
        UART0_C4 = UART0_C4 & ~UART0_C4_OSR_MASK | UART0_C4_OSR(3);
#endif
    }
    uint32_t baudRate;
    uint32_t previousUARTSRC;
    uint32_t previousOSR;
public:
    
    HardwareSerial1_LP( uint32_t baud ) : baudRate( baud ) {
        // isUsed variable is needed to be defined and set
        // to true for the driver to be called.
        isUsed = true;
    }
};
#endif /* defined(HardwareSerial1_LP_h) */
