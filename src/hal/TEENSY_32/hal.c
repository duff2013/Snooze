#if defined(__MK20DX256__)

#include "Arduino.h"
#include "common.h"
#include "hal.h"

#define COMPARE_WAKE    34
#define ALARM_WAKE      35
#define TIMER_WAKE      36
#define TOUCH_WAKE      37

typedef struct {
    uint8_t PE1;
    uint8_t PE2;
    uint8_t PE3;
    uint8_t PE4;
#if defined(HAS_KINETIS_LLWU_32CH)
    uint8_t PE5;
    uint8_t PE6;
    uint8_t PE7;
    uint8_t PE8;
#endif
    uint8_t ME;
    uint32_t wakeupSource;
    uint32_t llwuFlag;
} llwu_mask_t;

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

// module_enable defines
#define LLWU_LPTMR_MOD      0x01
//#ifdef KINETISK
#define LLWU_RTCA_MOD       0x02
#define LLWU_RTCS_MOD       0x04
//#endif
#define LLWU_CMP0_MOD       0x08
#define LLWU_CMP1_MOD       0x10
#ifdef KINETISK
#define LLWU_CMP2_MOD       0x20
#endif
#define LLWU_TSI_MOD        0x40

// external pin interrupt type defines
#define LLWU_PIN_DIS        0x00
#define LLWU_PIN_RISING     0x01
#define LLWU_PIN_FALLING    0x02
#define LLWU_PIN_ANY        0x03

// FILT1 Bit Fields
#define LLWU_FILT1_FILTSEL_MASK                  0xFu
#define LLWU_FILT1_FILTSEL_SHIFT                 0
#define LLWU_FILT1_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTSEL_SHIFT))&LLWU_FILT1_FILTSEL_MASK)
#define LLWU_FILT1_FILTE_MASK                    0x60u
#define LLWU_FILT1_FILTE_SHIFT                   5
#define LLWU_FILT1_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTE_SHIFT))&LLWU_FILT1_FILTE_MASK)
#define LLWU_FILT1_FILTF_MASK                    0x80u
#define LLWU_FILT1_FILTF_SHIFT                   7
// FILT2 Bit Fields
#define LLWU_FILT2_FILTSEL_MASK                  0xFu
#define LLWU_FILT2_FILTSEL_SHIFT                 0
#define LLWU_FILT2_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTSEL_SHIFT))&LLWU_FILT2_FILTSEL_MASK)
#define LLWU_FILT2_FILTE_MASK                    0x60u
#define LLWU_FILT2_FILTE_SHIFT                   5
#define LLWU_FILT2_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTE_SHIFT))&LLWU_FILT2_FILTE_MASK)
#define LLWU_FILT2_FILTF_MASK                    0x80u
#define LLWU_FILT2_FILTF_SHIFT                   7


// MCG_S Bit Fields
#define MCG_S_CLKST_SHIFT    ( uint8_t )0x02
#define LPTMR_MCGIRCLK       ( uint8_t )0x00
#define LPTMR_LPO            ( uint8_t )0x01
#define LPTMR_ERCLK32K       ( uint8_t )0x02
#define LPTMR_OSCERCLK       ( uint8_t )0x03
#define MCG_C1_FRDIV4_BIT    ( uint8_t )0x05

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


void ( * clear_flags )( uint32_t );

volatile uint32_t PCR3 = 0;

void wakeup_isr( void ) __attribute__ ((section(".fastrun"), noinline, noclone ));

volatile DMAMEM int wake_source = -1;

int llwu_disable( volatile llwu_mask_t *mask );

volatile llwu_mask_t llwuMask;

POWERDOWN_MODES PD_MODE = LLS;

