/***********************************************************************************
 *  SnoozeDigital.h
 *  Teensy 3.2
 *
 * Purpose: Digital Pin Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#include "SnoozeDigital.h"

#define DR    0
#define GDIR  1
#define PSR   2
#define ICR1  3
#define ICR2  4
#define IMR   5
#define ISR   6
#define EDGE  7

#ifdef __cplusplus
extern "C" {
#endif
    extern void wakeup_isr( void );
    extern int nvic_execution_priority( void );
#ifdef __cplusplus
}
#endif

uint64_t SnoozeDigital::isr_pin = 0;
volatile uint8_t SnoozeDigital::sleep_type = 0;
/*******************************************************************************
 *  Same as Arduino pinMode + isr trigger type
 *
 *  @param _pin Teensy Pin
 *  @param mode INPUT, INPUT_PULLUP
 *  @param type CHANGE, FALLING or LOW, RISING or HIGH
 *
 *  @return Teensy Pin
 *******************************************************************************/
int SnoozeDigital::pinMode( int _pin, int mode, int type ) {
    if ( _pin >= 33 ) return -1;
    isUsed = true;
    pin            = pin | ( ( uint64_t )0x01 << _pin );// save pin
    irqType[_pin]  = type;// save type
    irqType[_pin] |= mode << 4;// save mode
    return _pin;
}

/*******************************************************************************
 *  Enable digital interrupt and configure the pin
 *******************************************************************************/
