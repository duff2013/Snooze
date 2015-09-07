 /***********************************************************************************
 *  lptmr.h
 *  Teensy 3.x/LC
 *
 * Purpose: Low Power Timer
 *
 ***********************************************************************************/
#ifndef __LPTMR_H__
#define __LPTMR_H__
/***********************************************************************************/
#include <utility/util.h>
/***********************************************************************************/
static void ( * return_lptmr_irq ) ( void );
/***********************************************************************************/
typedef struct {
    uint16_t period;
    bool state = false;
} lptmr_mask_t;
/***********************************************************************************/
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
        SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL( 3 );
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS(1);//LPO Clock
        LPTMR0_CSR = LPTMR_CSR_TCF;
        //SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
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
       
        if ( !( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) || !( irqEnabledFlag & LPTMR_IRQ_BIT ) ) return;
        LPTMR0_CSR = LPTMR_CSR_TCF;
        irqEnabledFlag &= ~LPTMR_IRQ_BIT;
        if ( enable_periph_irq ) wakeupSource = 36;
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
        if ( enable_periph_irq ) {
            int priority = nvic_execution_priority( );// get current priority
            // if running from handler mode set priority higher than current handler
            priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
            NVIC_SET_PRIORITY( IRQ_LPTMR, priority );//set priority to new level
            __disable_irq( );
            return_lptmr_irq = _VectorsRam[IRQ_LPTMR+16];// save prev isr
            attachInterruptVector( IRQ_LPTMR, lptmrISR );
            __enable_irq( );
        }
        lptmr_init( );
        LPTMR0_CMR = mask->period;
        if( enable_periph_irq ) NVIC_ENABLE_IRQ( IRQ_LPTMR );
        LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TIE | LPTMR_CSR_TCF;
        irqEnabledFlag |= LPTMR_IRQ_BIT;
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
        LPTMR0_CSR = 0;
        SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
        if ( enable_periph_irq ) {
            NVIC_DISABLE_IRQ( IRQ_LPTMR );//return priority to core value
            NVIC_SET_PRIORITY( IRQ_LPTMR, 128 );// return priority core level
            __disable_irq( );
            attachInterruptVector( IRQ_LPTMR, return_lptmr_irq );// return prev interrupt
            __enable_irq( );
        }
    }
#ifdef __cplusplus
}
#endif
/***********************************************************************************/
#endif /* defined(__LPTMR_H__) */