//----------------------------------------------------------------------------------
// HAL specific power down setting
void hal_set_powerdown_mode( POWERDOWN_MODES mode ) {
    PD_MODE = mode;
}
//----------------------------------------------------------------------------------
// HAL specific power down setting
POWERDOWN_MODES hal_get_powerdown_mode( void ) {
    return PD_MODE;
}
//----------------------------------------------------------------------------------
void wait( void ) {
    // Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead of deep sleep.
    SCB_SCR = 0;
    // WFI instruction will start entry into WAIT mode
    asm volatile( "wfi" );
}
//----------------------------------------------------------------------------------
void vlpw( void ) {
    // disable systick timer interrupt
    SYST_CSR &= ~SYST_CSR_TICKINT;
    // WFI instruction will start entry into WAIT mode
    asm volatile( "wfi" );
    // renable systick timer interrupt
    SYST_CSR |= SYST_CSR_TICKINT;
}
//----------------------------------------------------------------------------------
void __stop( void ) {
    // disable systick timer interrupt
    SYST_CSR &= ~SYST_CSR_TICKINT;
    // Set the SLEEPDEEP bit to enable deep sleep mode (STOP)
    SCB_SCR = SCB_SCR_SLEEPDEEP;
    // WFI instruction will start entry into STOP mode
    asm volatile( "wfi" );
    
    /*if (SMC_PMCTRL & SMC_PMCTRL_STOPA) {
     digitalWrite(LED_BUILTIN, HIGH);
     while(1);
     }*/
    
    // Clear the SLEEPDEEP bit
    SCB_SCR = 0;
    // renable systick timer interrupt
    SYST_CSR |= SYST_CSR_TICKINT;
}
//----------------------------------------------------------------------------------
void vlls0_nopor( void ) {
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x00 );// set VLLSM = 0b00
    ( void ) SMC_VLLSCTRL;
    __stop( );
}
//----------------------------------------------------------------------------------
void vlls0( void ) {
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x00 );// set VLLSM = 0b00
    ( void ) SMC_VLLSCTRL;
    // Now execute the __stop instruction to go into VLLS0
    __stop( );
}
//----------------------------------------------------------------------------------
void vlls1( void ) {
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x01 );// set VLLSM = 0b01
    ( void ) SMC_VLLSCTRL;
    // Now execute the __stop instruction to go into VLLS1
    __stop( );
}
//----------------------------------------------------------------------------------
void vlls2( void ) {
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x02 );// set VLLSM = 0b10
    ( void ) SMC_VLLSCTRL;
    // Now execute the __stop instruction to go into VLLS2
    __stop( );
}
//----------------------------------------------------------------------------------
void vlls3( void ) {
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x03 );// set VLLSM = 0b11
    ( void ) SMC_VLLSCTRL;
    // Now execute the __stop instruction to go into VLLS3
    __stop( );
}
//----------------------------------------------------------------------------------
void lls( void ) {
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 ) ;
    ( void ) SMC_PMCTRL;
    // Now execute the __stop instruction to go into LLS
    __stop( );
}
//----------------------------------------------------------------------------------
void vlps( void ) {
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x02 ) ;
    ( void ) SMC_PMCTRL;
    // Now execute the __stop instruction to go into VLPS
    __stop( );
}
//----------------------------------------------------------------------------------
void exit_vlpr( void ) {
    // check to make sure in VLPR before exiting
    if  ( !( SMC_PMSTAT & SMC_PMSTAT_VLPR ) ) return;
    // Clear RUNM
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x00 );
    ( void ) SMC_PMCTRL;
    
    // Wait for normal RUN regulation mode to be confirmed
    // 0 MCU is not in run regulation mode
    // 1 MCU is in run regulation mode
    while( !( PMC_REGSC & PMC_REGSC_REGONS ) );
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
}
//----------------------------------------------------------------------------------
void enter_vlpr( void ) {
    if ( SMC_PMSTAT & SMC_PMSTAT_VLPR ) return;
    
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0 );
    ( void ) SMC_PMCTRL;
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
    
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 2 );
    ( void ) SMC_PMCTRL;
    
    // Wait for VLPS regulator mode to be confirmed
    while( ( PMC_REGSC & PMC_REGSC_REGONS ) ); // 0 Regulator in __stop Reg mode
    
    // 1 MCU is in Run regulation mode
    while( !( SMC_PMSTAT & SMC_PMSTAT_VLPR ) );
}
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 0 ) |     SIM_CLKDIV1_OUTDIV3( 0 ) |SIM_CLKDIV1_OUTDIV4( 1 );
}
//----------------------------------------------------------------------------------
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( div ) | SIM_CLKDIV1_OUTDIV2( div ) |     SIM_CLKDIV1_OUTDIV3( div ) |SIM_CLKDIV1_OUTDIV4( div );
}
//----------------------------------------------------------------------------------
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
    MCG_C5 = MCG_C5_PRDIV0( 5 );         // config PLL input for 16 MHz Crystal / 6 = 2.667 Hz
