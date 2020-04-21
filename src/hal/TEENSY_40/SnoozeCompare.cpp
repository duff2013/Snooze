/***********************************************************************************
 *  SnoozeCompare.h
 *  Teensy 4.0
 *
 * Purpose: Compare Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#include "SnoozeCompare.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern int nvic_execution_priority( void );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

#define CMP_CR0_FILTER_CNT(n)   (uint8_t)(((n) & 0x07) << 4)
#define CMP_CR0_HYSTCTR(n)      (uint8_t)(((n) & 0x03) << 0)

#define CMP_CR1_SE              (uint8_t)0x80 // Sample Enable
#define CMP_CR1_WE              (uint8_t)0x40 // Windowing Enable
#define CMP_CR1_TRIGM           (uint8_t)0x20 // Trigger Mode Enable
#define CMP_CR1_PMODE           (uint8_t)0x10 // Power Mode Select
#define CMP_CR1_INV             (uint8_t)0x08 // Comparator INVERT
#define CMP_CR1_COS             (uint8_t)0x04 // Comparator Output Select
#define CMP_CR1_OPE             (uint8_t)0x02 // Comparator Output Pin Enable
#define CMP_CR1_EN              (uint8_t)0x01 // Comparator Module Enable

#define CMP_SCR_DMAEN           (uint8_t)0x40 // DMA Enable Control
#define CMP_SCR_IER             (uint8_t)0x10 // Comparator Interrupt Enable Rising
#define CMP_SCR_IEF             (uint8_t)0x08 // Comparator Interrupt Enable Falling
#define CMP_SCR_CFR             (uint8_t)0x04 // Analog Comparator Flag Rising
#define CMP_SCR_CFF             (uint8_t)0x02 // Analog Comparator Flag Falling
#define CMP_SCR_COUT            (uint8_t)0x01 // Analog Comparator Output

#define CMP_DACCR_DACEN         (uint8_t)0x80 // DAC Enable
#define CMP_DACCR_VRSEL         (uint8_t)0x40 // Supply Voltage Reference Source Select
#define CMP_DACCR_VOSEL(n)      (uint8_t)(((n) & 0x3F) << 0) // DAC Output Voltage Select

#define CMP_MUXCR_PSTM          (uint8_t)0x40 // Pass Through Mode Enable
#define CMP_MUXCR_PSEL(n)       (uint8_t)(((n) & 0x07) << 3) // Plus Input Mux Control
#define CMP_MUXCR_MSEL(n)       (uint8_t)(((n) & 0x07) << 0) // Minus Input Mux Control

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
    switch (pin) {
        case 0:
            cmpx_cr0   = &CMP4_CR0;
            cmpx_cr1   = &CMP4_CR1;
            cmpx_scr   = &CMP4_SCR;
            cmpx_fpr   = &CMP4_FPR;
            cmpx_muxcr = &CMP4_MUXCR;
            cmpx_daccr = &CMP4_DACCR;
            break;
        case 1:
            cmpx_cr0   = &CMP3_CR0;
            cmpx_cr1   = &CMP3_CR1;
            cmpx_scr   = &CMP3_SCR;
            cmpx_fpr   = &CMP3_FPR;
            cmpx_muxcr = &CMP3_MUXCR;
            cmpx_daccr = &CMP3_DACCR;
            break;
        case 14:
            cmpx_cr0   = &CMP1_CR0;
            cmpx_cr1   = &CMP1_CR1;
            cmpx_scr   = &CMP1_SCR;
            cmpx_fpr   = &CMP1_FPR;
            cmpx_muxcr = &CMP1_MUXCR;
            cmpx_daccr = &CMP1_DACCR;
            break;
        case 15:
            cmpx_cr0   = &CMP2_CR0;
            cmpx_cr1   = &CMP2_CR1;
            cmpx_scr   = &CMP2_SCR;
            cmpx_fpr   = &CMP2_FPR;
            cmpx_muxcr = &CMP2_MUXCR;
            cmpx_daccr = &CMP2_DACCR;
            break;
        case 16:
            cmpx_cr0   = &CMP1_CR0;
            cmpx_cr1   = &CMP1_CR1;
            cmpx_scr   = &CMP1_SCR;
            cmpx_fpr   = &CMP1_FPR;
            cmpx_muxcr = &CMP1_MUXCR;
            cmpx_daccr = &CMP1_DACCR;
            break;
        case 17:
            cmpx_cr0   = &CMP1_CR0;
            cmpx_cr1   = &CMP1_CR1;
            cmpx_scr   = &CMP1_SCR;
            cmpx_fpr   = &CMP1_FPR;
            cmpx_muxcr = &CMP1_MUXCR;
            cmpx_daccr = &CMP1_DACCR;
            break;
        case 18:
            cmpx_cr0   = &CMP1_CR0;
            cmpx_cr1   = &CMP1_CR1;
            cmpx_scr   = &CMP1_SCR;
            cmpx_fpr   = &CMP1_FPR;
            cmpx_muxcr = &CMP1_MUXCR;
            cmpx_daccr = &CMP1_DACCR;
            break;
        case 19:
            cmpx_cr0   = &CMP4_CR0;
            cmpx_cr1   = &CMP4_CR1;
            cmpx_scr   = &CMP4_SCR;
            cmpx_fpr   = &CMP4_FPR;
            cmpx_muxcr = &CMP4_MUXCR;
            cmpx_daccr = &CMP4_DACCR;
            break;
        case 20:
            cmpx_cr0   = &CMP4_CR0;
            cmpx_cr1   = &CMP4_CR1;
            cmpx_scr   = &CMP4_SCR;
            cmpx_fpr   = &CMP4_FPR;
            cmpx_muxcr = &CMP4_MUXCR;
            cmpx_daccr = &CMP4_DACCR;
            break;
        case 21:
            cmpx_cr0   = &CMP1_CR0;
            cmpx_cr1   = &CMP1_CR1;
            cmpx_scr   = &CMP1_SCR;
            cmpx_fpr   = &CMP1_FPR;
            cmpx_muxcr = &CMP1_MUXCR;
            cmpx_daccr = &CMP1_DACCR;
            break;
        case 22:
            cmpx_cr0   = &CMP2_CR0;
            cmpx_cr1   = &CMP2_CR1;
            cmpx_scr   = &CMP2_SCR;
            cmpx_fpr   = &CMP2_FPR;
            cmpx_muxcr = &CMP2_MUXCR;
            cmpx_daccr = &CMP2_DACCR;
            break;
        case 23:
            cmpx_cr0   = &CMP3_CR0;
            cmpx_cr1   = &CMP3_CR1;
            cmpx_scr   = &CMP3_SCR;
            cmpx_fpr   = &CMP3_FPR;
            cmpx_muxcr = &CMP3_MUXCR;
            cmpx_daccr = &CMP3_DACCR;
            break;
        default:
            break;
    }
}

/*******************************************************************************
 *  Disable Driver
 *******************************************************************************/
