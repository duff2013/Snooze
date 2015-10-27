/***********************************************************************************
 *  cpu.h
 *  Teensy 3.x/LC
 *
 * Purpose: Dynamic CPU scaling F_CPU -> 2MHz VLPR
 *
 ***********************************************************************************/
#ifndef __CPU_H__
#define __CPU_H__
/***********************************************************************************/
#include "Arduino.h"
#include "mcg.h"
#include "smc.h"
#include "peripheral.h"
/***********************************************************************************/
#define TYPE uint8_t
#define REDUCED_CPU_BLOCK( ) \
for ( TYPE __ToDo = set_vlpr(  );  __ToDo;  __ToDo = set_run(  ) )

#ifdef __cplusplus
extern "C" {
#endif
    static inline
    TYPE set_vlpr( void )
    __attribute__((always_inline, unused));
    
    static inline
    TYPE set_vlpr(  ) {
        
        if ( mcg_mode( ) == PEE ) {
            usbDisable(   );
            pee_blpi  (   );
            enter_vlpr( 0 );// now safe to enter vlpr
        }
        else if ( mcg_mode( ) == BLPE ) {
            blpe_blpi (   );
            enter_vlpr( 0 );// now safe to enter vlpr
        }
        return 1;
    }
    
    static inline
    TYPE set_run( void )
    __attribute__((always_inline, unused));
    
    static inline
    TYPE set_run( void ) {
        
        if ( F_CPU >= TWENTY_FOUR_MHZ ) {
            exit_vlpr (   );
            blpi_pee  (   );
            usbEnable (   );
        }
        else if ( F_CPU > TWO_MHZ && F_CPU < TWENTY_FOUR_MHZ  ) {
            exit_vlpr (   );
            blpi_blpe (   );
        }
        return 0;
    }
#ifdef __cplusplus
}
#endif

#endif