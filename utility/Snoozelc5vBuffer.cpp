/***********************************************************************************
 *  Snoozelc5vBuffer.h
 *  Teensy 3.x/LC
 *
 * Purpose: LC 5v Buffer
 *
 ***********************************************************************************/
#include "Snoozelc5vBuffer.h"

/*******************************************************************************
 *
 *******************************************************************************/
void Snoozelc5vBuffer::disableDriver( void ) {
#if defined(KINETISL)
    volatile uint32_t *config;
    config = portConfigRegister( 17 );
    return_core_pin_config = *config;
#endif
}
/*******************************************************************************
 *  Configure pin 17 on Teensy LC for OUTPUT LOW
 *******************************************************************************/
void Snoozelc5vBuffer::enableDriver( void ) {
#if defined(KINETISL)
    *portModeRegister( 17 ) &= ~digitalPinToBitMask( 17 );
    volatile uint32_t *config;
    config = portConfigRegister( 17 );
    *config = return_core_pin_config;
    pinMode( 17, OUTPUT );
    digitalWriteFast( 17, LOW );
#endif
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void Snoozelc5vBuffer::clearIsrFlags( void ) {
    //isr( );
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void Snoozelc5vBuffer::isr( void ) {
    
}
