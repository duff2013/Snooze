/*******************************************************************************
 *  mcg.c
 *  Teensy3
 *
 * Purpose:     Provides routines to transition from BLPI->PEE and PEE->BLPI.
 *              In BLPI the mcu's SIM_CLKDIV1 will be configured as core(2MHZ), 
 *              bus(2MHZ), flash(1MHZ) and the SysTick will be reconfigured to work 
 *              as expected in this mode. This allows the mcu to enter vlpr run
 *              mode. When exiting vlpr the mcu will transition back to PEE mode
 *              and the mcu will be in a normal run state.
 *
 * NOTE:        Moving from PEE to BLPI cause's the USB module not to work so it
 *              must be disabled before moving into this mode.
 *******************************************************************************/

#include "mcg.h"
#include "bitband.h"
#include "Arduino.h"

#define MCG_C1_FRDIV4_BIT   0x05

void pee_blpe( void ) {
    if ( mcg_mode( ) != PEE ) return;
    // first move from PEE to PBE
    MCG_C1 |= MCG_C1_CLKS( 0x02 ); // select external reference clock as MCG_OUT
    // Wait for clock status bits to update indicating clock has switched
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x02 ) ) ;
    MCG_C2 |= MCG_C2_LP; // set the LP bit to enter BLPE
}

void blpe_pee( void ) {
    if ( mcg_mode( ) != BLPE ) return;
    MCG_C5 = MCG_C5_PRDIV0( 0x03 );
    MCG_C6 =  MCG_C6_PLLS | MCG_C6_VDIV0( 0x00 ); // update MCG_C6
    // Now that PLL is configured, LP is cleared to enable the PLL
    MCG_C2 &= ~MCG_C2_LP_MASK;
    // wait for PLLST status bit to set
    while ( !(MCG_S & MCG_S_PLLST) ) ;
    while ( !(MCG_S & MCG_S_LOCK0) ) ;
    // configure the clock dividers back again before switching to the PLL to
    // ensure the system clock speeds are in spec.
#if F_CPU == 168000000
    // config divisors: 168 MHz core, 56 MHz bus, 33.6 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 144000000
    // config divisors: 144 MHz core, 48 MHz bus, 28.8 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) |	SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 120000000
    // config divisors: 120 MHz core, 60 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 96000000
    // config divisors: 96 MHz core, 48 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |	SIM_CLKDIV1_OUTDIV4( 3 );
#elif F_CPU == 72000000
    // config divisors: 72 MHz core, 36 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 2 );
#elif F_CPU == 48000000
    // config divisors: 48 MHz core, 48 MHz bus, 24 MHz flash
    #if defined(KINETISK)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |	SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined(KINETISL)
    // config divisors: 48 MHz core, 24 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV4( 1 );
    #endif
#elif F_CPU == 24000000
    // config divisors: 24 MHz core, 24 MHz bus, 24 MHz flash
    #if defined(KINETISK)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV2( 3 ) |	SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined(KINETISL)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV4( 0 );
    #endif
//#error "Error, F_CPU must be 96000000, 48000000, or 24000000"
#endif
    // now in PEE
    MCG_C1 = MCG_C1_CLKS( 0 );
    SYST_RVR = ( F_CPU / 1000 ) - 1;
}
    
void pee_blpi( void ) {
    if ( mcg_mode( ) != PEE ) return;
    /* Moving from PEE to BLPI */
    // first move from PEE to PBE
    MCG_C1 |= MCG_C1_CLKS( 0x02 ); // select external reference clock as MCG_OUT
    // Wait for clock status bits to update indicating clock has switched
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 2 ) ) ;
    // now move to FBE mode
    // make sure the FRDIV is configured to keep the FLL reference within spec.
    MCG_C1 &= ~MCG_C1_FRDIV_MASK; // clear FRDIV field
    MCG_C1 |= MCG_C1_FRDIV( 0x04 ); // set FLL ref divider to 512
    MCG_C6 &= ~MCG_C6_PLLS; // clear PLLS to select the FLL
    while ( MCG_S & MCG_S_PLLST ) ; // Wait for PLLST status bit to clear to
    // indicate switch to FLL output
    // now move to FBI mode
    MCG_C2 |= MCG_C2_IRCS; // set the IRCS bit to select the fast IRC
    // set CLKS to 1 to select the internal reference clock
    // keep FRDIV at existing value to keep FLL ref clock in spec.
    // set IREFS to 1 to select internal reference clock
    MCG_C1 = MCG_C1_CLKS( 0x01 ) | MCG_C1_FRDIV( 0x04 ) | MCG_C1_IREFS;
    // wait for internal reference to be selected
    while ( !( MCG_S & MCG_S_IREFST ) ) ;
    // wait for fast internal reference to be selected
    while ( !( MCG_S & MCG_S_IRCST ) ) ;
    // wait for clock to switch to IRC
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 1 ) ) ;
    // now move to BLPI
    MCG_C2 |= MCG_C2_LP; // set the LP bit to enter BLPI
    mcg_div(0x00, 0x00, 0x01, 1999);
}

