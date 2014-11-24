/*******************************************************************************
 *  util.h
 *  Teensy3.x
 *
 * Purpose: driver utility functions
 *
 *******************************************************************************/
#ifndef __UTIL_H__
#define __UTIL_H__
/*******************************************************************************/
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
        switch (irq) {
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