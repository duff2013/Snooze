/*******************************************************************************
 Low Power Library for Teensy LC/3.x
 * Copyright (c) 2014, Colin Duffy https://github.com/duff2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ********************************************************************************
 *  mcg.c
 *  Teensy3
 *
 * Purpose:     Provides routines to transition from PEE->BLPI, BLPI->PEE, BLPE->PEE,
 *              PEE->BLPE, BLPI->BLPE, BLPE->BLPI and PBE->PEE.
 *
 *              In BLPI the mcu's SIM_CLKDIV1 will be configured as core(2MHZ),
 *              bus(2MHZ), flash(1MHZ) and the SysTick will be reconfigured to work
 *              as expected in this mode. This allows the mcu to enter vlpr mode.
 *              When exiting vlpr the mcu will transition back to PEE or BLPE mode and
 *              the mcu will be in a normal run state.
 *
 * NOTE:        Moving from PEE to BLPI cause's the USB module not to work so it
 *              must be disabled before moving into this mode.
 *******************************************************************************/

#ifndef systemClock_h
#define systemClock_h

/********************************************************************/
// MCG_S Bit Fields
#define MCG_S_CLKST_SHIFT    ( uint8_t )0x02
#define LPTMR_MCGIRCLK       ( uint8_t )0x00
#define LPTMR_LPO            ( uint8_t )0x01
#define LPTMR_ERCLK32K       ( uint8_t )0x02
#define LPTMR_OSCERCLK       ( uint8_t )0x03
#define MCG_C1_FRDIV4_BIT    ( uint8_t )0x05
/********************************************************************/

typedef enum {
    UNDEFINED   = 0,
    BLPI        = 1,
    FBI         = 2,
    FEI         = 3,
    FEE         = 4,
    FBE         = 5,
    BLPE        = 6,
    PBE         = 7,
    PEE         = 8
} CLOCK_MODE;

/* CPU SPeeds */
typedef enum {
    TWO_MHZ                 = 2000000,
    FOUR_MHZ                = 4000000,
    EIGHT_MHZ               = 8000000,
    SIXTEEN_MHZ             = 16000000,
    TWENTY_FOUR_MHZ         = 24000000,
    FORTY_EIGHT_MHZ         = 48000000,
    SEVENTY_TWO_MHZ         = 72000000,
    NINETY_SIX_MHZ          = 96000000,
    HUNDRED_TWENTY_MHZ      = 120000000,
    HUNDRED_FORTY_FOUR_MHZ  = 144000000,
    HUNDRED_SIXTY_EIGHT_MHZ = 168000000,
    HUNDRED_NINETY_TWO_MHZ  = 192000000,
} SPEED;

static inline CLOCK_MODE mcg_mode( void );
/**
 *  Use lptmr as the systick counter while we switch clock domians.
 *  Systick is disabled.
 */
static inline
void start_lptmr_systick( void )
__attribute__((always_inline, unused));

static inline
void start_lptmr_systick( void ) {
#if defined(KINETISK)
    if ( !( OSC0_CR & OSC_ERCLKEN ) ) {
        OSC0_CR |= OSC_ERCLKEN;
        while ( ( MCG_S & MCG_S_OSCINIT0 ) == 0 );
    }
#endif
    // start low power timer clock
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    //clear previous config
    LPTMR0_CSR = 0;
    // Use external ref clock (32.768 KHz), no prescale
    LPTMR0_PSR = LPTMR_PSR_PCS( LPTMR_OSCERCLK ) | LPTMR_PSR_PRESCALE(3);
    // Use timer compare flag
    LPTMR0_CSR = LPTMR_CSR_TCF;
    LPTMR0_CMR = 999;
    // read countflag
    uint32_t flag = SYST_CSR & SYST_CSR_COUNTFLAG;
    // suppress complier warning
    flag++;
    // wait for systick countflag to deassert
    while ( !( SYST_CSR & SYST_CSR_COUNTFLAG ) );
    // disable systick
    SYST_CSR = 0;
    // start low power timer, no interrupt, free running
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TFC;
}
/**
 *  Use lptmr as the systick counter while we switch clock domians.
 *  Systick is renabled after lptmr runs for 1 millisecond. Lptmr was
 *  started in start_lptmr_systick function.
 */
static inline
void stop_lptmr_systick( uint32_t reload )
__attribute__((always_inline, unused));

