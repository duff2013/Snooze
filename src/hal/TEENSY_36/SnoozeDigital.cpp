/***********************************************************************************
 *  SnoozeDigital.h
 *  Teensy 3.6
 *
 * Purpose: Digital Pin Driver
 *
 **********************************************************************************/
#if defined(__MK66FX1M0__)

#include "SnoozeDigital.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

uint64_t SnoozeDigital::isr_pin = 0;

volatile uint8_t SnoozeDigital::sleep_type = 0;

#if defined(__MK20DX128__) || defined(__MK20DX256__)
#define CORE_NUM_SLEEP_DEEP     34
#elif defined(__MKL26Z64__)
#define CORE_NUM_SLEEP_DEEP     27
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define CORE_NUM_SLEEP_DEEP     64
#endif
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
    if ( _pin >= CORE_NUM_INTERRUPT ) return -1;
    isUsed = true;
    pin            = pin | ( ( uint64_t )0x01 << _pin );// save pin
    irqType[_pin]  = type;// save type
    irqType[_pin] |= mode << 4;// save mode
    return _pin;
}

/*******************************************************************************
 *  Enable digital interrupt and configure the pin
 *******************************************************************************/
void SnoozeDigital::enableDriver( uint8_t type ) {
    sleep_type = type;
    if (type == 0) return;
    uint64_t _pin = pin;
    isr_pin = pin;
    if (type == 1) {
        return_isr_a_enabled = NVIC_IS_ENABLED( IRQ_PORTA );
        return_isr_b_enabled = NVIC_IS_ENABLED( IRQ_PORTB );
        return_isr_c_enabled = NVIC_IS_ENABLED( IRQ_PORTC );
        return_isr_d_enabled = NVIC_IS_ENABLED( IRQ_PORTD );
        return_isr_e_enabled = NVIC_IS_ENABLED( IRQ_PORTE );
        NVIC_DISABLE_IRQ(IRQ_PORTA);
        NVIC_DISABLE_IRQ(IRQ_PORTB);
        NVIC_DISABLE_IRQ(IRQ_PORTC);
        NVIC_DISABLE_IRQ(IRQ_PORTD);
        NVIC_DISABLE_IRQ(IRQ_PORTE);
        NVIC_CLEAR_PENDING(IRQ_PORTA);
        NVIC_CLEAR_PENDING(IRQ_PORTB);
        NVIC_CLEAR_PENDING(IRQ_PORTC);
        NVIC_CLEAR_PENDING(IRQ_PORTD);
        NVIC_CLEAR_PENDING(IRQ_PORTE);
        
        int priority = nvic_execution_priority( );// get current priority
        // if running from interrupt set priority higher than current interrupt
        priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        return_priority_a = NVIC_GET_PRIORITY( IRQ_PORTA );//get current priority
        return_priority_b = NVIC_GET_PRIORITY( IRQ_PORTB );//get current priority
        return_priority_c = NVIC_GET_PRIORITY( IRQ_PORTC );//get current priority
        return_priority_d = NVIC_GET_PRIORITY( IRQ_PORTD );//get current priority
        return_priority_e = NVIC_GET_PRIORITY( IRQ_PORTE );//get current priority
        NVIC_SET_PRIORITY( IRQ_PORTA, priority );//set priority to new level
        NVIC_SET_PRIORITY( IRQ_PORTB, priority );//set priority to new level
        NVIC_SET_PRIORITY( IRQ_PORTC, priority );//set priority to new level
        NVIC_SET_PRIORITY( IRQ_PORTD, priority );//set priority to new level
        NVIC_SET_PRIORITY( IRQ_PORTE, priority );//set priority to new level
        
        __disable_irq( );
        return_porta_irq = _VectorsRam[IRQ_PORTA+16];// save prev isr handler
        return_portb_irq = _VectorsRam[IRQ_PORTB+16];// save prev isr handler
        return_portc_irq = _VectorsRam[IRQ_PORTC+16];// save prev isr handler
        return_portd_irq = _VectorsRam[IRQ_PORTD+16];// save prev isr handler
        return_porte_irq = _VectorsRam[IRQ_PORTE+16];// save prev isr handler
        attachInterruptVector( IRQ_PORTA, wakeup_isr );// set snooze digA isr
        attachInterruptVector( IRQ_PORTB, wakeup_isr );// set snooze digB isr
        attachInterruptVector( IRQ_PORTC, wakeup_isr );// set snooze digC isr
        attachInterruptVector( IRQ_PORTD, wakeup_isr );// set snooze digD isr
        attachInterruptVector( IRQ_PORTE, wakeup_isr );// set snooze digE isr
        __enable_irq( );
        
        NVIC_ENABLE_IRQ( IRQ_PORTA );
        NVIC_ENABLE_IRQ( IRQ_PORTB );
        NVIC_ENABLE_IRQ( IRQ_PORTC );
        NVIC_ENABLE_IRQ( IRQ_PORTD );
        NVIC_ENABLE_IRQ( IRQ_PORTE );
    }
    _pin = pin;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber  = 63 - __builtin_clzll( _pin );// get pin
        
        if ( pinNumber > CORE_NUM_INTERRUPT ) break;
        
        uint32_t pin_mode = irqType[pinNumber] >> 4;// get mode
        uint32_t pin_type = irqType[pinNumber] & 0x0F;// get type
        
        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        return_core_pin_config[pinNumber] = *config;// save pin config
        // setup pin mode/type/interrupt
        if ( pin_mode == INPUT || pin_mode == INPUT_PULLUP  || pin_mode == INPUT_PULLDOWN ) {
            *portModeRegister( pinNumber ) = 0;
            *config = PORT_PCR_MUX( 1 );
            if ( pin_mode == INPUT_PULLUP ) *config |= PORT_PCR_PE | PORT_PCR_PS;// pullup
            else if ( pin_mode == INPUT_PULLDOWN ) {
                *config |= ( PORT_PCR_PE ); // pulldown
                *config &= ~( PORT_PCR_PS );
            }
            if (type == 1) {
                attachDigitalInterrupt( pinNumber, pin_type );// set pin interrupt
            }
            else {
                llwu_configure_pin_mask( pinNumber, pin_type );
            }
        } else {
            //pinMode( pinNumber, pin_mode );
            //digitalWriteFast( pinNumber, pin_type );
        }
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from list
    }
}

