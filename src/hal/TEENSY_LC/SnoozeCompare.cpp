/***********************************************************************************
 *  SnoozeCompare.h
 *  Teensy LC
 *
 * Purpose: Compare Driver
 *
 **********************************************************************************/
#if defined(__MKL26Z64__)

#include "SnoozeCompare.h"

#define LLWU_CMP0_MOD       0x08
#define LLWU_CMP1_MOD       0x10
#define LLWU_CMP2_MOD       0x20

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_modules_mask( uint8_t module );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

#define MCG_S_CLKST_SHIFT    ( uint8_t )0x02
#define LPTMR_MCGIRCLK       ( uint8_t )0x00
#define LPTMR_LPO            ( uint8_t )0x01
#define LPTMR_ERCLK32K       ( uint8_t )0x02
#define LPTMR_OSCERCLK       ( uint8_t )0x03
#define MCG_C1_FRDIV4_BIT    ( uint8_t )0x05

volatile uint8_t *SnoozeCompare::cmpx_cr0;
volatile uint8_t *SnoozeCompare::cmpx_cr1;
volatile uint8_t *SnoozeCompare::cmpx_scr;
volatile uint8_t *SnoozeCompare::cmpx_fpr;
volatile uint8_t *SnoozeCompare::cmpx_muxcr;
volatile uint8_t *SnoozeCompare::cmpx_daccr;
void ( * SnoozeCompare::return_cmp_irq ) ( void );
/*******************************************************************************
 *  Save the modes
 *
 *  @param _pin  Teensy pin
 *  @param _type HIGH, LOW
 *  @param val   Threshold Crossing
 *******************************************************************************/
void SnoozeCompare::pinMode( int _pin, int _type, float val ) {
    isUsed = true;
    pin = _pin;
    type = _type;
    threshold_crossing = val;
    if ( pin == 4 ) {
        return;
    }
    else if ( pin == 9 ) {
        return;
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
void SnoozeCompare::disableDriver( uint8_t mode ) {
    if (mode == 0) return;
    if (mode == 1) {
        IRQ_NUMBER_t IRQ_CMP;
        switch (pin) {
            case 11:
                IRQ_CMP = IRQ_CMP0;
                break;
            default:
                IRQ_CMP = IRQ_CMP;
                return;
        }
        if ( return_isr_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_CMP );
        NVIC_SET_PRIORITY( IRQ_CMP, return_priority );
        __disable_irq( );
        attachInterruptVector( IRQ_CMP, return_cmp_irq );// return prev interrupt
        __enable_irq( );
    }

    if ( pin == 11 ) {
        CORE_PIN11_CONFIG = return_core_pin_config[0];
    }
    else if ( pin == 4 ) {
        return;
    }
    else if ( pin == 9 ) {
        return;
    }
    
    LPTMR0_PSR = PSR;
    LPTMR0_CMR = CMR;
    LPTMR0_CSR = CSR;
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
    
    *cmpx_cr0   = CR0;
    *cmpx_cr1   = CR1;
    *cmpx_scr   = SCR;
    *cmpx_fpr   = FPR;
    *cmpx_muxcr = MUXCR;
    *cmpx_daccr = DACCR;
    
    if ( !SIM_SCGC4_clock_active ) SIM_SCGC4 &= ~SIM_SCGC4_CMP;
}

/*******************************************************************************
 *  Enable Driver
 *******************************************************************************/
void SnoozeCompare::enableDriver( uint8_t mode ) {
    if (mode == 0) return;
    if ( mode == 1 ) {
        IRQ_NUMBER_t IRQ_CMP;
        switch (pin) {
            case 11:
                IRQ_CMP = IRQ_CMP0;
                break;
            default:
                IRQ_CMP = IRQ_CMP;
                return;
        }
        return_priority = NVIC_GET_PRIORITY( IRQ_CMP );//get current priority
        
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler mode set priority higher than current handler
        priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_CMP, priority );//set priority to new level
        __disable_irq( );
        return_cmp_irq = _VectorsRam[IRQ_CMP+16];// save prev isr
        attachInterruptVector( IRQ_CMP, wakeup_isr );
        __enable_irq( );
    }
    
    if ( SIM_SCGC4 & SIM_SCGC4_CMP ) SIM_SCGC4_clock_active = true;
    else SIM_SCGC4 |= SIM_SCGC4_CMP;
    
    CR0 = *cmpx_cr0;
    CR1 = *cmpx_cr1;
    SCR = *cmpx_scr;
    FPR = *cmpx_fpr;
    MUXCR = *cmpx_muxcr;
    DACCR = *cmpx_daccr;
    
    uint8_t _pin = 0;
    *cmpx_cr0 = 0;
    *cmpx_cr1 = 0;
    *cmpx_scr = 0;
    
    if ( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) SIM_SCGC5_clock_active = true;
    else SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    PSR = LPTMR0_PSR;
    CMR = LPTMR0_CMR;
    CSR = LPTMR0_CSR;
    
    if ( pin == 11 ) {
        
        if (mode >= 2) {
            llwu_configure_modules_mask( LLWU_CMP0_MOD );
        }
        return_core_pin_config[0] = CORE_PIN11_CONFIG;
        CORE_PIN11_CONFIG = PORT_PCR_MUX( 0 );
        _pin = 0x00;
    }
    // save if isr is already enabled and enable isr if not
    if (mode == 1) {
        IRQ_NUMBER_t IRQ_CMP;
        switch (pin) {
            case 11:
                IRQ_CMP = IRQ_CMP0;
                break;
            default:
                IRQ_CMP = IRQ_CMP;
                return;
        }
        return_isr_enabled = NVIC_IS_ENABLED( IRQ_CMP );
        if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_CMP );
    }
    
    // setup compare
    *cmpx_cr0 = CMP_CR0_FILTER_CNT( 0x07 );
    
    if ( type == CHANGE ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF | CMP_SCR_IER;
    else if ( type == RISING || type == HIGH ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IER;
    else if ( type == FALLING || type == LOW ) *cmpx_scr = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF;
    else return;
    
    uint8_t tap = ( threshold_crossing/0.0515625 ) - 1;
    *cmpx_fpr = 0x00;
    *cmpx_muxcr = CMP_MUXCR_MSEL( 0x07 ) | CMP_MUXCR_PSEL( _pin );
    *cmpx_daccr = CMP_DACCR_DACEN | CMP_DACCR_VRSEL | CMP_DACCR_VOSEL( tap );
    
    // compare needs lptmr to operate in low power with LC, 3.6
    *cmpx_cr1 = CMP_CR1_EN | CMP_CR1_TRIGM;
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    LPTMR0_CSR = 0;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );//LPO Clock
    LPTMR0_CMR = 1;
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
}

/*******************************************************************************
 *  Clear Interrupt Flags
 *******************************************************************************/
void SnoozeCompare::clearIsrFlags( uint32_t ipsr ) {
    if ( !( SIM_SCGC4 & SIM_SCGC4_CMP ) ) return;
    if ( *cmpx_scr & CMP_SCR_CFF ) *cmpx_scr = CMP_SCR_CFF;
    if ( *cmpx_scr & CMP_SCR_CFR ) *cmpx_scr = CMP_SCR_CFR;
}
#endif /* __MKL26Z64__ */
