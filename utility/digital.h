/*******************************************************************************
 *  digital.h
 *  Teensy 3.x/LC
 *
 * Purpose: Digital Pins
 *
 *******************************************************************************/
#ifndef __DIGITAL_H__
#define __DIGITAL_H__
#include <utility/util.h>
/********************************************************************/
typedef struct {
    uint64_t pin = 0;
    uint8_t mode_irqType[33];
    bool state = false;
#if defined(KINETISK)
    void ( * _porta_irq )( void );
    void ( * _portb_irq )( void );
    void ( * _portc_irq )( void );
    void ( * _portd_irq )( void );
    void ( * _porte_irq )( void );
#elif defined(KINETISL)
    void ( * _porta_irq ) ( void );
    void ( * _portcd_irq )( void );
#endif
} digital_mask_t;
/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       digital_configure_pin_mask
     *
     *******************************************************************************/
    static inline
    void digital_configure_pin_mask( uint8_t pin, uint8_t mode, uint8_t type, digital_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void digital_configure_pin_mask( uint8_t pin, uint8_t mode, uint8_t type, digital_mask_t *mask ) {
#if defined(KINETISK)
        mask->_porta_irq = _VectorsRam[IRQ_PORTA+16];
        mask->_portb_irq = _VectorsRam[IRQ_PORTB+16];
        mask->_portc_irq = _VectorsRam[IRQ_PORTC+16];
        mask->_portd_irq = _VectorsRam[IRQ_PORTD+16];
        mask->_porte_irq = _VectorsRam[IRQ_PORTE+16];
#elif defined(KINETISL)
        mask->_porta_irq  = _VectorsRam[IRQ_PORTA+16];
        mask->_portcd_irq = _VectorsRam[IRQ_PORTCD+16];
#endif
        mask->pin = mask->pin | ( ( uint64_t )0x01 << pin );
        mask->mode_irqType[pin] = type;
        mask->mode_irqType[pin] |= mode << 4;
        mask->state = true;
    }
    /*******************************************************************************
     *
     *       digitalISR
     *
     *******************************************************************************/    
    static inline
    void digitalISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void digitalISR( void ) {
        uint32_t isfr_a = PORTA_ISFR;
        PORTA_ISFR = isfr_a;
#if defined(__MK20DX128__) || defined(__MK20DX256__)
        uint32_t isfr_b = PORTB_ISFR;
        uint32_t isfr_e = PORTE_ISFR;
        PORTB_ISFR = isfr_b;
        PORTE_ISFR = isfr_e;
#endif
        uint32_t isfr_c = PORTC_ISFR;
        uint32_t isfr_d = PORTD_ISFR;
        PORTC_ISFR = isfr_c;
        PORTD_ISFR = isfr_d;
        
        if ( isfr_a & CORE_PIN3_BITMASK )       { wakeupSource = 3;  return; }
        else if ( isfr_a & CORE_PIN4_BITMASK )  { wakeupSource = 4;  return; }
#if defined(__MK20DX128__) || defined(__MK20DX256__)
        else if ( isfr_a & CORE_PIN24_BITMASK ) { wakeupSource = 24; return; }
        else if ( isfr_a & CORE_PIN33_BITMASK ) { wakeupSource = 33; return; }

        if ( isfr_b & CORE_PIN0_BITMASK )       { wakeupSource = 0;  return; }
        else if ( isfr_b & CORE_PIN1_BITMASK )  { wakeupSource = 1;  return; }
        else if ( isfr_b & CORE_PIN16_BITMASK ) { wakeupSource = 16; return; }
        else if ( isfr_b & CORE_PIN17_BITMASK ) { wakeupSource = 17; return; }
        else if ( isfr_b & CORE_PIN18_BITMASK ) { wakeupSource = 18; return; }
        else if ( isfr_b & CORE_PIN19_BITMASK ) { wakeupSource = 19; return; }
        else if ( isfr_b & CORE_PIN25_BITMASK ) { wakeupSource = 25; return; }
        else if ( isfr_b & CORE_PIN32_BITMASK ) { wakeupSource = 32; return; }
#endif
        if ( isfr_c & CORE_PIN9_BITMASK )       { wakeupSource = 9;  return; }
        else if ( isfr_c & CORE_PIN10_BITMASK ) { wakeupSource = 10; return; }
        else if ( isfr_c & CORE_PIN11_BITMASK ) { wakeupSource = 11; return; }
        else if ( isfr_c & CORE_PIN12_BITMASK ) { wakeupSource = 12; return; }
        else if ( isfr_c & CORE_PIN13_BITMASK ) { wakeupSource = 13; return; }
        else if ( isfr_c & CORE_PIN15_BITMASK ) { wakeupSource = 15; return; }
        else if ( isfr_c & CORE_PIN22_BITMASK ) { wakeupSource = 22; return; }
        else if ( isfr_c & CORE_PIN23_BITMASK ) { wakeupSource = 23; return; }
#if defined(__MK20DX128__) || defined(__MK20DX256__)
        else if ( isfr_c & CORE_PIN27_BITMASK ) { wakeupSource = 27; return; }
        else if ( isfr_c & CORE_PIN28_BITMASK ) { wakeupSource = 28; return; }
        else if ( isfr_c & CORE_PIN29_BITMASK ) { wakeupSource = 29; return; }
        else if ( isfr_c & CORE_PIN30_BITMASK ) { wakeupSource = 30; return; }
#endif
        if ( isfr_d & CORE_PIN2_BITMASK )       { wakeupSource = 2;  return; }
        else if ( isfr_d & CORE_PIN5_BITMASK )  { wakeupSource = 5;  return; }
        else if ( isfr_d & CORE_PIN6_BITMASK )  { wakeupSource = 6;  return; }
        else if ( isfr_d & CORE_PIN7_BITMASK )  { wakeupSource = 7;  return; }
        else if ( isfr_d & CORE_PIN8_BITMASK )  { wakeupSource = 8;  return; }
        else if ( isfr_d & CORE_PIN14_BITMASK ) { wakeupSource = 14; return; }
        else if ( isfr_d & CORE_PIN20_BITMASK ) { wakeupSource = 20; return; }
        else if ( isfr_d & CORE_PIN21_BITMASK ) { wakeupSource = 21; return; }
        
#if defined(__MK20DX128__) || defined(__MK20DX256__)
        if ( isfr_e & CORE_PIN26_BITMASK )      { wakeupSource = 26; return; }
        else if ( isfr_e & CORE_PIN31_BITMASK ) { wakeupSource = 31; return; }
#endif
    }
    
    /*******************************************************************************
     *
     *       digital_set
     *
     *******************************************************************************/
    static inline
    void digital_set( digital_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void digital_set( digital_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) {
#if defined(__MK20DX128__) || defined(__MK20DX256__)
            attachInterruptVector( IRQ_PORTA, digitalISR );
            attachInterruptVector( IRQ_PORTB, digitalISR );
            attachInterruptVector( IRQ_PORTC, digitalISR );
            attachInterruptVector( IRQ_PORTD, digitalISR );
            attachInterruptVector( IRQ_PORTE, digitalISR );
#elif defined(__MKL26Z64__)
            attachInterruptVector( IRQ_PORTA, digitalISR );
            attachInterruptVector( IRQ_PORTCD, digitalISR );
#endif
        }
        uint64_t _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            int pin  = 63 - __builtin_clzll( _pin );
            int mode = mask->mode_irqType[pin] >> 4;
            int type = mask->mode_irqType[pin] & 0x0F;
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
#ifdef KINETISK
            *portModeRegister( pin ) = 0;
#else
            *portModeRegister( pin ) &= ~digitalPinToBitMask( pin ); // TODO: atomic
#endif
            if ( mode == 0 ) *config = PORT_PCR_MUX( 1 );
            else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
            
            if ( enable_periph_irq ) attachDigitalInterrupt( pin, type );
            _pin &= ~( ( uint64_t )1<<pin );
        }
    }
    /*******************************************************************************
     *
     *       digital_disable
     *
     ******************************************************************************/
    static inline
    void digital_disable( digital_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void digital_disable( digital_mask_t *mask ) {
        if ( mask->state == false ) return;
        if (  enable_periph_irq ) {
#if defined(__MK20DX128__) || defined(__MK20DX256__)
            attachInterruptVector( IRQ_PORTA, mask->_porta_irq );
            attachInterruptVector( IRQ_PORTB, mask->_portb_irq );
            attachInterruptVector( IRQ_PORTC, mask->_portc_irq );
            attachInterruptVector( IRQ_PORTD, mask->_portd_irq );
            attachInterruptVector( IRQ_PORTE, mask->_porte_irq );
#elif defined(__MKL26Z64__)
            attachInterruptVector( IRQ_PORTA, mask->_porta_irq );
            attachInterruptVector( IRQ_PORTCD, mask->_portcd_irq );
#endif
        }
        uint64_t _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            int pin = 63 - __builtin_clzll( _pin );
            if ( enable_periph_irq ) detachDigitalInterrupt( pin );
            _pin &= ~( ( uint64_t )1<<pin );
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __DIGITAL_H__ */