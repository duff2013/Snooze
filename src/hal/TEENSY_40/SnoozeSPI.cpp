/***********************************************************************************
 *  SnoozeSPI.h
 *  Teensy 4.0
 *
 *  Purpose: SPI Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#include "SnoozeSPI.h"
#define INTERNAL_SPI_CLK_PIN 60

/*******************************************************************************
 *
 *******************************************************************************/
void SnoozeSPI::setSPIClass( SPIClass *p ) {
    if ( p == &SPI ) {
        port_addr = ( uintptr_t )&IMXRT_LPSPI4_S;
    }
    else if ( p == &SPI1 ) {
        port_addr = ( uintptr_t )&IMXRT_LPSPI3_S;
    }
    else if ( p == &SPI2 ) {
        port_addr = ( uintptr_t )&IMXRT_LPSPI1_S;
    }
}
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
    port( ).CR = LPSPI_CR_MEN;
    
    uint32_t fast_io = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_SPEED(2);
    *(portControlRegister(11)) = fast_io;
    *(portControlRegister(12)) = fast_io;
    *(portControlRegister(13)) = fast_io;
    
    *(portConfigRegister(11)) = 0x03 | 0x10;
    *(portConfigRegister(12)) = 0x03 | 0x10;
    *(portConfigRegister(13)) = 0x03 | 0x10;
    //LPSPI4_CR = 0;
    //*portModeRegister( clk_pin ) = 0;
    //volatile uint32_t *config;
    //config = portConfigRegister( clk_pin );
    //*config = return_core_pin_config;
}
/*******************************************************************************
 *  SPI clk -> output low
 *******************************************************************************/
void SnoozeSPI::enableDriver( uint8_t mode ) {
    port( ).CR = 0;
    uint32_t disabled_io = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_HYS;
    uint32_t sck_io = IOMUXC_PAD_DSE(7);
    *(portControlRegister(11)) = disabled_io;
    *(portControlRegister(12)) = disabled_io;
    *(portControlRegister(13)) = sck_io;
    
    *(portConfigRegister(11)) = 0x05;
    *(portConfigRegister(12)) = 0x05;
    *(portConfigRegister(13)) = 0x05 | 0x10;
    //LPSPI4_CR = 0;
    //volatile uint32_t *config;
    //config = portConfigRegister( clk_pin );
    //return_core_pin_config = *config;
    //*portModeRegister( clk_pin ) = 1;
    //*config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX( 1 );
    //*config &= ~PORT_PCR_ODE;
}
#endif /* __IMXRT1062__ */