/*******************************************************************************
 *  Disable interrupt and configure pin to orignal state.
 *******************************************************************************/
void SnoozeDigital::disableDriver( uint8_t type ) {
    if (type == 0) return;
    uint64_t _pin = pin;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber = 63 - __builtin_clzll( _pin );
        
        if ( pinNumber > 33 ) break;
        
        *portModeRegister( pinNumber ) = 0;
        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        *config = return_core_pin_config[pinNumber];
        
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from list
    }
    if (type == 1) {
        NVIC_SET_PRIORITY( IRQ_PORTA, return_priority_a );//return priority
        NVIC_SET_PRIORITY( IRQ_PORTB, return_priority_b );//return priority
        NVIC_SET_PRIORITY( IRQ_PORTC, return_priority_c );//return priority
        NVIC_SET_PRIORITY( IRQ_PORTD, return_priority_d );//return priority
        NVIC_SET_PRIORITY( IRQ_PORTE, return_priority_e );//return priority
        __disable_irq( );
        attachInterruptVector( IRQ_PORTA, return_porta_irq );// set previous isr func
        attachInterruptVector( IRQ_PORTB, return_portb_irq );// set previous isr func
        attachInterruptVector( IRQ_PORTC, return_portc_irq );// set previous isr func
        attachInterruptVector( IRQ_PORTD, return_portd_irq );// set previous isr func
        attachInterruptVector( IRQ_PORTE, return_porte_irq );// set previous isr func
        __enable_irq( );
        if ( return_isr_a_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTA );
        if ( return_isr_b_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTB );
        if ( return_isr_c_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTC );
        if ( return_isr_d_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTD );
        if ( return_isr_e_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTE );
    }
}

/*******************************************************************************
 *  for LLWU wakeup ISR to call actual digital ISR code
 *******************************************************************************/