static inline
void stop_lptmr_systick( uint32_t reload ) {
    // sync for systick, low power timer waits here until 1 millisecond has elapsed
    while ( !( LPTMR0_CSR & LPTMR_CSR_TCF ) );
    // disable low power timer
    LPTMR0_CSR = LPTMR_CSR_TCF;
    SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#if defined(KINETISK)
    OSC0_CR &= ~OSC_ERCLKEN;
#endif
    // since systick isr was disabled manually fire interrupt to update count
    SCB_ICSR |= SCB_ICSR_PENDSTSET;
    // enable systick
    SYST_RVR = reload;
    SYST_CVR = 0;
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}
/**
 *  Transistion from clock domian pee -> blpe
 */
static inline
void pee_blpe( void )
__attribute__((always_inline, unused));

static inline
void pee_blpe( void ) {
    if ( mcg_mode( ) != PEE ) return;
    /* Moving from PEE to BLPE */
    // first move from PEE to PBE
    // select external reference clock as MCG_OUT
    // now move to PBE mode
    // make sure the FRDIV is configured to keep the FLL reference within spec.
    MCG_C1 = MCG_C1_CLKS( 0x02 ) | MCG_C1_FRDIV( 0x04 );
    // wait for the oscillator to initialize again
    while ( !( MCG_S & MCG_S_OSCINIT0 ) ) ;
    // wait for Reference clock to switch to external reference
    while ( MCG_S & MCG_S_IREFST ) ;
    // wait for MCGOUT to switch over to the external reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x02 ) ) ;
    // To move from PBE to BLPE the LP bit must be set
    MCG_C2 |= MCG_C2_LP; // set LP bit
    // TODO: Is this right?
    const uint32_t div = ( 16 / ( F_CPU / 1000000 ) ) - 1;
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( div ) | SIM_CLKDIV1_OUTDIV2( div ) |	 SIM_CLKDIV1_OUTDIV3( div ) | SIM_CLKDIV1_OUTDIV4( div );
}
/**
 *  Transistion from clock domian blpe -> pee
 */
static inline
void blpe_pee( void )
__attribute__((always_inline, unused));

static inline
void blpe_pee( void ) {
    /* Moving from BLPE to PEE */
    // first move to PBE
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPE
    // move to PEE
    // configure PLL and system clock dividers
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    SMC_PMCTRL = SMC_PMCTRL_RUNM(3); // enter HSRUN mode
    while (SMC_PMSTAT != SMC_PMSTAT_HSRUN) ; // wait for HSRUN
#endif
#if F_CPU == 256000000
    //See table in 27.4.6 MCG Control 6 Register (MCG_C6)
    //16 -> Multiply factor 32. 32*8MHz =256MHz
    MCG_C5 = MCG_C5_PRDIV0(0);
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0(16);
#elif F_CPU == 240000000
    MCG_C5 = MCG_C5_PRDIV0( 0 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 14 );
#elif F_CPU == 216000000
    MCG_C5 = MCG_C5_PRDIV0( 0 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 11 );
#elif F_CPU == 192000000
    MCG_C5 = MCG_C5_PRDIV0( 0 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 8 );
#elif F_CPU == 180000000
    MCG_C5 = MCG_C5_PRDIV0( 1 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 29 );
#elif F_CPU == 168000000
    MCG_C5 = MCG_C5_PRDIV0( 0 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 5 );
#elif F_CPU == 144000000
    MCG_C5 = MCG_C5_PRDIV0( 0 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 2 );
#elif F_CPU == 120000000
    MCG_C5 = MCG_C5_PRDIV0( 1 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 14 );
#elif F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
    MCG_C5 = MCG_C5_PRDIV0( 1 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 8 );
#elif F_CPU == 72000000
    MCG_C5 = MCG_C5_PRDIV0( 1 );
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 2 );
#endif
#else
#if F_CPU == 72000000
    MCG_C5 = MCG_C5_PRDIV0( 5 );		 // config PLL input for 16 MHz Crystal / 6 = 2.667 Hz
#else
    MCG_C5 = MCG_C5_PRDIV0( 3 );		 // config PLL input for 16 MHz Crystal / 4 = 4 MHz
