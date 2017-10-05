/***********************************************************************************
 *  SnoozeCompare.h
 *  Teensy 3.x/LC
 *
 * Purpose: Compare Driver
 *
 ***********************************************************************************/
#include "SnoozeCompare.h"
#include "wake.h"

volatile uint8_t *SnoozeCompare::cmpx_cr0;//CMP0_CR0;
volatile uint8_t *SnoozeCompare::cmpx_cr1;//CMP0_CR1;
volatile uint8_t *SnoozeCompare::cmpx_scr;//CMP0_SCR;
volatile uint8_t *SnoozeCompare::cmpx_fpr;//CMP0_FPR;
volatile uint8_t *SnoozeCompare::cmpx_muxcr;//CMP0_MUXCR;
volatile uint8_t *SnoozeCompare::cmpx_daccr;//CMP0_DACCR;
/*******************************************************************************
 *  Save the modes
 *
 *  @param _pin  <#_pin description#>
 *  @param _type <#_type description#>
 *  @param val   <#val description#>
 *******************************************************************************/
void SnoozeCompare::pinMode( int _pin, int _type, float val ) {
    isUsed = true;
    pin = _pin;
    type = _type;
    threshold_crossing = val;
    if ( pin == 4 ) {
#if defined(KINETISK)
        cmpx_cr0   = &CMP2_CR0;
        cmpx_cr1   = &CMP2_CR1;
        cmpx_scr   = &CMP2_SCR;
        cmpx_fpr   = &CMP2_FPR;
        cmpx_muxcr = &CMP2_MUXCR;
        cmpx_daccr = &CMP2_DACCR;
#endif
    }
    else if ( pin == 9 ) {
#if defined(KINETISK)
        cmpx_cr0   = &CMP1_CR0;
        cmpx_cr1   = &CMP1_CR1;
        cmpx_scr   = &CMP1_SCR;
        cmpx_fpr   = &CMP1_FPR;
        cmpx_muxcr = &CMP1_MUXCR;
        cmpx_daccr = &CMP1_DACCR;
#endif
    }
    else if ( pin == 11 ) {
        cmpx_cr0   = &CMP0_CR0;
        cmpx_cr1   = &CMP0_CR1;
        cmpx_scr   = &CMP0_SCR;
        cmpx_fpr   = &CMP0_FPR;
        cmpx_muxcr = &CMP0_MUXCR;
        cmpx_daccr = &CMP0_DACCR;
    }
}

/*******************************************************************************
 *  Disable Driver
 *******************************************************************************/
void SnoozeCompare::disableDriver( void ) {
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
        /*NVIC_DISABLE_IRQ( IRQ_CMP2 );
        NVIC_SET_PRIORITY( IRQ_CMP2, 128 );//return priority to core value
        __disable_irq( );
        attachInterruptVector( IRQ_CMP2, return_cmp0_irq );// return prev interrupt
        __enable_irq( );*/
    }

    if ( pin == 11 ) {
        CORE_PIN11_CONFIG = return_core_pin_config[0];
    }
    else if ( pin == 4 ) {
#if defined(KINETISK)
        CORE_PIN4_CONFIG = return_core_pin_config[1];
#else 
        return;
#endif
    }
    else if ( pin == 9 ) {
#if defined(KINETISK)
        CORE_PIN9_CONFIG = return_core_pin_config[2];
#else
        return;
#endif
    }
    
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    LPTMR0_PSR = PSR;
    LPTMR0_CMR = CMR;
    LPTMR0_CSR = CSR;
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#endif
    
    *cmpx_cr0   = CR0;
    *cmpx_cr1   = CR1;
    *cmpx_scr   = SCR;
    *cmpx_fpr   = FPR;
    *cmpx_muxcr = MUXCR;
    *cmpx_daccr = DACCR;
    
    /*CMP2_CR0 = CR0;
    CMP2_CR1 = CR1;
    CMP2_SCR = SCR;
    CMP2_FPR = FPR;
    CMP2_MUXCR = MUXCR;
    CMP2_DACCR = DACCR;*/
    if ( !SIM_SCGC4_clock_active ) SIM_SCGC4 &= ~SIM_SCGC4_CMP;
    //CMP2_CR0 = 0x00;
    //CMP2_CR1 = 0x00;
    //SIM_SCGC4 &= ~SIM_SCGC4_CMP;
    

}

/*******************************************************************************
 *  Enable Driver
 *******************************************************************************/
void SnoozeCompare::enableDriver( void ) {
    
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
        /*int priority = nvic_execution_priority( );// get current priority
        // if running from handler set priority higher than current handler
        priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_CMP2, priority );//set priority to new level
        
        __disable_irq( );
        return_cmp0_irq = _VectorsRam[IRQ_CMP2+16];// save prev isr
        attachInterruptVector( IRQ_CMP2, wakeupIsr );
        __enable_irq( );
        NVIC_ENABLE_IRQ( IRQ_CMP2 );*/
    }
    
    if ( SIM_SCGC4 & SIM_SCGC4_CMP ) SIM_SCGC4_clock_active = true;
    else SIM_SCGC4 |= SIM_SCGC4_CMP;
    
    /*CR0 = CMP2_CR0;
    CR1 = CMP2_CR1;
    SCR = CMP2_SCR;
    FPR = CMP2_FPR;
    MUXCR = CMP2_MUXCR;
    DACCR = CMP2_DACCR;*/
    
    CR0 = *cmpx_cr0;//CMP2_CR0;
    CR1 = *cmpx_cr1;//CMP2_CR1;
    SCR = *cmpx_scr;//CMP2_SCR;
    FPR = *cmpx_fpr;//CMP2_FPR;
    MUXCR = *cmpx_muxcr;//CMP2_MUXCR;
    DACCR = *cmpx_daccr;//CMP2_DACCR;
    
    uint8_t _pin = 0;
    *cmpx_cr0 = 0;
    *cmpx_cr1 = 0;
    *cmpx_scr = 0;
    
    /*CMP2_CR0 = 0x00;
    CMP2_CR1 = 0x00;
    CMP2_SCR = 0x00;*/
    
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    if ( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) SIM_SCGC5_clock_active = true;
    else SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    PSR = LPTMR0_PSR;
    CMR = LPTMR0_CMR;
    CSR = LPTMR0_CSR;
