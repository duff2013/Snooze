/***********************************************************************************
 *  Snoozelc5vBuffer.h
 *  Teensy LC
 *
 * Purpose: LC 5v Buffer
 *
 **********************************************************************************/
#if defined(__MKL26Z64__)

#include "Snoozelc5vBuffer.h"

/*******************************************************************************
 *
 *******************************************************************************/
void Snoozelc5vBuffer::disableDriver( uint8_t mode ) {
    volatile uint32_t *config;
    config = portConfigRegister( 17 );
    return_core_pin_config = *config;
}
/*******************************************************************************
 *  Configure pin 17 on Teensy LC for OUTPUT LOW
 *******************************************************************************/
void Snoozelc5vBuffer::enableDriver( uint8_t mode ) {
    *portModeRegister( 17 ) &= ~digitalPinToBitMask( 17 );
    volatile uint32_t *config;
    config = portConfigRegister( 17 );
    *config = return_core_pin_config;
    pinMode( 17, OUTPUT );
    digitalWriteFast( 17, LOW );
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void Snoozelc5vBuffer::clearIsrFlags( uint32_t ipsr ) {

}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void Snoozelc5vBuffer::isr( void ) {
    
}
#endif /* __MKL26Z64__ */