#endif
#if F_CPU == 168000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 18 ); // config PLL for 168 MHz output
#elif F_CPU == 144000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 12 ); // config PLL for 144 MHz output
#elif F_CPU == 120000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 6 ); // config PLL for 120 MHz output
#elif F_CPU == 72000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 3 ); // config PLL for 72 MHz output
#elif F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
    MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 0 ); // config PLL for 96 MHz output
#endif
#endif
    while ( !(MCG_S & MCG_S_PLLST) ) ;
    while ( !(MCG_S & MCG_S_LOCK0) ) ;
    // configure the clock dividers back again before switching to the PLL to
    // ensure the system clock speeds are in spec.
#if F_CPU == 256000000
    // config divisors: 256 MHz core, 64 MHz bus, 32 MHz flash, USB = IRC48M
    // TODO: gradual ramp-up for HSRUN mode
#if F_BUS == 64000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(3) | SIM_CLKDIV1_OUTDIV4(7);
#elif F_BUS == 128000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) | SIM_CLKDIV1_OUTDIV4(7);
#else
#error "This F_CPU & F_BUS combination is not supported"
#endif
    SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);
#elif F_CPU == 240000000
#if F_BUS == 60000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#elif F_BUS == 80000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#elif F_BUS == 120000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#endif
#elif F_CPU == 216000000
#if F_BUS == 54000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#elif F_BUS == 72000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#elif F_BUS == 108000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 7 );
#endif
#elif F_CPU == 192000000
#if F_BUS == 48000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 6 );
#elif F_BUS == 64000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 6 );
#elif F_BUS == 96000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 6 );
#endif
#elif F_CPU == 180000000
    // config divisors: 180 MHz core, 60 MHz bus, 25.7 MHz flash, USB = IRC48M
#if F_BUS == 60000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 6 );
#elif F_BUS == 90000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 6 );
#endif
#elif F_CPU == 168000000
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV3( 1 ) |  SIM_CLKDIV1_OUTDIV4( 3 );
#elif defined(KINETISL)
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV4( 1 );
#endif
#elif F_CPU == 24000000
    // config divisors: 24 MHz core, 24 MHz bus, 24 MHz flash
#if defined( KINETISK )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV3( 3 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#elif defined( KINETISL )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV4( 0 );
#endif
#endif
    // switch to PLL as clock source, FLL input = 16 MHz / 512
    MCG_C1 = MCG_C1_CLKS( 0x00 ) | MCG_C1_FRDIV( 0x04 );
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x03 ) ) ;
}
/**
 *  Transistion from clock domian pee -> blpi
 */
static inline
void pee_blpi( void )
__attribute__((always_inline, unused));

static inline
void pee_blpi( void ) {
    if ( mcg_mode( ) != PEE ) return;
    MCG_SC = MCG_SC_FCRDIV( 0 );
    /* Moving from PEE to BLPI */
    // first move from PEE to PBE
    // select external reference clock as MCG_OUT
    // now move to FBE mode
    // make sure the FRDIV is configured to keep the FLL reference within spec.
    MCG_C1 = MCG_C1_CLKS( 0x02 ) | MCG_C1_FRDIV( 0x04 );
    // Wait for clock status bits to update indicating clock has switched
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 2 ) ) ;
    //MCG_C1 &= ~MCG_C1_FRDIV_MASK; // clear FRDIV field
    //MCG_C1 |= MCG_C1_FRDIV( 0x04 ); // set FLL ref divider to 512
    MCG_C6 &= ~MCG_C6_PLLS; // clear PLLS to select the FLL
    while ( MCG_S & MCG_S_PLLST ) ; // Wait for PLLST status bit to clear to
    // indicate switch to FLL output
    // now move to FBI mode
    MCG_C2 |= MCG_C2_IRCS;// | 0x40; // set the IRCS bit to select the fast IRC
#if defined( __MK64FX512__ )
    MCG_C4 = MCG_C4_FCTRIM( 10 );
#endif
    // set CLKS to 1 to select the internal reference clock
    // keep FRDIV at existing value to keep FLL ref clock in spec.
    // set IREFS to 1 to select internal reference clock
#if defined( KINETISL )
    MCG_C1 = MCG_C1_CLKS( 0x01 ) | MCG_C1_FRDIV( 0x04 ) | MCG_C1_IREFS | MCG_C1_IREFSTEN | MCG_C1_IRCLKEN;
#else
    MCG_C1 = MCG_C1_CLKS( 0x01 ) | MCG_C1_FRDIV( 0x04 ) | MCG_C1_IREFS | MCG_C1_IREFSTEN;
