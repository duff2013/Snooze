/***********************************************************************************
 *  SnoozeTimer.h
 *  Teensy 3.5
 *
 * Purpose: Low Power Timer Driver
 *
 **********************************************************************************/
#if defined(__MK64FX512__)

#include "SnoozeTimer.h"

#define LLWU_LPTMR_MOD      ( uint8_t )0x01
#define LPTMR_LPO           ( uint8_t )0x01

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_modules_mask( uint8_t module );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

void ( * SnoozeTimer::return_lptmr_irq ) ( void );
volatile uint16_t SnoozeTimer::period;
/*******************************************************************************
 *  Set The Period for the Low Power Timer to wake the processor
 *
 *  @param newPeriod period in milliseconds
 *******************************************************************************/
void SnoozeTimer::setTimer( uint16_t newPeriod ) {
    isUsed = true;
    period = newPeriod;
}

/*******************************************************************************
 *  Disables low power timer and returns registers and clock to previous state
 *******************************************************************************/
void SnoozeTimer::disableDriver( uint8_t mode ) {
    LPTMR0_CSR = 0;
    if ( mode == 0 ) return;
    if ( mode == 1 ) {
        if ( return_isr_enabled == 0 )  NVIC_DISABLE_IRQ( IRQ_LPTMR ); //disable irq
        NVIC_SET_PRIORITY( IRQ_LPTMR, return_priority );// return priority
        __disable_irq( );
        attachInterruptVector( IRQ_LPTMR, return_lptmr_irq );// return prev interrupt
        __enable_irq( );
    }
    LPTMR0_PSR = PSR;
    LPTMR0_CMR = CMR;
    LPTMR0_CSR = CSR;
    if ( !OSC_clock_active ) OSC0_CR &= ~OSC_ERCLKEN;
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
}

/*******************************************************************************
 *  Enables low power timer and saves regiater and clock state
 *******************************************************************************/
void SnoozeTimer::enableDriver( uint8_t mode ) {
    if ( mode == 0 ) return;
    if ( mode == 1 ) {
        return_priority = NVIC_GET_PRIORITY( IRQ_LPTMR );//get current priority
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler mode set priority higher than current handler
        priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_LPTMR, priority );//set priority to new level
        __disable_irq( );
        return_lptmr_irq = _VectorsRam[IRQ_LPTMR+16];// save prev isr
        attachInterruptVector( IRQ_LPTMR, wakeup_isr );
        __enable_irq( );
    }
    
    if ( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) SIM_SCGC5_clock_active = true;
    else SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    
    PSR = LPTMR0_PSR;
    CMR = LPTMR0_CMR;
    CSR = LPTMR0_CSR;
    
    LPTMR0_CSR = 0;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
    LPTMR0_CMR = period;
    
    /*if ( period <= 50 ) {
     #if defined(KINETISK)
     OSC_clock_active = true;
     if ( !( OSC0_CR & OSC_ERCLKEN ) ) {
     OSC_clock_active = false;
     OSC0_CR |= OSC_EREFSTEN | OSC_ERCLKEN;
     while ( ( MCG_S & MCG_S_OSCINIT0 ) == 0 );
     }
     #endif
     LPTMR0_PSR = LPTMR_PSR_PCS( LPTMR_OSCERCLK ) | LPTMR_PSR_PRESCALE( 8 );
     LPTMR0_CMR = (period*31.24);
     
     } else {
     LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
     LPTMR0_CMR = period;
     }*/
    
    //if ( mode == VLPW || mode == VLPS ) {
    if ( mode == 1 ) {
        NVIC_CLEAR_PENDING(IRQ_LPTMR);
        LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TIE | LPTMR_CSR_TCF;
        return_isr_enabled = NVIC_IS_ENABLED( IRQ_LPTMR );
        if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_LPTMR );
    } else {
        llwu_configure_modules_mask( LLWU_LPTMR_MOD );
        LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TIE | LPTMR_CSR_TCF;
    }
}

/*******************************************************************************
 *  clears low power timer flags called from sleep llwu wakeup
 *******************************************************************************/
void SnoozeTimer::clearIsrFlags( uint32_t ipsr ) {
    if ( !( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) ) return;
    systick_millis_count += period;
    LPTMR0_CSR |= LPTMR_CSR_TCF;
}
#endif /* __MK64FX512__ */
