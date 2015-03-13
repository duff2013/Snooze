/*******************************************************************************
 *  cmp.h
 *  Teensy 3.x/LC
 *
 * Purpose: Analog Compare
 *
 *******************************************************************************/
#ifndef __CMP_H__
#define __CMP_H__
/*******************************************************************************/
#include <utility/util.h>

#define CMP_CR0_HYSTCTR(n)      (uint8_t)(((n) & 0x03) << 0)
#define CMP_CR0_FILTER_CNT(n)   (uint8_t)(((n) & 0x07) << 4)

#define CMP_CR1_EN              (uint8_t)0x01
#define CMP_CR1_OPE             (uint8_t)0x02
#define CMP_CR1_COS             (uint8_t)0x04
#define CMP_CR1_INV             (uint8_t)0x08
#define CMP_CR1_PMODE           (uint8_t)0x10
#define CMP_CR1_WE              (uint8_t)0x40
#define CMP_CR1_SE              (uint8_t)0x80

#define CMP_SCR_COUT            (uint8_t)0x01
#define CMP_SCR_CFF             (uint8_t)0x02
#define CMP_SCR_CFR             (uint8_t)0x04
#define CMP_SCR_IEF             (uint8_t)0x08
#define CMP_SCR_IER             (uint8_t)0x10
#define CMP_SCR_WE              (uint8_t)0x40

#define CMP_DACCR_VOSEL(n)      (uint8_t)(((n) & 0x3F) << 0)
#define CMP_DACCR_VRSEL         (uint8_t)0x40
#define CMP_DACCR_DACEN         (uint8_t)0x80

#define CMP_MUXCR_MSEL(n)       (uint8_t)(((n) & 0x07) << 0)
#define CMP_MUXCR_PSEL(n)       (uint8_t)(((n) & 0x07) << 3)
#define CMP_MUXCR_PSTM          (uint8_t)0x40

typedef struct {
    float threshold_crossing;
    uint8_t pin;
    uint8_t mode;
    uint8_t type;
    bool state = false;
} cmp_mask_t;
/********************************************************************/
#include "Arduino.h"
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       cmp_configure_pin_mask
     *
     *******************************************************************************/
    static inline
    void cmp_configure_pin_mask( uint8_t pin, uint8_t mode, uint8_t type, float threshold, cmp_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void cmp_configure_pin_mask( uint8_t pin, uint8_t mode, uint8_t type, float threshold, cmp_mask_t *mask ) {
        mask->pin = pin;
        mask->mode = mode;
        mask->type = type;
        mask->threshold_crossing = threshold;
        mask->state = true;
    }
    /*******************************************************************************
     *
     *       cmp0ISR
     *
     *******************************************************************************/
    static inline
    void cmp0ISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void cmp0ISR( void ) {
        if ( !(SIM_SCGC4 & SIM_SCGC4_CMP) ) return;
        if ( CMP0_SCR & CMP_SCR_CFF ) CMP0_SCR = CMP_SCR_CFF;
        if ( CMP0_SCR & CMP_SCR_CFR ) CMP0_SCR = CMP_SCR_CFR;
#if defined(KINETISL)
        //LPTMR0_CSR = LPTMR_CSR_TCF;
        //SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#endif
        wakeupSource = 34;
    }
    /*******************************************************************************
     *
     *       cmp_set
     *
     *******************************************************************************/
    static inline
    void cmp_set( cmp_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void cmp_set( cmp_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) attachInterruptVector( IRQ_CMP0, cmp0ISR );
        uint8_t _pin;
        SIM_SCGC4 |= SIM_SCGC4_CMP;
        CMP0_CR0 = 0x00;
        CMP0_CR1 = 0x00;
        CMP0_SCR = 0x00;
        
        if ( mask->pin == 11 ) {
            CORE_PIN11_CONFIG = PORT_PCR_MUX(0);
            _pin = 0x00;
        }
        else if ( mask->pin == 12 ) {
            CORE_PIN12_CONFIG = PORT_PCR_MUX(0);
            _pin = 0x01;
        }
        else {
            SIM_SCGC4 &= ~SIM_SCGC4_CMP;
            return;
        }
        CMP0_CR0 = CMP_CR0_FILTER_CNT( 0x07 );
        if ( mask->type == CHANGE ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF | CMP_SCR_IER;
        else if ( mask->type == RISING || mask->type == HIGH ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IER;
        else if ( mask->type == FALLING || mask->type == LOW ) CMP0_SCR = CMP_SCR_CFF | CMP_SCR_CFR | CMP_SCR_IEF;
        else return;
        uint8_t tap = ( mask->threshold_crossing/0.0515625 ) - 1;
        CMP0_FPR = 0x00;
        CMP0_MUXCR = CMP_MUXCR_MSEL(0x07) | CMP_MUXCR_PSEL(_pin);
        CMP0_DACCR = CMP_DACCR_DACEN | CMP_DACCR_VRSEL | CMP_DACCR_VOSEL( tap );
        if ( enable_periph_irq ) NVIC_ENABLE_IRQ( IRQ_CMP0 );
        CMP0_CR1 = CMP_CR1_EN;
        //SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        //LPTMR0_CMR = 1;
        //LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
    }
    /*******************************************************************************
     *
     *       cmp_disable
     *
     *******************************************************************************/
    static inline
    void cmp_disable( cmp_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void cmp_disable( cmp_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) {
            NVIC_DISABLE_IRQ( IRQ_CMP0 );
            detachInterruptVector( IRQ_CMP0 );
        }
        CMP0_CR0 = 0x00;
        CMP0_CR1 = 0x00;
        SIM_SCGC4 &= ~SIM_SCGC4_CMP;
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __CMP_H__ */