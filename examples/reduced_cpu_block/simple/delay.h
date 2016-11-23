#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
    /***************************************************************************************************/
    // delayMicroseconds for 2 MHz cpu speed
    void delay_lp(uint32_t msec);
    uint32_t micros_lp(void);
    
    static inline void delayMicroseconds_lp(uint32_t) __attribute__((always_inline, unused));
    static inline void delayMicroseconds_lp(uint32_t usec) {
        uint32_t n = usec >> 1;
        if (n == 0) return;
        __asm__ volatile(
                         "L_%=_delayMicroseconds:"   "\n\t"
                         "nop"         "\n\t"
#ifdef KINETISL
                         "sub    %0, #1"       "\n\t"
#else
                         "subs   %0, #1"       "\n\t"
#endif
                         "bne    L_%=_delayMicroseconds"   "\n"
                         : "+r" (n) :
                         );
    }
    /***************************************************************************************************/
#ifdef __cplusplus
}
#endif