void blpi_pee( void ) {
    if ( mcg_mode( ) != BLPI ) return;
    /* Moving from BLPI to PEE */
    // first move to FBI
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPI
    // move to FBE
    // clear IREFS to select the external ref clock
    // set CLKS = 2 to select the ext ref clock as clk source
    // it is assumed the oscillator parameters in MCG_C2 have not been changed
    MCG_C1 = MCG_C1_CLKS( 0x02 ) | MCG_C1_FRDIV( 0x04 );
    // wait for the oscillator to initialize again
    while ( !( MCG_S & MCG_S_OSCINIT0 ) ) ;
    // wait for Reference clock to switch to external reference
    while ( MCG_S & MCG_S_IREFST ) ;
    // wait for MCGOUT to switch over to the external reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x02 ) ) ;
    // configure PLL and system clock dividers
    // if we need faster than the crystal, turn on the PLL
#if F_CPU == 168000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 18 ); // config PLL for 168 MHz output
#elif F_CPU == 144000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 12 ); // config PLL for 144 MHz output
#elif F_CPU == 120000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 6 ); // config PLL for 120 MHz output
#elif F_CPU == 72000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 3 ); // config PLL for 72 MHz output
#else
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 0 ); // config PLL for 96 MHz output
#endif
    while ( !(MCG_S & MCG_S_PLLST) ) ;
    while ( !(MCG_S & MCG_S_LOCK0) ) ;
    // configure the clock dividers back again before switching to the PLL to
    // ensure the system clock speeds are in spec.
#if F_CPU == 168000000
    // config divisors: 168 MHz core, 56 MHz bus, 33.6 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) |	SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 144000000
    // config divisors: 144 MHz core, 48 MHz bus, 28.8 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 120000000
    // config divisors: 120 MHz core, 60 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |	SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 96000000
    // config divisors: 96 MHz core, 48 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#elif F_CPU == 72000000
    // config divisors: 72 MHz core, 36 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |	SIM_CLKDIV1_OUTDIV4( 2 );
#elif F_CPU == 48000000
    // config divisors: 48 MHz core, 48 MHz bus, 24 MHz flash
    #if defined(KINETISK)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |	SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined(KINETISL)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV4( 1 );
    #endif
#elif F_CPU == 24000000
    // config divisors: 24 MHz core, 24 MHz bus, 24 MHz flash
    #if defined( KINETISK )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV2( 3 ) |	SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined( KINETISL )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV4( 0 );
    #endif
#else
//#error "Error, F_CPU must be 168, 144, 120, 96, 72, 48, 24, 16, 8, 4, or 2 MHz"
#endif
    // switch to PLL as clock source, FLL input = 16 MHz / 512
    MCG_C1 = MCG_C1_CLKS( 0x00 ) | MCG_C1_FRDIV( 0x04 );
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x03 ) ) ;
    // reinitialize the SysTick counter
    SYST_RVR = ( F_CPU / 1000 ) - 1;
}

void blpi_blpe( void ) {
    if ( mcg_mode( ) != BLPI ) return;
    /* Moving from BLPI to BLPE */
    // first move to FBI
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPI
    // enable capacitors for crystal
    OSC0_CR = OSC_SC8P | OSC_SC2P;
    // enable osc, 8-32 MHz range, low power mode
    MCG_C2 = MCG_C2_RANGE0( 2 ) | MCG_C2_EREFS;
    // move to FBE
    // clear IREFS to select the external ref clock
    // set CLKS = 2 to select the ext ref clock as clk source
    // it is assumed the oscillator parameters in MCG_C2 have not been changed
    MCG_C1 = MCG_C1_CLKS( 0x02 ) | MCG_C1_FRDIV( 0x04 );
    
    // wait for the oscillator to initialize again
    while ( !( MCG_S & MCG_S_OSCINIT0 ) ) ;
    
    // wait for Reference clock to switch to external reference
    while ( MCG_S & MCG_S_IREFST ) ;
    
    // wait for MCGOUT to switch over to the external reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x02 ) ) ;
    // To move from FBE to BLPE the LP bit must be set
    MCG_C2 |= MCG_C2_LP; // set LP bit
    
    const uint32_t div = ( 16/( F_CPU/1000000 ) )-1;
    const uint32_t systck = F_CPU - 1;
    mcg_div( div, div, div, systck );
}