#endif
    // wait for internal reference to be selected
    while ( !( MCG_S & MCG_S_IREFST ) ) ;
    // wait for fast internal reference to be selected
    while ( !( MCG_S & MCG_S_IRCST ) ) ;
    // wait for clock to switch to IRC
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 1 ) ) ;
    // now move to BLPI
    MCG_C2 |= MCG_C2_LP; // set the LP bit to enter BLPI
    // config divisors: 2 MHz, 2 MHz, 1 MHz
#if defined( KINETISK )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV3( 1 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#elif defined( KINETISL )
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#endif
}
/**
 *  Transistion from clock domian blpi -> pee
 */
static inline
void blpi_pee( void )
__attribute__((always_inline, unused));

static inline
void blpi_pee( void ) {
    /* Moving from BLPI to PEE */
    // first move to FBI
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPI
    // move to FBE
    // clear IREFS to select the external ref clock
    // set CLKS = 2 to select the ext ref clock as clk source
    // it is assumed the oscillator parameters in MCG_C2 have not been changed
    MCG_C1 = MCG_C1_CLKS( 0x02 ) | MCG_C1_FRDIV( 0x04 );
    // wait for the oscillator to initialize again
    while ( !( MCG_S & MCG_S_OSCINIT0 ) );
    // wait for Reference clock to switch to external reference
    while ( MCG_S & MCG_S_IREFST ) ;
    // wait for MCGOUT to switch over to the external reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x02 ) ) ;
    // configure PLL and system clock dividers
#if defined(__MK66FX1M0__)
    #if F_CPU > 120000000
        SMC_PMCTRL = SMC_PMCTRL_RUNM(3); // enter HSRUN mode
        while (SMC_PMSTAT != SMC_PMSTAT_HSRUN) ; // wait for HSRUN
    #endif
    #if F_CPU == 256000000
        //See table in 27.4.6 MCG Control 6 Register (MCG_C6)
        //16 -> Multiply factor 32. 32*8MHz =256MHz
        MCG_C5 = MCG_C5_PRDIV0(0);
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0(16);
    #elif F_CPU == 240000000
        MCG_C5 = MCG_C5_PRDIV0( 0 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 14 );
    #elif F_CPU == 216000000
        MCG_C5 = MCG_C5_PRDIV0( 0 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 11 );
    #elif F_CPU == 192000000
        MCG_C5 = MCG_C5_PRDIV0( 0 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 8 );
    #elif F_CPU == 180000000
        MCG_C5 = MCG_C5_PRDIV0( 1 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 29 );
    #elif F_CPU == 168000000
        MCG_C5 = MCG_C5_PRDIV0( 0 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 5 );
    #elif F_CPU == 144000000
        MCG_C5 = MCG_C5_PRDIV0( 0 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 2 );
    #elif F_CPU == 120000000
        MCG_C5 = MCG_C5_PRDIV0( 1 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 14 );
    #elif F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
        MCG_C5 = MCG_C5_PRDIV0( 1 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 8 );
    #elif F_CPU == 72000000
        MCG_C5 = MCG_C5_PRDIV0( 1 );
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 2 );
    #endif
#else
    #if F_CPU == 72000000
        MCG_C5 = MCG_C5_PRDIV0( 5 );		 // config PLL input for 16 MHz Crystal / 6 = 2.667 Hz
    #else
        MCG_C5 = MCG_C5_PRDIV0( 3 );		 // config PLL input for 16 MHz Crystal / 4 = 4 MHz
    #endif
    #if F_CPU == 168000000
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 18 ); // config PLL for 168 MHz output
    #elif F_CPU == 144000000
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 12 ); // config PLL for 144 MHz output
    #elif F_CPU == 120000000
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 6 ); // config PLL for 120 MHz output
    #elif F_CPU == 72000000
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 3 ); // config PLL for 72 MHz output
    #elif F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
        MCG_C6 = MCG_C6_PLLS | MCG_C6_VDIV0( 0 ); // config PLL for 96 MHz output
    #endif
#endif
    while ( !(MCG_S & MCG_S_PLLST) ) ;
    while ( !(MCG_S & MCG_S_LOCK0) ) ;
    // configure the clock dividers back again before switching to the PLL to
    // ensure the system clock speeds are in spec.