void SnoozeCompare::disableDriver( uint8_t mode ) {
    if ( mode == 0 ) return;
    IRQ_NUMBER_t IRQ_CMP;
    if ( mode == 1 ) {
        switch ( pin ) {
            case 0:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 1:
                IRQ_CMP = IRQ_ACMP3;
                break;
            case 14:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 15:
                IRQ_CMP = IRQ_ACMP2;
                break;
            case 16:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 17:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 18:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 19:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 20:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 21:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 22:
                IRQ_CMP = IRQ_ACMP2;
                break;
            case 23:
                IRQ_CMP = IRQ_ACMP3;
                break;
            default:
                IRQ_CMP = IRQ_CMP;
                break;
        }
        if ( return_isr_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_CMP );
        NVIC_SET_PRIORITY( IRQ_CMP, return_priority );
        __disable_irq( );
        attachInterruptVector( IRQ_CMP, return_cmp_irq );// return prev interrupt
        __enable_irq( );
    }
    switch (pin) {
        case 0:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0xC000000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN0_CONFIG = return_core_pin_config[0];
            break;
        case 1:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x3000000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN1_CONFIG = return_core_pin_config[1];
            break;
        case 14:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x300000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN14_CONFIG = return_core_pin_config[2];
            break;
        case 15:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0xC00000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN15_CONFIG = return_core_pin_config[3];
            break;
        case 16:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x300000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN16_CONFIG = return_core_pin_config[4];
            break;
        case 17:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x300000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN17_CONFIG = return_core_pin_config[5];
            break;
        case 18:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x300000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN18_CONFIG = return_core_pin_config[6];
            break;
        case 19:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0xC000000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN19_CONFIG = return_core_pin_config[7];
            break;
        case 20:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0xC000000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN20_CONFIG = return_core_pin_config[8];
            break;
        case 21:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x300000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN21_CONFIG = return_core_pin_config[9];
            break;
        case 22:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0xC00000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN22_CONFIG = return_core_pin_config[10];
            break;
        case 23:
            if ( CCM_CCGR3_clock_active == false ) CCM_CCGR3 &= ~0x3000000;
            else CCM_CCGR3_clock_active = false;
            CORE_PIN23_CONFIG = return_core_pin_config[11];
            break;
        default:

            break;
    }
    
    *cmpx_cr0   = CR0;
    *cmpx_cr1   = CR1;
    *cmpx_scr   = SCR;
    *cmpx_fpr   = FPR;
    *cmpx_muxcr = MUXCR;
    *cmpx_daccr = DACCR;
}

/*******************************************************************************
 *  Enable Driver
 *******************************************************************************/