void SnoozeDigital::clearIsrFlags( uint32_t ipsr ) {
    /*int source = -1;
    
    uint32_t isfr_a = PORTA_ISFR;
    PORTA_ISFR = isfr_a;
    uint32_t isfr_b = PORTB_ISFR;
    uint32_t isfr_e = PORTE_ISFR;
    PORTB_ISFR = isfr_b;
    PORTE_ISFR = isfr_e;
    uint32_t isfr_c = PORTC_ISFR;
    uint32_t isfr_d = PORTD_ISFR;
    PORTC_ISFR = isfr_c;
    PORTD_ISFR = isfr_d;*/
    
    // return if using deepSleep or hibernate
    //if ( mode == LLS || mode == VLLS3 || mode == VLLS2 || mode == VLLS1 ) return;
    if ( sleep_type > 1 ) return;
    uint64_t _pin = isr_pin;
    while ( __builtin_popcountll( _pin ) ) {
        uint32_t pinNumber = 63 - __builtin_clzll( _pin );
        if ( pinNumber > 33 ) return;
        detachDigitalInterrupt( pinNumber );// remove pin interrupt
        _pin &= ~( ( uint64_t )1 << pinNumber );// remove pin from local list
    }
    /*
    // get source of interrupt
    if ( isfr_a & CORE_PIN3_BITMASK )       source = 3;
    else if ( isfr_a & CORE_PIN4_BITMASK  ) source = 4;
    else if ( isfr_a & CORE_PIN25_BITMASK ) source = 25;
    else if ( isfr_a & CORE_PIN26_BITMASK ) source = 26;
    else if ( isfr_a & CORE_PIN27_BITMASK ) source = 27;
    else if ( isfr_a & CORE_PIN28_BITMASK ) source = 28;
    else if ( isfr_a & CORE_PIN39_BITMASK ) source = 39;
    
    else if ( isfr_b & CORE_PIN0_BITMASK )  source = 0;
    else if ( isfr_b & CORE_PIN1_BITMASK )  source = 1;
    else if ( isfr_b & CORE_PIN16_BITMASK ) source = 16;
    else if ( isfr_b & CORE_PIN17_BITMASK ) source = 17;
    else if ( isfr_b & CORE_PIN18_BITMASK ) source = 18;
    else if ( isfr_b & CORE_PIN19_BITMASK ) source = 19;
    else if ( isfr_b & CORE_PIN29_BITMASK ) source = 29;
    else if ( isfr_b & CORE_PIN30_BITMASK ) source = 30;
    else if ( isfr_b & CORE_PIN31_BITMASK ) source = 31;
    else if ( isfr_b & CORE_PIN32_BITMASK ) source = 32;
    
    else if ( isfr_c & CORE_PIN9_BITMASK )  source = 9;
    else if ( isfr_c & CORE_PIN10_BITMASK ) source = 10;
    else if ( isfr_c & CORE_PIN11_BITMASK ) source = 11;
    else if ( isfr_c & CORE_PIN12_BITMASK ) source = 12;
    else if ( isfr_c & CORE_PIN13_BITMASK ) source = 13;
    else if ( isfr_c & CORE_PIN15_BITMASK ) source = 15;
    else if ( isfr_c & CORE_PIN22_BITMASK ) source = 22;
    else if ( isfr_c & CORE_PIN23_BITMASK ) source = 23;
    else if ( isfr_c & CORE_PIN35_BITMASK ) source = 35;
    else if ( isfr_c & CORE_PIN36_BITMASK ) source = 36;
    else if ( isfr_c & CORE_PIN37_BITMASK ) source = 37;
    else if ( isfr_c & CORE_PIN38_BITMASK ) source = 38;
    
    else if ( isfr_d & CORE_PIN2_BITMASK )  source = 2;
    else if ( isfr_d & CORE_PIN5_BITMASK )  source = 5;
    else if ( isfr_d & CORE_PIN6_BITMASK )  source = 6;
    else if ( isfr_d & CORE_PIN7_BITMASK )  source = 7;
    else if ( isfr_d & CORE_PIN8_BITMASK )  source = 8;
    else if ( isfr_d & CORE_PIN14_BITMASK ) source = 14;
    else if ( isfr_d & CORE_PIN20_BITMASK ) source = 20;
    else if ( isfr_d & CORE_PIN21_BITMASK ) source = 21;
    
    else if ( isfr_e & CORE_PIN24_BITMASK ) source = 24;
    else if ( isfr_e & CORE_PIN33_BITMASK ) source = 33;
    else if ( isfr_e & CORE_PIN34_BITMASK ) source = 34;
    wake_source = source;
     */
}

/*******************************************************************************
 *  sleep pin wakeups go through NVIC
 *
 *  @param pin  Teensy Pin
 *  @param type CHANGE, FALLING or LOW, RISING or HIGH
 *******************************************************************************/
void SnoozeDigital::attachDigitalInterrupt( uint8_t pin, int type ) {
    volatile uint32_t *config;
    uint32_t cfg, mask;
    
    if ( pin >= CORE_NUM_DIGITAL ) return;
    switch ( type ) {
        case CHANGE:	mask = 0x0B; break;
        case RISING:	mask = 0x09; break;
        case FALLING:	mask = 0x0A; break;
        case LOW:	mask = 0x08; break;
        case HIGH:	mask = 0x0C; break;
        default: return;
    }
    __disable_irq( );
    mask = ( mask << 16 ) | 0x01000000;
    config = portConfigRegister( pin );
    cfg = *config;
    cfg &= ~0x000F0000;		// disable any previous interrupt
    *config = cfg;
    cfg |= mask;
    *config = cfg;			// enable the new interrupt
    __enable_irq( );
}

/*******************************************************************************
 *  remove interrupt from pin
 *
 *  @param pin Teensy Pin
 *******************************************************************************/
void SnoozeDigital::detachDigitalInterrupt( uint8_t pin ) {
    volatile uint32_t *config;
    __disable_irq( );
    config = portConfigRegister( pin );
    *config = ( ( *config & ~0x000F0000 ) | 0x01000000 );
    __enable_irq( );
}
#endif /* __MK66FX1M0__ */