void SnoozeDigital::enableDriver( uint8_t mode ) {
    sleep_type = mode;
    if ( mode == 0 ) return;
    uint64_t _pin = pin;
    isr_pin = pin;
    
    int priority = nvic_execution_priority( );// get current priority
    // if running from interrupt set priority higher than current interrupt
    priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    
    _pin = pin;
    const struct digital_pin_bitband_and_config_table_struct *p;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber  = 63 - __builtin_clzll( _pin );// get pin
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from list
        p = digital_pin_to_info_PGM + pinNumber;
        if ( pinNumber > 33 ) continue;
        
        uint32_t pin_mode = irqType[pinNumber] >> 4;// get mode
        uint32_t pin_type = irqType[pinNumber] & 0x0F;// get type
        
        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        return_core_pin_config[pinNumber] = *config;// save pin config
        
        // setup pin mode/type/interrupt
        if ( pin_mode == INPUT || pin_mode == INPUT_PULLUP  || pin_mode == INPUT_PULLDOWN ) {
            *(p->reg + 1) &= ~(p->mask); // TODO: atomic
            if ( pin_mode == INPUT ) {
                *( p->pad ) = IOMUXC_PAD_DSE( 7 );
            } else if ( pin_mode == INPUT_PULLUP ) {
                *(p->pad) = IOMUXC_PAD_DSE( 7 ) | IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS( 3 ) | IOMUXC_PAD_HYS;
            } else if ( pin_mode == INPUT_PULLDOWN ) {
                *(p->pad) = IOMUXC_PAD_DSE( 7 ) | IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS( 0 ) | IOMUXC_PAD_HYS;
            } else { // INPUT_DISABLE
                *( p->pad ) = IOMUXC_PAD_DSE( 7 ) | IOMUXC_PAD_HYS;
            }
            attachDigitalInterrupt( pinNumber, pin_type );// set pin interrupt
        } else {
            //pinMode( pinNumber, pin_mode );
            //digitalWriteFast( pinNumber, pin_type );
        }
        
        if ( mode == 3 ) {
            volatile uint32_t *gpio = portOutputRegister( pinNumber );
            switch( ( uint32_t )gpio ) {
                case ( uint32_t )&GPIO6_DR: {
                    if ( ( GPIO1_IMR & 0xFFFF0000 ) && return_gpio1_16_31_irq == 0 ) {
                        return_isr_gpio1_16_31_enabled = NVIC_IS_ENABLED( IRQ_GPIO1_16_31 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO1_16_31 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO1_16_31 );
                        return_priority_gpio1_16_31 = NVIC_GET_PRIORITY( IRQ_GPIO1_16_31 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO1_16_31, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio1_16_31_irq = _VectorsRam[IRQ_GPIO1_16_31+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO1_16_31, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO1_16_31 );
                        //IOMUXC_GPR_GPR26 &= ~( GPIO1_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO1_IMR & 0xFFFF ) && return_gpio1_0_15_irq == 0 ) {
                        return_isr_gpio1_0_15_enabled = NVIC_IS_ENABLED( IRQ_GPIO1_0_15 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO1_0_15 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO1_0_15 );
                        return_priority_gpio1_0_15 = NVIC_GET_PRIORITY( IRQ_GPIO1_0_15 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO1_0_15, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio1_0_15_irq = _VectorsRam[IRQ_GPIO1_0_15+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO1_0_15, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO1_0_15 );
                        //IOMUXC_GPR_GPR26 &= ~( GPIO1_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR26 &= ~( GPIO1_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR26 &= ~( GPIO1_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO7_DR: {
                    if ( ( GPIO2_IMR & 0xFFFF0000 ) && return_gpio2_16_31_irq == 0 ) {
                        return_isr_gpio2_16_31_enabled = NVIC_IS_ENABLED( IRQ_GPIO2_16_31 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO2_16_31 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO2_16_31 );
                        return_priority_gpio2_16_31 = NVIC_GET_PRIORITY( IRQ_GPIO2_16_31 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO2_16_31, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio2_16_31_irq = _VectorsRam[IRQ_GPIO2_16_31+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO2_16_31, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO2_16_31 );
                        //IOMUXC_GPR_GPR27 &= ~( GPIO2_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO2_IMR & 0xFFFF ) && return_gpio2_0_15_irq == 0 ) {
                        return_isr_gpio2_0_15_enabled = NVIC_IS_ENABLED( IRQ_GPIO2_0_15 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO2_0_15 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO2_0_15 );
                        return_priority_gpio2_0_15 = NVIC_GET_PRIORITY( IRQ_GPIO2_0_15 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO2_0_15, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio2_0_15_irq = _VectorsRam[IRQ_GPIO2_0_15+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO2_0_15, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO2_0_15 );
                        //IOMUXC_GPR_GPR27 &= ~( GPIO2_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR27 &= ~( GPIO2_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR27 &= ~( GPIO2_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO8_DR: {
                    if ( ( GPIO3_IMR & 0xFFFF0000 ) && return_gpio3_16_31_irq == 0 ) {
                        return_isr_gpio3_16_31_enabled = NVIC_IS_ENABLED( IRQ_GPIO3_16_31 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO3_16_31 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO3_16_31 );
                        return_priority_gpio3_16_31 = NVIC_GET_PRIORITY( IRQ_GPIO3_16_31 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO3_16_31, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio3_16_31_irq = _VectorsRam[IRQ_GPIO3_16_31+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO3_16_31, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO3_16_31 );
                        //IOMUXC_GPR_GPR28 &= ~( GPIO3_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO3_IMR & 0xFFFF ) && return_gpio3_0_15_irq == 0 ) {
                        return_isr_gpio3_0_15_enabled = NVIC_IS_ENABLED( IRQ_GPIO3_0_15 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO3_0_15 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO3_0_15 );
                        return_priority_gpio3_0_15 = NVIC_GET_PRIORITY( IRQ_GPIO3_0_15 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO3_0_15, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio3_0_15_irq = _VectorsRam[IRQ_GPIO3_0_15+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO3_0_15, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO3_0_15 );
                        //IOMUXC_GPR_GPR28 &= ~( GPIO3_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR28 &= ~( GPIO3_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR28 &= ~( GPIO3_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO9_DR: {
                    if ( ( GPIO4_IMR & 0xFFFF0000 ) && return_gpio4_16_31_irq == 0 ) {
                        return_isr_gpio4_16_31_enabled = NVIC_IS_ENABLED( IRQ_GPIO4_16_31 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO4_16_31 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO4_16_31 );
                        return_priority_gpio4_16_31 = NVIC_GET_PRIORITY( IRQ_GPIO4_16_31 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO4_16_31, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio4_16_31_irq = _VectorsRam[IRQ_GPIO4_16_31+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO4_16_31, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO4_16_31 );
                        //IOMUXC_GPR_GPR29 &= ~( GPIO4_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO4_IMR & 0xFFFF ) && return_gpio4_0_15_irq == 0 ) {
                        return_isr_gpio4_0_15_enabled = NVIC_IS_ENABLED( IRQ_GPIO4_0_15 );
                        NVIC_DISABLE_IRQ( IRQ_GPIO4_0_15 );
                        NVIC_CLEAR_PENDING( IRQ_GPIO4_0_15 );
                        return_priority_gpio4_0_15 = NVIC_GET_PRIORITY( IRQ_GPIO4_0_15 );//get current priority
                        NVIC_SET_PRIORITY( IRQ_GPIO4_0_15, priority );//set priority to new level
                        __disable_irq( );
                        return_gpio4_0_15_irq = _VectorsRam[IRQ_GPIO4_0_15+16];// save prev isr handler
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO4_0_15, &wakeup_isr );
                        __enable_irq( );
                        NVIC_ENABLE_IRQ( IRQ_GPIO4_0_15 );
                        //IOMUXC_GPR_GPR29 &= ~( GPIO4_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR29 &= ~( GPIO4_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR29 &= ~( GPIO4_IMR & 0xFFFF );
                    break;
                }
                default:
                    break;
            }
        }
        else if ( mode <= 2 ) {
            return_isr_gpio6789_enabled = NVIC_IS_ENABLED( IRQ_GPIO6789 );
            NVIC_DISABLE_IRQ( IRQ_GPIO6789 );
            NVIC_CLEAR_PENDING( IRQ_GPIO6789 );
            return_priority_gpio6789 = NVIC_GET_PRIORITY( IRQ_GPIO6789 );//get current priority
            NVIC_SET_PRIORITY( IRQ_GPIO6789, priority );//set priority to new level
            __disable_irq( );
            return_gpio6789_irq = _VectorsRam[IRQ_GPIO6789+16];// save prev isr handler
            __disable_irq( );
            attachInterruptVector( IRQ_GPIO6789, &wakeup_isr );
            __enable_irq( );
            NVIC_ENABLE_IRQ( IRQ_GPIO6789 );
        }
    }
}

/*******************************************************************************
 *  Disable interrupt and configure pin to orignal state.
 *******************************************************************************/
void SnoozeDigital::disableDriver( uint8_t mode ) {
    if (mode == 0) return;
    uint64_t _pin = pin;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber = 63 - __builtin_clzll( _pin );
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from list
        if ( pinNumber > 33 ) continue;

        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        *config = return_core_pin_config[pinNumber];
        
        if ( mode == 3 ) {
            volatile uint32_t *gpio = portOutputRegister( pinNumber );
            switch( ( uint32_t )gpio ) {
                case ( uint32_t )&GPIO6_DR: {
                    if ( ( GPIO1_IMR & 0xFFFF0000 ) && return_gpio1_16_31_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO1_16_31, return_priority_gpio1_16_31 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO1_16_31, return_gpio1_16_31_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio1_16_31_irq = 0;
                        if ( return_isr_gpio1_16_31_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO1_16_31 );
                        //IOMUXC_GPR_GPR26 |= ( GPIO1_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO1_IMR & 0xFFFF ) && return_gpio1_0_15_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO1_0_15, return_priority_gpio1_0_15 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO1_0_15, return_gpio1_0_15_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio1_0_15_irq = 0;
                        if ( return_isr_gpio1_0_15_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO1_0_15 );
                        //IOMUXC_GPR_GPR26 |= ( GPIO1_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR26 |= ( GPIO1_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR26 |= ( GPIO1_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO7_DR: {
                    if ( ( GPIO2_IMR & 0xFFFF0000 ) ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO2_16_31, return_priority_gpio2_16_31 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO2_16_31, return_gpio2_16_31_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio2_16_31_irq = 0;
                        if ( return_isr_gpio2_16_31_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO2_16_31 );
                        //IOMUXC_GPR_GPR27 |= ( GPIO2_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO2_IMR & 0xFFFF ) && return_gpio2_0_15_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO2_0_15, return_priority_gpio2_0_15 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO2_0_15, return_gpio2_0_15_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio2_0_15_irq = 0;
                        if ( return_isr_gpio2_0_15_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO2_0_15 );
                        //IOMUXC_GPR_GPR27 |= ( GPIO2_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR27 |= ( GPIO2_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR27 |= ( GPIO2_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO8_DR: {
                    if ( ( GPIO3_IMR & 0xFFFF0000 ) && return_gpio2_16_31_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO3_16_31, return_priority_gpio3_16_31 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO3_16_31, return_gpio3_16_31_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio3_16_31_irq = 0;
                        if ( return_isr_gpio3_16_31_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO3_16_31 );
                        //IOMUXC_GPR_GPR28 |= ( GPIO3_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO3_IMR & 0xFFFF ) && return_gpio3_0_15_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO3_0_15, return_priority_gpio3_0_15 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO3_0_15, return_gpio3_0_15_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio3_0_15_irq = 0;
                        if ( return_isr_gpio3_0_15_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO3_0_15 );
                        //IOMUXC_GPR_GPR28 |= ( GPIO3_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR28 |= ( GPIO3_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR28 |= ( GPIO3_IMR & 0xFFFF );
                    break;
                }
                case ( uint32_t )&GPIO9_DR: {
                    if ( ( GPIO4_IMR & 0xFFFF0000 ) && return_gpio4_16_31_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO4_16_31, return_priority_gpio4_16_31 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO4_16_31, return_gpio4_16_31_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio4_16_31_irq = 0;
                        if ( return_isr_gpio4_16_31_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO4_16_31 );
                        //IOMUXC_GPR_GPR29 |= ( GPIO4_IMR & 0xFFFF0000 );
                    }
                    if ( ( GPIO4_IMR & 0xFFFF ) && return_gpio4_0_15_irq != 0 ) {
                        NVIC_SET_PRIORITY( IRQ_GPIO4_0_15, return_priority_gpio4_0_15 );
                        __disable_irq( );
                        attachInterruptVector( IRQ_GPIO4_0_15, return_gpio4_0_15_irq );// set previous isr func
                        __enable_irq( );
                        return_gpio4_0_15_irq = 0;
                        if ( return_isr_gpio4_0_15_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO4_0_15 );
                        //IOMUXC_GPR_GPR29 |= ( GPIO4_IMR & 0xFFFF );
                    }
                    IOMUXC_GPR_GPR29 |= ( GPIO4_IMR & 0xFFFF0000 );
                    IOMUXC_GPR_GPR29 |= ( GPIO4_IMR & 0xFFFF );
                    break;
                }
                default:
                    break;
            }
        }
        else if ( mode <= 2 ) {
            NVIC_SET_PRIORITY( IRQ_GPIO6789, return_priority_gpio6789 );
            __disable_irq( );
            attachInterruptVector( IRQ_GPIO6789, return_gpio6789_irq );// set previous isr func
            __enable_irq( );
            return_gpio6789_irq = 0;
            if ( return_isr_gpio6789_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_GPIO6789 );
        }
    }
}

/*******************************************************************************
 *  for LLWU wakeup ISR to call actual digital ISR code
 *******************************************************************************/
void SnoozeDigital::clearIsrFlags( uint32_t ipsr ) {
    if ( sleep_type == 0 ) return;
    uint64_t _pin = isr_pin;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber = 63 - __builtin_clzll( _pin );
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from local list
        if ( pinNumber > 33 ) continue;
        volatile uint32_t *gpio = portOutputRegister( pinNumber );
        if ( sleep_type == 3 ) {
            switch( ( uint32_t )gpio ) {
                case ( uint32_t )&GPIO6_DR:
                    gpio = &GPIO1_DR;
                    break;
                case ( uint32_t )&GPIO7_DR:
                    gpio = &GPIO2_DR;
                    break;
                case ( uint32_t )&GPIO8_DR:
                    gpio = &GPIO3_DR;
                    break;
                case ( uint32_t )&GPIO9_DR:
                    gpio = &GPIO4_DR;
                    break;
                default:
                    break;
            }
        }
        uint32_t status = gpio[ISR] & gpio[IMR];
        if ( status ) {
            gpio[ISR] = status;
        }
        detachDigitalInterrupt( pinNumber );// remove pin interrupt
    }
}

/*******************************************************************************
 *  sleep pin wakeups go through NVIC
 *
 *  @param pin  Teensy Pin
 *  @param type CHANGE, FALLING or LOW, RISING or HIGH
 *******************************************************************************/
void SnoozeDigital::attachDigitalInterrupt( uint8_t pin, int type ) {
    if ( pin >= 33 ) return;
    volatile uint32_t *gpio = portOutputRegister( pin );
    if ( sleep_type == 3 ) {
        switch( ( uint32_t )gpio ) {
            case ( uint32_t )&GPIO6_DR:
                gpio = &GPIO1_DR;
                break;
            case ( uint32_t )&GPIO7_DR:
                gpio = &GPIO2_DR;
                break;
            case ( uint32_t )&GPIO8_DR:
                gpio = &GPIO3_DR;
                break;
            case ( uint32_t )&GPIO9_DR:
                gpio = &GPIO4_DR;
                break;
            default:
                break;
        }
    }
    volatile uint32_t *mux = portConfigRegister( pin );
    uint32_t mask = digitalPinToBitMask( pin );
    
    uint32_t icr;
    switch ( type ) {
        case CHANGE:  icr = 0; break;
        case RISING:  icr = 2; break;
        case FALLING: icr = 3; break;
        case LOW:     icr = 0; break;
        case HIGH:    icr = 1; break;
        default: return;
    }
    // TODO: global interrupt disable to protect these read-modify-write accesses?
    gpio[IMR] &= ~mask;    // disable interrupt
    *mux = 5;               // pin is GPIO
    gpio[GDIR] &= ~mask;    // pin to input mode
    uint32_t index = __builtin_ctz( mask );
    if ( type == CHANGE ) {
        gpio[EDGE] |= mask;
    } else {
        gpio[EDGE] &= ~mask;
        if ( index < 15 ) {
            uint32_t shift = index * 2;
            gpio[ICR1] = (gpio[ICR1] & ~( 3 << shift ) ) | ( icr << shift );
        } else {
            uint32_t shift = ( index - 16 ) * 2;
            gpio[ICR2] = ( gpio[ICR2] & ~( 3 << shift ) ) | ( icr << shift );
        }
    }
    gpio[ISR] = mask;  // clear any prior pending interrupt
    gpio[IMR] |= mask; // enable interrupt

}

/*******************************************************************************
 *  remove interrupt from pin
 *
 *  @param pin Teensy Pin
 *******************************************************************************/
void SnoozeDigital::detachDigitalInterrupt( uint8_t pin ) {
    if ( pin >= 33 ) return;
    volatile uint32_t *gpio = portOutputRegister( pin );
    if ( sleep_type == 2 ) {
        switch( ( uint32_t )gpio ) {
            case ( uint32_t )&GPIO6_DR:
                gpio = &GPIO1_DR;
                break;
            case ( uint32_t )&GPIO7_DR:
                gpio = &GPIO2_DR;
                break;
            case ( uint32_t )&GPIO8_DR:
                gpio = &GPIO3_DR;
                break;
            case ( uint32_t )&GPIO9_DR:
                gpio = &GPIO4_DR;
                break;
            default:
                break;
        }
    }
    uint32_t mask = digitalPinToBitMask( pin );
    gpio[IMR] &= ~mask;
}
#endif /* __IMXRT1062__ */
