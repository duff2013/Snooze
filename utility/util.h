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
static DMAMEM volatile uint8_t irqEnabledFlag = 0;
#define CMP_IRQ_BIT         0x01
#define RTC_IRQ_BIT         0x02
#define LPTMR_IRQ_BIT       0x04
#define TSI_IRQ_BIT         0x08
#define DIGITALA_IRQ_BIT    0x10
#define DIGITALB_IRQ_BIT    0x20
#define DIGITALC_IRQ_BIT    0x40
#define DIGITALD_IRQ_BIT    0x80
#define DIGITALE_IRQ_BIT    0x100
static volatile uint32_t PCR3;
/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    //--------------------------------------------------------//
    static inline
    void attachDigitalInterrupt( uint8_t pin, int mode )
    __attribute__((always_inline, unused));
    
    static inline
    void attachDigitalInterrupt( uint8_t pin, int mode ) {
        volatile uint32_t *config;
        uint32_t cfg, mask;
        
        if ( pin >= CORE_NUM_DIGITAL ) return;
        switch (mode) {
            case CHANGE:	mask = 0x0B; break;
            case RISING:	mask = 0x09; break;
            case FALLING:	mask = 0x0A; break;
            case LOW:	mask = 0x08; break;
            case HIGH:	mask = 0x0C; break;
            default: return;
        }
        mask = ( mask << 16 ) | 0x01000000;
        config = portConfigRegister( pin );
        cfg = *config;
        cfg &= ~0x000F0000;		// disable any previous interrupt
        *config = cfg;
        cfg |= mask;
        *config = cfg;			// enable the new interrupt
    }
    //--------------------------------------------------------//
    static inline
    void detachDigitalInterrupt( uint8_t pin )
    __attribute__((always_inline, unused));
    
    static inline
    void detachDigitalInterrupt( uint8_t pin ) {
        volatile uint32_t *config;
        config = portConfigRegister( pin );
        *config = ( ( *config & ~0x000F0000 ) | 0x01000000 );
    }
    //--------------------------------------------------------//
    static inline
    void enableHibernate( void )
    __attribute__((always_inline, unused));
    
    static inline
    void enableHibernate( void ) {
        SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
        SIM_SOPT1 |= SIM_SOPT1_USBSSTBY;
        PCR3 = PORTA_PCR3;
        PORTA_PCR3 = PORT_PCR_MUX( 0 );
    }
    //--------------------------------------------------------//
    static inline
    void disableHibernate( void )
    __attribute__((always_inline, unused));
    
    static inline
    void disableHibernate( void ) {
        SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
        SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
        PORTA_PCR3 = PCR3;
    }
    //--------------------------------------------------------//

#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __UTIL_H__ */