#endif
    
    if ( pin == 11 ) {
        if ( mode >= LLS ) llwu_configure_modules_mask( LLWU_CMP0_MOD );
        return_core_pin_config[0] = CORE_PIN11_CONFIG;
        CORE_PIN11_CONFIG = PORT_PCR_MUX( 0 );
        _pin = 0x00;
    }
    else if ( pin == 4 ) {
#if defined(KINETISK)
        if ( mode >= LLS ) llwu_configure_modules_mask( LLWU_CMP2_MOD );
        return_core_pin_config[1] = CORE_PIN4_CONFIG;
        CORE_PIN4_CONFIG = PORT_PCR_MUX( 0 );
        _pin = 0x01;
#else
        return;
#endif
    }
    else if ( pin == 9 ) {
#if defined(KINETISK)
        if ( mode >= LLS ) llwu_configure_modules_mask( LLWU_CMP1_MOD );
        return_core_pin_config[2] = CORE_PIN9_CONFIG;
        CORE_PIN9_CONFIG = PORT_PCR_MUX( 0 );
        _pin = 0x01;
#else
        return;
#endif
    }
    /*else {
        SIM_SCGC4 &= ~SIM_SCGC4_CMP;
        return;
    }*/
    
    *cmpx_cr0 = CMP_CR0_FILTER_CNT( 0x07 );
    if ( type == CHANGE ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF | CMP_SCR_IER;
    else if ( type == RISING || type == HIGH ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IER;
    else if ( type == FALLING || type == LOW ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF;
    else return;
    uint8_t tap = ( threshold_crossing/0.0515625 ) - 1;
    *cmpx_fpr = 0x00;
    *cmpx_muxcr = CMP_MUXCR_MSEL( 0x07 ) | CMP_MUXCR_PSEL( _pin );
    *cmpx_daccr = CMP_DACCR_DACEN | CMP_DACCR_VRSEL | CMP_DACCR_VOSEL( tap );
    
    //if ( mode == VLPW ) NVIC_ENABLE_IRQ( IRQ_CMP2 );
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    *cmpx_cr1 = CMP_CR1_EN | CMP_CR1_TRIGM;
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    LPTMR0_CSR = 0;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
    LPTMR0_CMR = 1;
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
#else
    *cmpx_cr1 = CMP_CR1_EN;
#endif
    
    /*CMP2_CR0 = CMP_CR0_FILTER_CNT( 0x07 );
    if ( type == CHANGE ) CMP2_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF | CMP_SCR_IER;
    else if ( type == RISING || type == HIGH ) CMP2_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IER;
    else if ( type == FALLING || type == LOW ) CMP2_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF;
    else return;
    uint8_t tap = ( threshold_crossing/0.0515625 ) - 1;
    CMP2_FPR = 0x00;
    CMP2_MUXCR = CMP_MUXCR_MSEL( 0x07 ) | CMP_MUXCR_PSEL( _pin );
    CMP2_DACCR = CMP_DACCR_DACEN | CMP_DACCR_VRSEL | CMP_DACCR_VOSEL( tap );
    if ( mode == VLPW ) NVIC_ENABLE_IRQ( IRQ_CMP2 );
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    CMP2_CR1 = CMP_CR1_EN | CMP_CR1_TRIGM;
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    LPTMR0_CSR = 0;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
    LPTMR0_CMR = 1;
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
#else
    CMP2_CR1 = CMP_CR1_EN;
#endif*/
}

/*******************************************************************************
 *  Clear Interrupt Flags
 *******************************************************************************/
void SnoozeCompare::clearIsrFlags( void ) {
    isr( );
}

/*******************************************************************************
 *  Interrupt handler
 *******************************************************************************/
void SnoozeCompare::isr( void ) {
    if ( !( SIM_SCGC4 & SIM_SCGC4_CMP ) ) return;
    if ( *cmpx_scr & CMP_SCR_CFF ) *cmpx_scr = CMP_SCR_CFF;
    if ( *cmpx_scr & CMP_SCR_CFR ) *cmpx_scr = CMP_SCR_CFR;
    if ( mode == VLPW || mode == VLPS ) source = 34;
    
    /*if ( CMP2_SCR & CMP_SCR_CFF ) CMP2_SCR = CMP_SCR_CFF;
    if ( CMP2_SCR & CMP_SCR_CFR ) CMP2_SCR = CMP_SCR_CFR;
    if ( mode == VLPW || mode == VLPS ) source = 34;*/
    
}
