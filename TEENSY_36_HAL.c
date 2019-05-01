#include "Arduino.h"
#include "TEENSY_35_HAL.h"

#if defined(__MK66FX1M0__)

inline void wait( void ) {
    // Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead of deep sleep.
    SCB_SCR = 0;
    // WFI instruction will start entry into WAIT mode
    asm volatile( "wfi" );
}

inline void vlpw( void ) {
    // disable systick timer interrupt
    SYST_CSR &= ~SYST_CSR_TICKINT;
    // WFI instruction will start entry into WAIT mode
    asm volatile( "wfi" );
    // renable systick timer interrupt
    SYST_CSR |= SYST_CSR_TICKINT;
}

inline void stop( void ) {
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

inline void vlls0_nopor( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x03 ) | SMC_PMCTRL_STOPM( 0x04 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 ) ;
    ( void ) SMC_PMCTRL;
#endif
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x00 );// set VLLSM = 0b00
    ( void ) SMC_VLLSCTRL;
    // Now execute the stop instruction to go into VLLS0
#if defined(__MK66FX1M0__)
    kinetis_hsrun_enable( );
#endif
}

inline void vlls0( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x03 ) | SMC_PMCTRL_STOPM( 0x04 );
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x00 );// set VLLSM = 0b00
    ( void ) SMC_VLLSCTRL;
    // Now execute the stop instruction to go into VLLS0
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_enable( );
#endif
#endif
}

inline void vlls1( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_disable( );
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x01 );// set VLLSM = 0b01
    ( void ) SMC_VLLSCTRL;
    // Now execute the stop instruction to go into VLLS1
    stop( );
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_enable( );
#endif
#endif
}

inline void vlls2( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_disable( );
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x02 );// set VLLSM = 0b10
    ( void ) SMC_VLLSCTRL;
    // Now execute the stop instruction to go into VLLS2
    stop( );
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_enable( );
#endif
#endif
}

inline void vlls3( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_disable( );
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
#endif
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x03 );// set VLLSM = 0b11
    ( void ) SMC_VLLSCTRL;
    // Now execute the stop instruction to go into VLLS3
    stop( );
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_enable( );
#endif
#endif
}

inline void lls( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_disable( );
    
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 );
    ( void ) SMC_PMCTRL;
    //SMC_STOPCTRL = 0x02;
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 ) ;
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x03 ) ;
    ( void ) SMC_PMCTRL;
#endif
    // Now execute the stop instruction to go into LLS
    stop( );
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_enable( );
#endif
#endif
}

inline void vlps( void ) {
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    kinetis_hsrun_disable( );
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x02 ) | SMC_PMCTRL_STOPM( 0x02 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x02 ) | SMC_PMCTRL_STOPM( 0x02 );
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x02 ) ;
    ( void ) SMC_PMCTRL;
#endif
    // Now execute the stop instruction to go into VLPS
    stop( );
}

inline void exit_vlpr( void ) {
    // check to make sure in VLPR before exiting
    if  ( !( SMC_PMSTAT & SMC_PMSTAT_VLPR ) ) return;
    // Clear RUNM
    
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0 );
    ( void ) SMC_PMCTRL;
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
    
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x03 );
    ( void ) SMC_PMCTRL;
#else
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x00 );
    ( void ) SMC_PMCTRL;
#endif
#else
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0x00 );
    ( void ) SMC_PMCTRL;
#endif
    
    // Wait for normal RUN regulation mode to be confirmed
    // 0 MCU is not in run regulation mode
    // 1 MCU is in run regulation mode
    while( !( PMC_REGSC & PMC_REGSC_REGONS ) );
#if defined(__MK66FX1M0__)
#if F_CPU > 120000000
    while( !( SMC_PMSTAT & SMC_PMSTAT_HSRUN ) );
#else
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
#endif
#else
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
#endif
    
    
}

inline void enter_vlpr( void ) {
    if ( SMC_PMSTAT & SMC_PMSTAT_VLPR ) return;
    
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 0 );
    ( void ) SMC_PMCTRL;
    while( !( SMC_PMSTAT & SMC_PMSTAT_RUN ) );
    
    SMC_PMCTRL = SMC_PMCTRL_RUNM( 2 );
    ( void ) SMC_PMCTRL;
    
    // Wait for VLPS regulator mode to be confirmed
    while( ( PMC_REGSC & PMC_REGSC_REGONS ) ); // 0 Regulator in stop Reg mode
    
    // 1 MCU is in Run regulation mode
    while( !( SMC_PMSTAT & SMC_PMSTAT_VLPR ) );
}

inline void _stop( void ) {
    // Set the STOPM field to 0b000 for normal STOP mode - Need to
    // retain state of LPWUI bit 8
    SMC_PMCTRL =  SMC_PMCTRL_STOPM( 0x00 );// set STOPM = 0b000
    ( void ) SMC_PMCTRL;
    stop( );
}

#endif