#if F_CPU == 256000000
    // config divisors: 256 MHz core, 64 MHz bus, 32 MHz flash, USB = IRC48M
    // TODO: gradual ramp-up for HSRUN mode
#if F_BUS == 64000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(3) | SIM_CLKDIV1_OUTDIV4(7);
#elif F_BUS == 128000000
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) | SIM_CLKDIV1_OUTDIV4(7);
#else
#error "This F_CPU & F_BUS combination is not supported"
#endif
    SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);
#elif F_CPU == 240000000
    #if F_BUS == 60000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #elif F_BUS == 80000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #elif F_BUS == 120000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #endif
#elif F_CPU == 216000000
    #if F_BUS == 54000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #elif F_BUS == 72000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #elif F_BUS == 108000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 7 );
    #endif
#elif F_CPU == 192000000
    #if F_BUS == 48000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV4( 6 );
    #elif F_BUS == 64000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 6 );
    #elif F_BUS == 96000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 6 );
    #endif
#elif F_CPU == 180000000
    // config divisors: 180 MHz core, 60 MHz bus, 25.7 MHz flash, USB = IRC48M
    #if F_BUS == 60000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 6 );
    #elif F_BUS == 90000000
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 6 );
    #endif
#elif F_CPU == 168000000
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
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV3( 1 ) |  SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined(KINETISL)
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 1 ) | SIM_CLKDIV1_OUTDIV4( 1 );
    #endif
#elif F_CPU == 24000000
    // config divisors: 24 MHz core, 24 MHz bus, 24 MHz flash
    #if defined( KINETISK )
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV2( 3 ) | SIM_CLKDIV1_OUTDIV3( 3 ) | SIM_CLKDIV1_OUTDIV4( 3 );
    #elif defined( KINETISL )
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 3 ) | SIM_CLKDIV1_OUTDIV4( 0 );
    #endif
#endif
    // switch to PLL as clock source, FLL input = 16 MHz / 512
    MCG_C1 = MCG_C1_CLKS( 0x00 ) | MCG_C1_FRDIV( 0x04 );
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x03 ) ) ;
}
/**
 *  Transistion from clock domian blpi -> blpe
 */
static inline
void blpi_blpe( void )
__attribute__((always_inline, unused));

static inline
void blpi_blpe( void ) {
    /* Moving from BLPI to BLPE */
    // first move to FBI
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPI
    // enable capacitors for crystal
    OSC0_CR |= OSC_SC8P | OSC_SC2P;
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
    // TODO: Is this right?
    const uint32_t div = ( 16 / ( F_CPU / 1000000 ) ) - 1;
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( div ) | SIM_CLKDIV1_OUTDIV2( div ) |	 SIM_CLKDIV1_OUTDIV3( div ) |SIM_CLKDIV1_OUTDIV4( div );
}
/**
 *  Transistion from clock domian blpe -> blpi
 */
static inline
void blpe_blpi( void )
__attribute__((always_inline, unused));

static inline
void blpe_blpi( void ) {
    /* Moving from BLPE to BLPI */
    // first move to FBE
    //MCG_C2
    MCG_C2 &= ~MCG_C2_LP; // clear the LP bit to exit BLPE
    // move to FBE
    // set CLKS = 1 to select the internal ref clock as clk source
    MCG_C1 = MCG_C1_CLKS( 0x01 ) | MCG_C1_FRDIV( 0x04 ) | MCG_C1_IREFS;
    // wait for Reference clock to switch to internal reference
    //while ( MCG_S & MCG_S_IREFST ) ;
    // wait for MCGOUT to switch over to the internal reference clock
    while ( ( MCG_S & MCG_S_CLKST_MASK ) != MCG_S_CLKST( 0x01 ) ) ;
    MCG_C2 |= MCG_C2_IRCS;
    while ( !( MCG_S & MCG_S_IRCST ) ) ;
    // To move from FBI to BLPI the LP bit must be set
    MCG_C2 |= MCG_C2_LP; // set LP bit
    // config divisors: 2MHz, 2MHz, 1MHz
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 0 ) |	 SIM_CLKDIV1_OUTDIV3( 0 ) |SIM_CLKDIV1_OUTDIV4( 1 );
}

/**
 *  Transistion from clock domian pbe -> pee
 */
static inline
void pbe_pee( void )
__attribute__((always_inline, unused));

