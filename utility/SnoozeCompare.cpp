/***********************************************************************************
 *  SnoozeCompare.h
 *  Teensy 3.x/LC
 *
 * Purpose: Compare Driver
 *
 ***********************************************************************************/
#include "Arduino.h"
#include "SnoozeCompare.h"
#include "wake.h"

#ifndef CMP_CR1_TRIGM
  #define CMP_CR1_TRIGM (uint8_t)0x20 // Trigger Mode Enable
#endif
/*******************************************************************************
 *  <#Description#>
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
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeCompare::disableDriver( void ) {
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
        NVIC_DISABLE_IRQ( IRQ_CMP0 );
        NVIC_SET_PRIORITY( IRQ_CMP0, 128 );//return priority to core value
        __disable_irq( );
        attachInterruptVector( IRQ_CMP0, return_cmp0_irq );// return prev interrupt
        __enable_irq( );
    }

    if ( pin == 11 ) {
        CORE_PIN11_CONFIG = return_core_pin_config[0];
    }
    else if ( pin == 12 ) {
        CORE_PIN12_CONFIG = return_core_pin_config[1];
    }
    
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    LPTMR0_PSR = PSR;
    LPTMR0_CMR = CMR;
    LPTMR0_CSR = CSR;
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#endif
    
    CMP0_CR0 = CR0;
    CMP0_CR1 = CR1;
    CMP0_SCR = SCR;
    CMP0_FPR = FPR;
    CMP0_MUXCR = MUXCR;
    CMP0_DACCR = DACCR;
    if ( !SIM_SCGC4_clock_active ) SIM_SCGC4 &= ~SIM_SCGC4_CMP;
    //CMP0_CR0 = 0x00;
    //CMP0_CR1 = 0x00;
    //SIM_SCGC4 &= ~SIM_SCGC4_CMP;
    

}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeCompare::enableDriver( void ) {
    if ( mode == RUN_LP ) { return; }
    if ( mode == VLPW || mode == VLPS ) {
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler set priority higher than current handler
        priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_CMP0, priority );//set priority to new level
        
        __disable_irq( );
        return_cmp0_irq = _VectorsRam[IRQ_CMP0+16];// save prev isr
        attachInterruptVector( IRQ_CMP0, isr );
        __enable_irq( );
        NVIC_ENABLE_IRQ( IRQ_CMP0 );
    }
    
    if ( SIM_SCGC4 & SIM_SCGC4_CMP ) SIM_SCGC4_clock_active = true;
    else SIM_SCGC4 |= SIM_SCGC4_CMP;
    
    CR0 = CMP0_CR0;
    CR1 = CMP0_CR1;
    SCR = CMP0_SCR;
    FPR = CMP0_FPR;
    MUXCR = CMP0_MUXCR;
    DACCR = CMP0_DACCR;
    
    uint8_t _pin;
    CMP0_CR0 = 0x00;
    CMP0_CR1 = 0x00;
    CMP0_SCR = 0x00;
    
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
    else if ( pin == 12 ) {
        if ( mode >= LLS ) llwu_configure_modules_mask( LLWU_CMP1_MOD );
        return_core_pin_config[1] = CORE_PIN12_CONFIG;
        CORE_PIN12_CONFIG = PORT_PCR_MUX( 0 );
        _pin = 0x01;
    }
    else {
        SIM_SCGC4 &= ~SIM_SCGC4_CMP;
        return;
    }
    CMP0_CR0 = CMP_CR0_FILTER_CNT( 0x07 );
    if ( type == CHANGE ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF | CMP_SCR_IER;
    else if ( type == RISING || type == HIGH ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IER;
    else if ( type == FALLING || type == LOW ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF;
    else return;
    uint8_t tap = ( threshold_crossing/0.0515625 ) - 1;
    CMP0_FPR = 0x00;
    CMP0_MUXCR = CMP_MUXCR_MSEL(0x07) | CMP_MUXCR_PSEL( _pin );
    CMP0_DACCR = CMP_DACCR_DACEN | CMP_DACCR_VRSEL | CMP_DACCR_VOSEL( tap );
    if ( mode == VLPW ) NVIC_ENABLE_IRQ( IRQ_CMP0 );
#if defined(__MKL26Z64__) || defined(__MK66FX1M0__)
    CMP0_CR1 = CMP_CR1_EN | CMP_CR1_TRIGM;
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    LPTMR0_CSR = 0;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
    LPTMR0_CMR = 1;
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
#else
    CMP0_CR1 = CMP_CR1_EN;
#endif
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeCompare::clearIsrFlags( void ) {
    isr( );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeCompare::isr( void ) {
    if ( !( SIM_SCGC4 & SIM_SCGC4_CMP ) ) return;
    if ( CMP0_SCR & CMP_SCR_CFF ) CMP0_SCR = CMP_SCR_CFF;
    if ( CMP0_SCR & CMP_SCR_CFR ) CMP0_SCR = CMP_SCR_CFR;
    if ( mode == VLPW || mode == VLPS ) source = 34;
}