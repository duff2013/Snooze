/***********************************************************************************
 *  SnoozeSPI.h
 *  Teensy 3.x/LC
 *
 * Purpose: SPI Driver
 *
 ***********************************************************************************/
#include "SnoozeSPI.h"
#define INTERNAL_SPI_CLK_PIN 60
/*******************************************************************************
 *
 *******************************************************************************/
void SnoozeSPI::spiClockPin( uint8_t pin ) {
    if ( pin == BUILTIN_SD_CLK_PIN ) {
        clk_pin = INTERNAL_SPI_CLK_PIN;
    } else {
        clk_pin = pin;
    }
}
/*******************************************************************************
 *  
 *******************************************************************************/
void SnoozeSPI::disableDriver( void ) {
    *portModeRegister( clk_pin ) = 0;
    volatile uint32_t *config;
    config = portConfigRegister( clk_pin );
    *config = return_core_pin_config;
}
/*******************************************************************************
 *  SPI clk -> output low
 *******************************************************************************/
void SnoozeSPI::enableDriver( void ) {
    volatile uint32_t *config;
    config = portConfigRegister( clk_pin );
    return_core_pin_config = *config;
#ifdef KINETISK
    *portModeRegister( clk_pin ) = 1;
#else
    *portModeRegister( clk_pin ) |= digitalPinToBitMask( clk_pin ); // TODO: atomic
#endif
    *config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX( 1 );
    *config &= ~PORT_PCR_ODE;
}