#else
    MCG_C5 = MCG_C5_PRDIV0( 3 );         // config PLL input for 16 MHz Crystal / 4 = 4 MHz
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) |    SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 144000000
    // config divisors: 144 MHz core, 48 MHz bus, 28.8 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 120000000
    // config divisors: 120 MHz core, 60 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |    SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 96000000
    // config divisors: 96 MHz core, 48 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#elif F_CPU == 72000000
    // config divisors: 72 MHz core, 36 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |    SIM_CLKDIV1_OUTDIV4( 2 );
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
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
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
    MCG_C5 = MCG_C5_PRDIV0( 5 );         // config PLL input for 16 MHz Crystal / 6 = 2.667 Hz
#else
    MCG_C5 = MCG_C5_PRDIV0( 3 );         // config PLL input for 16 MHz Crystal / 4 = 4 MHz
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) |    SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 144000000
    // config divisors: 144 MHz core, 48 MHz bus, 28.8 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 2 ) | SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 120000000
    // config divisors: 120 MHz core, 60 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |    SIM_CLKDIV1_OUTDIV4( 4 );
#elif F_CPU == 96000000
    // config divisors: 96 MHz core, 48 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) | SIM_CLKDIV1_OUTDIV4( 3 );
#elif F_CPU == 72000000
    // config divisors: 72 MHz core, 36 MHz bus, 24 MHz flash
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( 0 ) | SIM_CLKDIV1_OUTDIV2( 1 ) |    SIM_CLKDIV1_OUTDIV4( 2 );
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
//----------------------------------------------------------------------------------
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
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1( div ) | SIM_CLKDIV1_OUTDIV2( div ) |     SIM_CLKDIV1_OUTDIV3( div ) | SIM_CLKDIV1_OUTDIV4( div );
}
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
void startup_early_hook( void ) {
#if defined(KINETISK)
    WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;
#elif defined(KINETISL)
    SIM_COPC = 0;  // disable the watchdog
#endif
    if ( PMC_REGSC & PMC_REGSC_ACKISO ) {
        //llwuMask.llwuFlag = llwu_clear_flags( );// clear flags
        //llwuMask.wakeupSource = llwu_disable( );
    }
}
//----------------------------------------------------------------------------------
void llwu_configure_filter( unsigned int pin_num, unsigned char filter_en, unsigned char rise_fall ) {
    //pin_num is the pin number to be written to FILTSEL.  pin_num is not the same as pin_en.
    uint8_t temp;
    
    temp = 0;
    //first clear filter values and clear flag by writing a 1
    LLWU_FILT1 = LLWU_FILT1_FILTF_MASK;
    LLWU_FILT2 = LLWU_FILT2_FILTF_MASK;
    
    if ( filter_en == 1 ) {
        //clear the flag bit and set the others
        temp |= ( LLWU_FILT1_FILTF_MASK ) | ( LLWU_FILT1_FILTE( rise_fall ) | LLWU_FILT1_FILTSEL( pin_num ) );
        LLWU_FILT1 = temp;
    }
    else if ( filter_en == 2 ) {
        //clear the flag bit and set the others
        temp |= ( LLWU_FILT2_FILTF_MASK ) | ( LLWU_FILT2_FILTE( rise_fall ) | LLWU_FILT2_FILTSEL( pin_num ) );
        LLWU_FILT2 = temp;
    }
    else {
        
    }
}
//----------------------------------------------------------------------------------
int llwu_disable( volatile llwu_mask_t *mask ) {
    uint32_t flag = mask->llwuFlag;
    int source = -1;
    if      ( flag & LLWU_F1_WUF0 ) source = 26;
    else if ( flag & LLWU_F1_WUF3 ) source = 33;
    else if ( flag & LLWU_F1_WUF4 ) source = 4;
    else if ( flag & LLWU_F1_WUF5 ) source = 16;
    else if ( flag & LLWU_F1_WUF6 ) source = 22;
    else if ( flag & LLWU_F1_WUF7 ) source = 9;
    
    else if ( ( flag>>8 ) & LLWU_F2_WUF8  ) source = 10;
    else if ( ( flag>>8 ) & LLWU_F2_WUF9  ) source = 13;
    else if ( ( flag>>8 ) & LLWU_F2_WUF10 ) source = 11;
    else if ( ( flag>>8 ) & LLWU_F2_WUF11 ) source = 30;
    else if ( ( flag>>8 ) & LLWU_F2_WUF12 ) source = 2;
    else if ( ( flag>>8 ) & LLWU_F2_WUF13 ) source = 7;
    else if ( ( flag>>8 ) & LLWU_F2_WUF14 ) source = 6;
    else if ( ( flag>>8 ) & LLWU_F2_WUF15 ) source = 21;
    
    else if ( ( flag>>16 ) & LLWU_F3_MWUF0 ) source = 36;
    else if ( ( flag>>16 ) & LLWU_F3_MWUF1 ) source = 34;
    else if ( ( flag>>16 ) & LLWU_F3_MWUF4 ) source = 37;
    else if ( ( flag>>16 ) & LLWU_F3_MWUF3 ) source = 34;
    else if ( ( flag>>16 ) & LLWU_F3_MWUF2 ) source = 34;
    else if ( ( flag>>16 ) & LLWU_F3_MWUF5 ) source = 35;
    LLWU_PE1 = 0;
    LLWU_PE2 = 0;
    LLWU_PE3 = 0;
    LLWU_PE4 = 0;
    mask->PE1 = 0;
    mask->PE2 = 0;
    mask->PE3 = 0;
    mask->PE4 = 0;
    LLWU_ME  = mask->ME;
    mask->ME = 0;
    mask->llwuFlag = 0;
    return source;
}
//----------------------------------------------------------------------------------
uint32_t llwu_clear_flags( void ) {
    uint32_t flag = ( LLWU_F1 | LLWU_F2 << 8 | LLWU_F3 << 16 );
    LLWU_F1 = 0xFF;
    LLWU_F2 = 0xFF;
    LLWU_F3 = 0xFF;
    LLWU_FILT1 = 0x80;
    LLWU_FILT2 = 0x80;
    return flag;
}
//----------------------------------------------------------------------------------
void llwu_set(void ) {
    volatile llwu_mask_t *mask = &llwuMask;
    LLWU_PE1 = mask->PE1;
    LLWU_PE2 = mask->PE2;
    LLWU_PE3 = mask->PE3;
    LLWU_PE4 = mask->PE4;
    LLWU_ME  = mask->ME;
}
//----------------------------------------------------------------------------------
void llwu_clear_modules( void ) {
    LLWU_ME = 0;
}
//----------------------------------------------------------------------------------
void llwu_clear_pins( void ) {
    LLWU_PE1 = 0;
    LLWU_PE2 = 0;
    LLWU_PE3 = 0;
    LLWU_PE4 = 0;
}
//----------------------------------------------------------------------------------
void llwu_configure_modules_mask( uint8_t module ) {
    volatile llwu_mask_t *mask = &llwuMask;
    if( module & LLWU_LPTMR_MOD )      mask->ME |= LLWU_ME_WUME0;
    else if ( module & LLWU_RTCA_MOD ) mask->ME |= LLWU_ME_WUME5;
    else if ( module & LLWU_RTCS_MOD ) mask->ME |= LLWU_ME_WUME7;
    else if ( module & LLWU_TSI_MOD )  mask->ME |= LLWU_ME_WUME4;
    else if ( module & LLWU_CMP0_MOD ) mask->ME |= LLWU_ME_WUME1;
    else if ( module & LLWU_CMP1_MOD ) mask->ME |= LLWU_ME_WUME2;
#ifdef KINETISK
    else if( module & LLWU_CMP2_MOD )  mask->ME |= LLWU_ME_WUME3;
#endif
}
//----------------------------------------------------------------------------------
void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall ) {
    volatile llwu_mask_t *mask = &llwuMask;
    uint8_t detect_type;
    if ( rise_fall == RISING )       detect_type = LLWU_PIN_RISING;
    else if ( rise_fall == FALLING ) detect_type = LLWU_PIN_FALLING;
    else if ( rise_fall == CHANGE )  detect_type = LLWU_PIN_ANY;
    else                             detect_type = LLWU_PIN_ANY;
    
    if ( pin == 2 )       mask->PE4 |= LLWU_PE4_WUPE12( detect_type );
    else if ( pin == 4 )  mask->PE2 |= LLWU_PE2_WUPE4 ( detect_type );
    else if ( pin == 6 )  mask->PE4 |= LLWU_PE4_WUPE14( detect_type );
    else if ( pin == 7 )  mask->PE4 |= LLWU_PE4_WUPE13( detect_type );
    else if ( pin == 9 )  mask->PE2 |= LLWU_PE2_WUPE7 ( detect_type );
    else if ( pin == 10 ) mask->PE3 |= LLWU_PE3_WUPE8 ( detect_type );
    else if ( pin == 11 ) mask->PE3 |= LLWU_PE3_WUPE10( detect_type );
    else if ( pin == 13 ) mask->PE3 |= LLWU_PE3_WUPE9 ( detect_type );
    else if ( pin == 16 ) mask->PE2 |= LLWU_PE2_WUPE5 ( detect_type );
    else if ( pin == 21 ) mask->PE4 |= LLWU_PE4_WUPE15( detect_type );
    else if ( pin == 22 ) mask->PE2 |= LLWU_PE2_WUPE6 ( detect_type );
    else if ( pin == 26 ) mask->PE1 |= LLWU_PE1_WUPE0 ( detect_type );
    else if ( pin == 30 ) mask->PE3 |= LLWU_PE3_WUPE11( detect_type );
    else if ( pin == 33 ) mask->PE1 |= LLWU_PE1_WUPE3 ( detect_type );
    else if ( pin == 59 ) mask->PE1 |= LLWU_PE1_WUPE0( detect_type );
    else if ( pin == 60 ) mask->PE1 |= LLWU_PE1_WUPE1( detect_type );
    else if ( pin == 62 ) mask->PE1 |= LLWU_PE1_WUPE2( detect_type );
}
//----------------------------------------------------------------------------------
void wakeup_isr( void ) {
    int source = -1;
    uint32_t ipsr;
    __asm__ volatile("mrs %0, ipsr\n" : "=r" (ipsr)::);
    switch (ipsr-16) {
        case IRQ_CMP0:
            source = COMPARE_WAKE;
            break;
        case IRQ_CMP1:
            source = COMPARE_WAKE;
            break;
        case IRQ_CMP2:
            source = COMPARE_WAKE;
            break;
        case IRQ_RTC_ALARM:
            source = ALARM_WAKE;
            break;
        case IRQ_LPTMR:
            source = TIMER_WAKE;
            break;
        case IRQ_TSI:
            source = TOUCH_WAKE;
            break;
        case IRQ_PORTA: {
            uint32_t isfr_a = PORTA_ISFR;
            PORTA_ISFR = isfr_a;
            if ( isfr_a & CORE_PIN3_BITMASK )       source = 3;
            else if ( isfr_a & CORE_PIN4_BITMASK  ) source = 4;
            else if ( isfr_a & CORE_PIN24_BITMASK ) source = 24;
            else if ( isfr_a & CORE_PIN33_BITMASK ) source = 33;
            break;
        }
        case IRQ_PORTB: {
            uint32_t isfr_b = PORTB_ISFR;
            PORTB_ISFR = isfr_b;
            if ( isfr_b & CORE_PIN0_BITMASK  )      source = 0;
            else if ( isfr_b & CORE_PIN1_BITMASK  ) source = 1;
            else if ( isfr_b & CORE_PIN16_BITMASK ) source = 16;
            else if ( isfr_b & CORE_PIN17_BITMASK ) source = 17;
            else if ( isfr_b & CORE_PIN18_BITMASK ) source = 18;
            else if ( isfr_b & CORE_PIN19_BITMASK ) source = 19;
            else if ( isfr_b & CORE_PIN25_BITMASK ) source = 25;
            else if ( isfr_b & CORE_PIN32_BITMASK ) source = 32;
            
            break;
        }
        case IRQ_PORTC: {
            uint32_t isfr_c = PORTC_ISFR;
            PORTC_ISFR = isfr_c;
            if ( isfr_c & CORE_PIN9_BITMASK )       source = 9;
            else if ( isfr_c & CORE_PIN10_BITMASK ) source = 10;
            else if ( isfr_c & CORE_PIN11_BITMASK ) source = 11;
            else if ( isfr_c & CORE_PIN12_BITMASK ) source = 12;
            else if ( isfr_c & CORE_PIN13_BITMASK ) source = 13;
            else if ( isfr_c & CORE_PIN15_BITMASK ) source = 15;
            else if ( isfr_c & CORE_PIN22_BITMASK ) source = 22;
            else if ( isfr_c & CORE_PIN23_BITMASK ) source = 23;
            else if ( isfr_c & CORE_PIN27_BITMASK ) source = 27;
            else if ( isfr_c & CORE_PIN28_BITMASK ) source = 28;
            else if ( isfr_c & CORE_PIN29_BITMASK ) source = 29;
            else if ( isfr_c & CORE_PIN30_BITMASK ) source = 30;
            break;
        }
        case IRQ_PORTD: {
            uint32_t isfr_d = PORTD_ISFR;
            PORTD_ISFR = isfr_d;
            if ( isfr_d & CORE_PIN2_BITMASK )       source = 2;
            else if ( isfr_d & CORE_PIN5_BITMASK )  source = 5;
            else if ( isfr_d & CORE_PIN6_BITMASK )  source = 6;
            else if ( isfr_d & CORE_PIN7_BITMASK )  source = 7;
            else if ( isfr_d & CORE_PIN8_BITMASK )  source = 8;
            else if ( isfr_d & CORE_PIN14_BITMASK ) source = 14;
            else if ( isfr_d & CORE_PIN20_BITMASK ) source = 20;
            else if ( isfr_d & CORE_PIN21_BITMASK ) source = 21;
            break;
        }
        case IRQ_PORTE:
            break;
        case IRQ_LLWU:
            llwuMask.llwuFlag = llwu_clear_flags( );
            source = llwu_disable( &llwuMask );
            pbe_pee( );
            break;
        default:
            source = -1;
            break;
    }
    clear_flags( ipsr );
    wake_source = source;
}
//----------------------------------------------------------------------------------
void hal_initialize( void ( * ptr )( uint32_t ) ) {
    //if ( mode < VLLS3 ) mode = RUN;
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
    attachInterruptVector( IRQ_LLWU, wakeup_isr );
    NVIC_ENABLE_IRQ( IRQ_LLWU );
    clear_flags = ptr;
}
//----------------------------------------------------------------------------------
void hal_idle ( void ) {
    wait( );
}
//----------------------------------------------------------------------------------
int source ( void ) {
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_sleep ( void ) {
    //wake_source = -1;
    pee_blpi( );
    //pee_blpe( );
    enter_vlpr( );
    //vlps( );
    vlpw( );
    exit_vlpr( );
    //blpe_pee( );
    blpi_pee( );
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_deepSleep ( void ) {
    int priority = nvic_execution_priority( );// get current priority
    priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    NVIC_SET_PRIORITY( IRQ_LLWU, priority );//set priority to new level
    llwu_set( );
    switch ( PD_MODE ) {
        case LLS:
            lls( );
            break;
        case VLLS3:
            vlls3( );
            break;
        case VLLS2:
            vlls2( );
            break;
        case VLLS1:
            vlls1( );
            break;
        case VLLS0:
            vlls0( );
            break;
        default:
            break;
    }
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_hibernate ( void ) {
    int priority = nvic_execution_priority( );// get current priority
    priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    NVIC_SET_PRIORITY( IRQ_LLWU, priority );//set priority to new level
    // allows the SOPT1 USBSSTBY bit to be written
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 |= SIM_SOPT1_USBSSTBY;
    PCR3 = PORTA_PCR3;
    PORTA_PCR3 = PORT_PCR_MUX( 0 );
    llwu_set( );
    switch ( PD_MODE ) {
        case LLS:
            lls( );
            break;
        case VLLS3:
            vlls3( );
            break;
        case VLLS2:
            vlls2( );
            break;
        case VLLS1:
            vlls1( );
            break;
        case VLLS0:
            vlls0( );
            break;
        default:
            break;
    }
    // allows the SOPT1 USBSSTBY bit to be written
    SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
    SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
    PORTA_PCR3 = PCR3;
    return wake_source;
}
#endif /* __MK20DX256__ */
