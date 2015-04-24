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
#if defined(KINETISK)
    uint64_t pin = 0;
    uint8_t mode_irqType[33];
    bool state = false;
#elif defined(KINETISL)
    uint32_t pin = 0;
    uint8_t mode_irqType[23];
    bool state = false;
#endif
} digital_mask_t;

#if defined(KINETISK)
    static void ( * return_porta_irq ) ( void );
    static void ( * return_portb_irq ) ( void );
    static void ( * return_portc_irq ) ( void );
    static void ( * return_portd_irq ) ( void );
    static void ( * return_porte_irq ) ( void );
#elif defined(KINETISL)
    static void ( * return_porta_irq ) ( void );
    static void ( * return_portcd_irq )( void );
#endif

/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       digital_init
     *
     *******************************************************************************/
    static inline
    void digital_init( void )
    __attribute__((always_inline, unused));
    
    static inline
    void digital_init( void ) {
        __disable_irq( );
#if defined(KINETISK)
        return_porta_irq = _VectorsRam[IRQ_PORTA+16];
        return_portb_irq = _VectorsRam[IRQ_PORTB+16];
        return_portc_irq = _VectorsRam[IRQ_PORTC+16];
        return_portd_irq = _VectorsRam[IRQ_PORTD+16];
        return_porte_irq = _VectorsRam[IRQ_PORTE+16];
#elif defined(KINETISL)
        return_porta_irq  = _VectorsRam[IRQ_PORTA+16];
        return_portcd_irq = _VectorsRam[IRQ_PORTCD+16];
#endif
        __enable_irq( );
    }
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
        if ( pin >= CORE_NUM_DIGITAL ) return;
#if defined(KINETISK)
        mask->pin = mask->pin | ( ( uint64_t )0x01 << pin );
        mask->mode_irqType[pin] = type;
        mask->mode_irqType[pin] |= mode << 4;
        mask->state = true;
#elif defined(KINETISL)
        mask->pin = mask->pin | ( ( uint32_t )0x01 << pin );
        mask->mode_irqType[pin] = type;
        mask->mode_irqType[pin] |= mode << 4;
        mask->state = true;
#endif
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
#if defined(KINETISK)
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
#if defined(KINETISK)
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
#if defined(KINETISK)
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
        
#if defined(KINETISK)
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
            __disable_irq( );
#if defined(KINETISK)
            attachInterruptVector( IRQ_PORTA, digitalISR );
            attachInterruptVector( IRQ_PORTB, digitalISR );
            attachInterruptVector( IRQ_PORTC, digitalISR );
            attachInterruptVector( IRQ_PORTD, digitalISR );
            attachInterruptVector( IRQ_PORTE, digitalISR );
#elif defined(KINETISL)
            attachInterruptVector( IRQ_PORTA, digitalISR );
            attachInterruptVector( IRQ_PORTCD, digitalISR );
#endif
            __enable_irq( );
        }
        
#if defined(KINETISK)
        uint64_t _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            uint32_t pin  = 63 - __builtin_clzll( _pin );
            uint32_t mode = mask->mode_irqType[pin] >> 4;
            uint32_t type = mask->mode_irqType[pin] & 0x0F;
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
            
            if ( mode == INPUT || mode == INPUT_PULLUP ) {
                *portModeRegister( pin ) = 0;
                if ( mode == INPUT ) *config = PORT_PCR_MUX( 1 );
                else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
                if ( enable_periph_irq ) attachDigitalInterrupt( pin, type );
            } else {
                pinMode( pin, mode );
                digitalWriteFast( pin, type );
            }
            _pin &= ~( ( uint64_t )1<<pin );
        }
#elif defined(KINETISL)
        uint32_t _pin = mask->pin;
        while ( __builtin_popcount( _pin ) ) {
            uint32_t pin  = 31 - __builtin_clz( _pin );
            uint32_t mode = mask->mode_irqType[pin] >> 4;
            uint32_t type = mask->mode_irqType[pin] & 0x0F;
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
            
            if ( mode == INPUT || mode == INPUT_PULLUP ) {
                *portModeRegister( pin ) &= ~digitalPinToBitMask( pin ); // TODO: atomic
                if ( mode == INPUT ) *config = PORT_PCR_MUX( 1 );
                else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
                if ( enable_periph_irq ) attachDigitalInterrupt( pin, type );
            } else {
                pinMode( pin, mode );
                digitalWriteFast( pin, type );
            }
            _pin &= ~( ( uint32_t )1<<pin );
        }
#endif
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
            __disable_irq( );
#if defined(KINETISK)
            attachInterruptVector( IRQ_PORTA, return_porta_irq );
            attachInterruptVector( IRQ_PORTB, return_portb_irq );
            attachInterruptVector( IRQ_PORTC, return_portc_irq );
            attachInterruptVector( IRQ_PORTD, return_portd_irq );
            attachInterruptVector( IRQ_PORTE, return_porte_irq );
#elif defined(KINETISL)
            attachInterruptVector( IRQ_PORTA, return_porta_irq );
            attachInterruptVector( IRQ_PORTCD, return_portcd_irq );
#endif
            __enable_irq( );
            
#if defined(KINETISK)
            uint64_t _pin = mask->pin;
            while ( __builtin_popcountll( _pin ) ) {
                uint32_t pin = 63 - __builtin_clzll( _pin );
                detachDigitalInterrupt( pin );
                _pin &= ~( ( uint64_t )1<<pin );
            }
#elif defined(KINETISL)
            uint32_t _pin = mask->pin;
            while ( __builtin_popcount( _pin ) ) {
                uint32_t pin = 31 - __builtin_clz( _pin );
                detachDigitalInterrupt( pin );
                _pin &= ~( ( uint32_t )1<<pin );
            }
#endif
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __DIGITAL_H__ */