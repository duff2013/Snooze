/***********************************************************************************
 *  digital.h
 *  Teensy 3.x/LC
 *
 * Purpose: Digital Pins
 *
 ***********************************************************************************/
#ifndef __DIGITAL_H__
#define __DIGITAL_H__
#include <utility/util.h>
/***********************************************************************************/
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
/***********************************************************************************/
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
/***********************************************************************************/
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
        if ( pin >= CORE_NUM_DIGITAL ) return;
#if defined(KINETISK)
        mask->pin = mask->pin | ( ( uint64_t )0x01 << pin );// save pin
        mask->mode_irqType[pin] = type;// save type
        mask->mode_irqType[pin] |= mode << 4;// save mode
        mask->state = true;
#elif defined(KINETISL)
        mask->pin = mask->pin | ( ( uint32_t )0x01 << pin );// save pin
        mask->mode_irqType[pin] = type;// save type
        mask->mode_irqType[pin] |= mode << 4;// save mode
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
#if defined(KINETISK)
        uint64_t _pin = mask->pin;
        if ( enable_periph_irq ) {// if using sleep must setup pin interrupt to wake
            while ( __builtin_popcountll( _pin ) ) {
                uint32_t pin  = 63 - __builtin_clzll( _pin );// get pin
                if ( pin > 33 ) return;
                int priority = nvic_execution_priority( );// get current priority
                // if running from interrupt set priority higher than current interrupt
                priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
                if ( pin == 3 || pin == 4 || pin == 24 || pin == 33 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTA, priority );//set priority to new level
                    __disable_irq( );
                    return_porta_irq = _VectorsRam[IRQ_PORTA+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTA, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                else if ( pin == 0 || pin == 1 || pin == 16 || pin == 17 || pin == 18 || pin == 19 || pin == 25 || pin == 32 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTB, priority );//set priority to new level
                    __disable_irq( );
                    return_portb_irq = _VectorsRam[IRQ_PORTB+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTB, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                else if ( pin == 9 || pin == 10 || pin == 11 || pin == 12 || pin == 13 || pin == 15 || pin == 22 || pin == 23 || pin == 27 || pin == 28 || pin == 29 || pin == 30 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTC, priority );//set priority to new level
                    __disable_irq( );
                    return_portc_irq = _VectorsRam[IRQ_PORTC+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTC, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                else if ( pin == 2 || pin == 5 || pin == 6 || pin == 7 || pin == 8 || pin == 14 || pin == 20 || pin == 21 )  {
                    NVIC_SET_PRIORITY( IRQ_PORTD, priority );//set priority to new level
                    __disable_irq( );
                    return_portd_irq = _VectorsRam[IRQ_PORTD+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTD, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                else if ( pin == 26 || pin == 31 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTE, priority );//set priority to new level
                    __disable_irq( );
                    return_porte_irq = _VectorsRam[IRQ_PORTE+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTE, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                
                _pin &= ~( ( uint64_t )1<<pin );// remove pin from list
            }
            __disable_irq( );// disable all interrupts for attaching an interrupt to a pin
        }
        _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            uint32_t pin  = 63 - __builtin_clzll( _pin );// get pin
            uint32_t mode = mask->mode_irqType[pin] >> 4;// get type
            uint32_t type = mask->mode_irqType[pin] & 0x0F;// get mode
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
            
            if ( mode == INPUT || mode == INPUT_PULLUP ) {// setup pin mode/type/interrupt
                *portModeRegister( pin ) = 0;
                if ( mode == INPUT ) *config = PORT_PCR_MUX( 1 );
                else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
                if ( enable_periph_irq ) attachDigitalInterrupt( pin, type );// set pin interrupt
            } else {
                pinMode( pin, mode );
                digitalWriteFast( pin, type );
            }
            _pin &= ~( ( uint64_t )1<<pin );// remove pin from list
        }
        
#elif defined(KINETISL)
        uint32_t _pin = mask->pin;
        if ( enable_periph_irq ) {// if using sleep must setup pin interrupt to wake
            while ( __builtin_popcount( _pin ) ) {
                uint32_t pin  = 31 - __builtin_clz( _pin );// get pin
                
                int priority = nvic_execution_priority( );// get current priority
                // if running from handler set priority higher than current handler
                priority = ( priority  < 256 ) && ( (priority - 16) > 0 ) ? priority - 16 : 128;
                if ( pin == 3 || pin == 4 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTA, priority );//set priority to new level
                    __disable_irq( );
                    return_porta_irq = _VectorsRam[IRQ_PORTA+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTA, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                else if ( pin == 2 || pin == 5 || pin == 6 || pin == 7 || pin == 8 || pin == 14 || pin == 20 || pin == 21 || pin == 9 || pin == 10 || pin == 11 || pin == 12 || pin == 13 || pin == 15 || pin == 22 || pin == 23 ) {
                    NVIC_SET_PRIORITY( IRQ_PORTCD, priority );//set priority to new level
                    __disable_irq( );
                    return_portcd_irq = _VectorsRam[IRQ_PORTCD+16];// save prev isr handler
                    attachInterruptVector( IRQ_PORTCD, digitalISR );// set snooze isr
                    __enable_irq( );
                }
                _pin &= ~( ( uint32_t )1<<pin );// remove pin from list
            }
            __disable_irq( );// disable all interrupts for attaching an interrupt to a pin
        }
        
        _pin = mask->pin;
        while ( __builtin_popcount( _pin ) ) {
            uint32_t pin  = 31 - __builtin_clz( _pin );// get pin
            uint32_t mode = mask->mode_irqType[pin] >> 4;// get type
            uint32_t type = mask->mode_irqType[pin] & 0x0F;// get mode
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
            
            if ( mode == INPUT || mode == INPUT_PULLUP ) {// setup pin mode/type/interrupt
                *portModeRegister( pin ) &= ~digitalPinToBitMask( pin ); // TODO: atomic
                if ( mode == INPUT ) *config = PORT_PCR_MUX( 1 );
                else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
                if ( enable_periph_irq ) attachDigitalInterrupt( pin, type );// set pin interrupt
            } else {
                pinMode( pin, mode );
                digitalWriteFast( pin, type );
            }
            _pin &= ~( ( uint32_t )1<<pin );// remove pin from list
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
#if defined(KINETISK)
            uint64_t _pin = mask->pin;
            while ( __builtin_popcountll( _pin ) ) {
                uint32_t pin = 63 - __builtin_clzll( _pin );
                if ( pin > 33 ) {
                    __enable_irq( );// enable interrupts from digital_set function
                    return;
                }
                detachDigitalInterrupt( pin );// remove pin interrupt
                _pin &= ~( ( uint64_t )1<<pin );// remove pin from list
            }
            __enable_irq( );// enable interrupts from digital_set function
            
            _pin = mask->pin;
            while ( __builtin_popcountll( _pin ) ) {
                
                uint32_t pin = 63 - __builtin_clzll( _pin );
                if ( enable_periph_irq ) {// if using sleep, give back previous isr handler
                    if ( pin == 3 || pin == 4 || pin == 24 || pin == 33 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTA, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTA, return_porta_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                    else if ( pin == 0 || pin == 1 || pin == 16 || pin == 17 || pin == 18 || pin == 19 || pin == 25 || pin == 32 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTB, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTB, return_portb_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                    else if ( pin == 9 || pin == 10 || pin == 11 || pin == 12 || pin == 13 || pin == 15 || pin == 22 || pin == 23 || pin == 27 || pin == 28 || pin == 29 || pin == 30 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTC, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTC, return_portc_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                    else if ( pin == 2 || pin == 5 || pin == 6 || pin == 7 || pin == 8 || pin == 14 || pin == 20 || pin == 21 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTD, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTD, return_portd_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                    else if ( pin == 26 || pin == 31 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTE, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTE, return_porte_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                }
                _pin &= ~( ( uint64_t )1<<pin );// remove pin from list
            }
#elif defined(KINETISL)
            uint32_t _pin = mask->pin;
            while ( __builtin_popcount( _pin ) ) {
                uint32_t pin = 31 - __builtin_clz( _pin );
                if ( pin > 33 ) {
                    __enable_irq( );// enable interrupts from digital_set function
                    return;
                }
                detachDigitalInterrupt( pin );// remove pin interrupt
                _pin &= ~( ( uint32_t )1<<pin );// remove pin from list
            }
            __enable_irq( );// enable interrupts from digital_set function
            
            _pin = mask->pin;
            while ( __builtin_popcount( _pin ) ) {
                uint32_t pin = 31 - __builtin_clz( _pin );
                if ( enable_periph_irq ) {// if using sleep give back previous isr handler
                    if ( pin == 3 || pin == 4 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTA, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTA, return_porta_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                    else if ( pin == 2 || pin == 5 || pin == 6 || pin == 7 || pin == 8 || pin == 14 || pin == 20 || pin == 21 || pin == 9 || pin == 10 || pin == 11 || pin == 12 || pin == 13 || pin == 15 || pin == 22 || pin == 23 ) {
                        NVIC_SET_PRIORITY( IRQ_PORTCD, 128 );//return priority to core level
                        __disable_irq( );
                        attachInterruptVector( IRQ_PORTCD, return_portcd_irq );// return prev interrupt handler
                        __enable_irq( );
                    }
                }
                _pin &= ~( ( uint32_t )1<<pin );// remove pin from list
            }
#endif
        }
    }
#ifdef __cplusplus
}
#endif
/***********************************************************************************/
#endif /* __DIGITAL_H__ */