void SnoozeCompare::enableDriver( uint8_t mode ) {
    if (mode == 0) return;
    IRQ_NUMBER_t IRQ_CMP;
    if ( mode == 1 ) {
        switch ( pin ) {
            case 0:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 1:
                IRQ_CMP = IRQ_ACMP3;
                break;
            case 14:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 15:
                IRQ_CMP = IRQ_ACMP2;
                break;
            case 16:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 17:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 18:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 19:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 20:
                IRQ_CMP = IRQ_ACMP4;
                break;
            case 21:
                IRQ_CMP = IRQ_ACMP1;
                break;
            case 22:
                IRQ_CMP = IRQ_ACMP2;
                break;
            case 23:
                IRQ_CMP = IRQ_ACMP3;
                break;
            default:
                IRQ_CMP = IRQ_CMP;
                break;
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
    
    // save if isr is already enabled and enable isr if not
    switch ( pin ) {
        case 0:
        {
            if ( CCM_CCGR3 & 0xC000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP4( CCM_CCGR_ON );
            return_core_pin_config[0] = CORE_PIN0_CONFIG;
            CORE_PIN0_CONFIG = 0;
            _pin = 0x04;
        }
            break;
        case 1:
        {
            if ( CCM_CCGR3 & 0x3000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP3( CCM_CCGR_ON );
            return_core_pin_config[1] = CORE_PIN1_CONFIG;
            CORE_PIN1_CONFIG = 1;
            _pin = 0x04;
        }
            break;
        case 14:
        {
            if ( CCM_CCGR3 & 0x300000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP1( CCM_CCGR_ON );
            return_core_pin_config[2] = CORE_PIN14_CONFIG;
            CORE_PIN14_CONFIG = 0;
            _pin = 0x03;
        }
            break;
        case 15:
        {
            if ( CCM_CCGR3 & 0xC00000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP2( CCM_CCGR_ON );
            return_core_pin_config[3] = CORE_PIN15_CONFIG;
            CORE_PIN15_CONFIG = 0;
            _pin = 0x03;
        }
            break;
        case 16:
        {
            if ( CCM_CCGR3 & 0x300000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP1( CCM_CCGR_ON );
            return_core_pin_config[4] = CORE_PIN16_CONFIG;
            CORE_PIN16_CONFIG = 0;
            _pin = 0x05;
        }
            break;
        case 17:
        {
            if ( CCM_CCGR3 & 0x300000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP1( CCM_CCGR_ON );
            return_core_pin_config[5] = CORE_PIN17_CONFIG;
            CORE_PIN17_CONFIG = 0;
            _pin = 0x01;
        }
            break;
        case 18:
        {
            if ( CCM_CCGR3 & 0x300000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP1( CCM_CCGR_ON );
            return_core_pin_config[6] = CORE_PIN18_CONFIG;
            CORE_PIN18_CONFIG = 0;
            _pin = 0x00;
        }
            break;
        case 19:
        {
            if ( CCM_CCGR3 & 0xC000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP4( CCM_CCGR_ON );
            return_core_pin_config[7] = CORE_PIN19_CONFIG;
            CORE_PIN19_CONFIG = 0;
            _pin = 0x02;
        }
            break;
        case 20:
        {
            if ( CCM_CCGR3 & 0xC000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP4( CCM_CCGR_ON );
            return_core_pin_config[8] = CORE_PIN20_CONFIG;
            CORE_PIN20_CONFIG = 0;
            _pin = 0x05;
        }
            break;
        case 21:
        {
            if ( CCM_CCGR3 & 0x300000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP1( CCM_CCGR_ON );
            return_core_pin_config[9] = CORE_PIN21_CONFIG;
            CORE_PIN21_CONFIG = 0;
            _pin = 0x06;
        }
            break;
        case 22:
        {
            if ( CCM_CCGR3 & 0x3000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP2( CCM_CCGR_ON );
            return_core_pin_config[10] = CORE_PIN22_CONFIG;
            CORE_PIN22_CONFIG = 1;
            _pin = 0x05;
        }
            break;
        case 23:
        {
            if ( CCM_CCGR3 & 0x3000000 ) CCM_CCGR3_clock_active = true;
            else CCM_CCGR3 |= CCM_CCGR3_ACMP3( CCM_CCGR_ON );
            return_core_pin_config[11] = CORE_PIN23_CONFIG;
            CORE_PIN23_CONFIG = 1;
            _pin = 0x05;
        }
            break;
        default:
            break;
    }
    return_isr_enabled = NVIC_IS_ENABLED( IRQ_CMP );
    if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_CMP );
    
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
    *cmpx_cr1 = CMP_CR1_EN;
}

/*******************************************************************************
 *  Clear Interrupt Flags
 *******************************************************************************/
void SnoozeCompare::clearIsrFlags( uint32_t ipsr ) {
    *cmpx_scr |= CMP_SCR_CFR;
    *cmpx_scr |= CMP_SCR_CFF;
}
#endif /* __IMXRT1062__ */