void blpe_blpi( void ) {
    if ( mcg_mode( ) != BLPE ) return;
    /* Moving from BLPE to BLPI */
    // first move to FBE
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPE
    // move to FBE
    // set CLKS = 1 to select the internal ref clock as clk source
    MCG_C1 = MCG_C1_CLKS( 0x01 ) | MCG_C1_FRDIV( 0x04 ) | MCG_C1_IREFS;
    // wait for Reference clock to switch to internal reference
    while ( MCG_S & MCG_S_IREFST ) ;
    // wait for MCGOUT to switch over to the internal reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x01 ) ) ;
    MCG_C2 = MCG_C2_IRCS;
    while ( !(MCG_S & MCG_S_IRCST ) ) ;
    // To move from FBI to BLPI the LP bit must be set
    MCG_C2 |= MCG_C2_LP; // set LP bit
    mcg_div( 0x00, 0x00, 0x01, 1999 );
}

void pbe_pee( void ) {
    // Check MCG is in PBE mode
    if ( mcg_mode( ) != PBE ) return;
    // we're in PBE mode
    // switch to PLL as clock source, FLL input = 16 MHz / 512
    MCG_C1 = 0x00;
    MCG_C1 = MCG_C1_CLKS( 0 ) | MCG_C1_FRDIV( 4 );
    // wait for PLL clock to be used
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x03 ) ) ;
}

CLOCK_MODE mcg_mode( void ) {
    // check if in FEI mode
    if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x00 ) &&     // check CLKS mux has selcted FLL output
        ( MCG_S & MCG_S_IREFST_MASK ) &&                                     // check FLL ref is internal ref clk
        ( !(  MCG_S & MCG_S_PLLST_MASK) ) )                                     // check PLLS mux has selected FLL
    {
        return FEI;                                                          // return FEI code
    }
    // Check MCG is in PEE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x03 ) && // check CLKS mux has selcted PLL output
             ( !( MCG_S & MCG_S_IREFST_MASK ) ) &&                              // check FLL ref is external ref clk
             ( MCG_S & MCG_S_PLLST_MASK ) )                                    // check PLLS mux has selected PLL
    {
        return PEE;                                                          // return PEE code
    }
    // Check MCG is in PBE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x02 ) && // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST_MASK ) ) &&                              // check FLL ref is external ref clk
             ( MCG_S & MCG_S_PLLST_MASK ) &&                                  // check PLLS mux has selected PLL
             ( !( MCG_C2 & MCG_C2_LP_MASK ) ) )                                  // check MCG_C2[LP] bit is not set
    {
        return PBE;                                                          // return PBE code
    }
    // Check MCG is in FBE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT ) == 0x02 ) && // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST_MASK ) ) &&                              // check FLL ref is external ref clk
             ( !( MCG_S & MCG_S_PLLST_MASK ) ) &&                               // check PLLS mux has selected FLL
             ( !( MCG_C2 & MCG_C2_LP_MASK ) ) )                                  // check MCG_C2[LP] bit is not set
    {
        return FBE;                                                          // return FBE code
    }
    // Check MCG is in BLPE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x02 ) && // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST_MASK ) ) &&                              // check FLL ref is external ref clk
             ( MCG_C2 & MCG_C2_LP_MASK ) )                                     // check MCG_C2[LP] bit is set
    {
        return BLPE;                                                         // return BLPE code
    }
    // check if in BLPI mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x01 ) && // check CLKS mux has selcted int ref clk
             ( MCG_S & MCG_S_IREFST_MASK ) &&                                 // check FLL ref is internal ref clk
             ( !( MCG_S & MCG_S_PLLST_MASK ) ) &&                               // check PLLS mux has selected FLL
             ( MCG_C2 & MCG_C2_LP_MASK ) )                                     // check LP bit is set
    {
        return BLPI;                                                         // return BLPI code
    }
    // check if in FBI mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x01 ) && // check CLKS mux has selcted int ref clk
             ( MCG_S & MCG_S_IREFST_MASK ) &&                                 // check FLL ref is internal ref clk
             ( !( MCG_S & MCG_S_PLLST_MASK ) ) &&                               // check PLLS mux has selected FLL
             ( !( MCG_C2 & MCG_C2_LP_MASK ) ) )                                  // check LP bit is clear
    {
        return FBI;                                                          // return FBI code
    }
    // Check MCG is in FEE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x00 ) && // check CLKS mux has selcted FLL
             ( !( MCG_S & MCG_S_IREFST_MASK ) ) &&                              // check FLL ref is external ref clk
             ( !( MCG_S & MCG_S_PLLST_MASK ) ) )                                 // check PLLS mux has selected FLL
    {
        return FEE;                                                          // return FEE code
    }
    else
    {
        return 0;                                                            // error condition
    }
}

void mcg_div( uint8_t cpu_div, uint8_t bus_div, uint8_t mem_div, uint32_t syst ) {
    // config divisors: cpu_div, bus_div, mem_div
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( cpu_div ) | SIM_CLKDIV1_OUTDIV2( bus_div ) |	 SIM_CLKDIV1_OUTDIV4( mem_div );
    SYST_RVR = syst;
}


