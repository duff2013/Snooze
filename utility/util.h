/*******************************************************************************
 *  util.h
 *  Teensy 3.x/LC
 *
 * Purpose: driver utility functions
 *
 *******************************************************************************/
#ifndef __UTIL_H__
#define __UTIL_H__
/*******************************************************************************/
static volatile bool enable_periph_irq = false;
static volatile int wakeupSource = -1;
static DMAMEM volatile int llwuFlag = -1;
/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    static inline
    void detachInterruptVector(enum IRQ_NUMBER_t irq)
    __attribute__((always_inline, unused));
    
    static inline
    void detachInterruptVector(enum IRQ_NUMBER_t irq) {
        switch ( irq ) {
            case IRQ_RTC_ALARM:
                _VectorsRam[irq + 16] = lptmr_isr;
                break;
            case IRQ_LLWU:
                _VectorsRam[irq + 16] = wakeup_isr;
                break;
            case IRQ_CMP0:
                _VectorsRam[irq + 16] = cmp0_isr;
                break;
            case IRQ_LPTMR:
                _VectorsRam[irq + 16] = lptmr_isr;
                break;
            case IRQ_PORTA:
                _VectorsRam[irq + 16] = porta_isr;
                break;
#if defined(__MK20DX128__) || defined(__MK20DX256__)
            case IRQ_PORTB:
                _VectorsRam[irq + 16] = portb_isr;
                break;
            case IRQ_PORTC:
                _VectorsRam[irq + 16] = portc_isr;
                break;
            case IRQ_PORTD:
                _VectorsRam[irq + 16] = portd_isr;
                break;
            case IRQ_PORTE:
                _VectorsRam[irq + 16] = porte_isr;
                break;
#elif defined(__MKL26Z64__)
            case IRQ_PORTCD:
                _VectorsRam[irq + 16] = portcd_isr;
                break;
#endif
            case IRQ_TSI:
                _VectorsRam[irq + 16] = tsi0_isr;
                break;
            default:
                
                break;
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __UTIL_H__ */