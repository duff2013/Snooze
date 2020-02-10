/***********************************************************************************
 *  SnoozeSPI.h
 *  Teensy LC
 *
 * Purpose: SPI Driver
 *
 **********************************************************************************/
#if defined(__MKL26Z64__)

#include "SnoozeSPI.h"
#define INTERNAL_SPI_CLK_PIN 60
/*******************************************************************************
 *
 *******************************************************************************/
void SnoozeSPI::setClockPin( uint8_t pin ) {
    if ( pin == BUILTIN_SD_CLK_PIN ) {
        clk_pin = INTERNAL_SPI_CLK_PIN;
    } else {
        clk_pin = pin;
    }
}
/*******************************************************************************
 *  
 *******************************************************************************/
void SnoozeSPI::disableDriver( uint8_t mode ) {
    *portModeRegister( clk_pin ) = 0;
    volatile uint32_t *config;
    config = portConfigRegister( clk_pin );
    *config = return_core_pin_config;
}
/*******************************************************************************
 *  SPI clk -> output low
 *******************************************************************************/
void SnoozeSPI::enableDriver( uint8_t mode ) {
    volatile uint32_t *config;
    config = portConfigRegister( clk_pin );
    return_core_pin_config = *config;
    *portModeRegister( clk_pin ) |= digitalPinToBitMask( clk_pin ); // TODO: atomic
    *config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX( 1 );
    *config &= ~PORT_PCR_ODE;
}
#endif /* __MKL26Z64__ */
