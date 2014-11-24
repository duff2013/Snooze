/*******************************************************************************
 *  lptmr.h
 *  Teensy3.x
 *
 * Purpose: Low Power Timer
 *
 *******************************************************************************/
#ifndef __LPTMR_H__
#define __LPTMR_H__
/*******************************************************************************/
#include <utility/util.h>

#define LPTMR_MCGIRCLK       (uint8_t)0x00
#define LPTMR_LPO            (uint8_t)0x01
#define LPTMR_ERCLK32K       (uint8_t)0x02
#define LPTMR_OSCERCLK       (uint8_t)0x03
#define LPTMR_PRESCALE(n)    (uint8_t)(((n) & 0x0F) << 0x03)

typedef struct {
    uint16_t period;
    bool state = false;
} lptmr_mask_t;
/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       lptmr_init
     *
     *******************************************************************************/
    static inline
    void lptmr_init( void )
    __attribute__((always_inline, unused));
    
    static inline
    void lptmr_init( void ) {
        SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(3);
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        LPTMR0_CSR = LPTMR_CSR_TCF;
        LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_LPO;
        SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
    }
    /*******************************************************************************
     *
     *       lptmr_configure_pin_mask
     *
     *******************************************************************************/
    static inline
    void lptmr_configure_period_mask( uint16_t period, lptmr_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lptmr_configure_period_mask( uint16_t period, lptmr_mask_t *mask ) {
        mask->period = period;
        mask->state = true;
    }
    /*******************************************************************************
     *
     *       lptmr_isr
     *
     *******************************************************************************/
    static inline
    void lptmrISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void lptmrISR( void ) {
        if ( !(SIM_SCGC5 & SIM_SCGC5_LPTIMER) ) return;
        LPTMR0_CSR = LPTMR_CSR_TCF;
        wakeupSource = 36;
    }
    /*******************************************************************************
     *
     *       lptmr_set
     *
     *******************************************************************************/
    static inline
    void lptmr_set( lptmr_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lptmr_set( lptmr_mask_t *mask ) {
        if ( mask->state == false ) return;
        attachInterruptVector( IRQ_LPTMR, lptmrISR );
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        NVIC_ENABLE_IRQ(IRQ_LPTMR);
        LPTMR0_CSR = LPTMR_CSR_TIE | LPTMR_CSR_TCF;
        LPTMR0_CMR = mask->period;
        LPTMR0_CSR |= LPTMR_CSR_TEN;
    }
    /*******************************************************************************
     *
     *       lptmr_stop
     *
     *******************************************************************************/
    static inline
    void lptmr_disable( lptmr_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lptmr_disable( lptmr_mask_t *mask ) {
        if ( mask->state == false ) return;
        SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
        NVIC_DISABLE_IRQ( IRQ_LPTMR );
        detachInterruptVector( IRQ_LPTMR );
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* defined(__LPTMR_H__) */