static inline
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

/**
 *  Get current mode
 *
 *  @return clock mode
 */
static inline
CLOCK_MODE mcg_mode( void )
__attribute__((always_inline, unused));

static inline
CLOCK_MODE mcg_mode( void ) {
    // check if in FEI mode
    if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x00 ) &&   // check CLKS mux has selcted FLL output
        ( MCG_S & MCG_S_IREFST ) &&                                             // check FLL ref is internal ref clk
        ( !(  MCG_S & MCG_S_PLLST ) ) )                                         // check PLLS mux has selected FLL
    {
        return FEI;                                                             // return FEI code
    }
    // Check MCG is in PEE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x03 ) &&  // check CLKS mux has selcted PLL output
             ( !( MCG_S & MCG_S_IREFST ) ) &&                                       // check FLL ref is external ref clk
             ( MCG_S & MCG_S_PLLST ) )                                              // check PLLS mux has selected PLL
    {
        return PEE;                                                                 // return PEE code
    }
    // Check MCG is in PBE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x02 ) &&  // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST ) ) &&                                       // check FLL ref is external ref clk
             ( MCG_S & MCG_S_PLLST ) &&                                             // check PLLS mux has selected PLL
             ( !( MCG_C2 & MCG_C2_LP ) ) )                                          // check MCG_C2[LP] bit is not set
    {
        return PBE;                                                                 // return PBE code
    }
    // Check MCG is in FBE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT ) == 0x02 ) &&   // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST ) ) &&                                       // check FLL ref is external ref clk
             ( !( MCG_S & MCG_S_PLLST ) ) &&                                        // check PLLS mux has selected FLL
             ( !( MCG_C2 & MCG_C2_LP ) ) )                                          // check MCG_C2[LP] bit is not set
    {
        return FBE;                                                                 // return FBE code
    }
    // Check MCG is in BLPE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x02 ) &&  // check CLKS mux has selcted external reference
             ( !( MCG_S & MCG_S_IREFST ) ) &&                                       // check FLL ref is external ref clk
             ( MCG_C2 & MCG_C2_LP ) )                                               // check MCG_C2[LP] bit is set
    {
        return BLPE;                                                                // return BLPE code
    }
    // check if in BLPI mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x01 ) &&  // check CLKS mux has selcted int ref clk
             ( MCG_S & MCG_S_IREFST ) &&                                            // check FLL ref is internal ref clk
             ( !( MCG_S & MCG_S_PLLST ) ) &&                                        // check PLLS mux has selected FLL
             ( MCG_C2 & MCG_C2_LP ) )                                               // check LP bit is set
    {
        return BLPI;                                                                // return BLPI code
    }
    // check if in FBI mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x01 ) &&  // check CLKS mux has selcted int ref clk
             ( MCG_S & MCG_S_IREFST ) &&                                            // check FLL ref is internal ref clk
             ( !( MCG_S & MCG_S_PLLST ) ) &&                                        // check PLLS mux has selected FLL
             ( !( MCG_C2 & MCG_C2_LP ) ) )                                          // check LP bit is clear
    {
        return FBI;                                                                 // return FBI code
    }
    // Check MCG is in FEE mode
    else if ( ( ( ( MCG_S & MCG_S_CLKST_MASK ) >> MCG_S_CLKST_SHIFT ) == 0x00 ) &&  // check CLKS mux has selcted FLL
             ( !( MCG_S & MCG_S_IREFST ) ) &&                                       // check FLL ref is external ref clk
             ( !( MCG_S & MCG_S_PLLST ) ) )                                         // check PLLS mux has selected FLL
    {
        return FEE;                                                                 // return FEE code
    }
    
    return UNDEFINED;
}



/*class systemClock  : public connect {
private:
    virtual void hook_after( void );
    virtual void hook_before( void );
    void pee_blpi( void );
    void blpi_pee( void );
    void pee_blpe( void );
    void blpe_pee( void );
    void pbe_pee ( void );
    void blpi_blpe( void );
    void blpe_blpi( void );
    CLOCK_MODE mcg_mode( void );
    bool SIM_SCGC5_clock_active;
    bool OSC_clock_active;
public:
    systemClock(void) : SIM_SCGC5_clock_active( false ),
                        OSC_clock_active( false )
    {
        isUsed = true;
    }
};*/

#endif /* defined(systemClock_h) */
