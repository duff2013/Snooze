/***********************************************************************************
 *  SnoozeDigital.h
 *  Teensy LC
 *
 * Purpose: Digital Pin Driver
 *
 **********************************************************************************/
#if defined(__MKL26Z64__)

#include "SnoozeDigital.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

uint32_t SnoozeDigital::isr_pin = 0;
volatile uint8_t SnoozeDigital::sleep_type = 0;
#define CORE_NUM_SLEEP_DEEP     27
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
    pin            = pin | ( ( uint32_t )0x01 << _pin );// save pin
    irqType[_pin]  = type;// save type
    irqType[_pin] |= mode << 4;// save mode
    return _pin;
}

/*******************************************************************************
 *  Enable digital interrupt and configure the pin
 *******************************************************************************/
void SnoozeDigital::enableDriver( uint8_t mode ) {
    sleep_type = mode;
    if (mode == 0) return;

    uint32_t _pin = pin;
    isr_pin = pin;
    // if using sleep must setup pin interrupt to wake
    if (mode == 1) {
        return_isr_a_enabled  = NVIC_IS_ENABLED( IRQ_PORTA );
        return_isr_cd_enabled = NVIC_IS_ENABLED( IRQ_PORTCD );
        NVIC_DISABLE_IRQ(IRQ_PORTA);
        NVIC_DISABLE_IRQ(IRQ_PORTCD);
        NVIC_CLEAR_PENDING(IRQ_PORTA);
        NVIC_CLEAR_PENDING(IRQ_PORTCD);
        int priority = nvic_execution_priority( );// get current priority
        // if running from interrupt set priority higher than current interrupt
        priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        return_priority_a = NVIC_GET_PRIORITY( IRQ_PORTA );//get current priority
        return_priority_cd = NVIC_GET_PRIORITY( IRQ_PORTCD );//get current priority
        NVIC_SET_PRIORITY( IRQ_PORTA, priority );//set priority to new level
        NVIC_SET_PRIORITY( IRQ_PORTCD, priority );//set priority to new level
        __disable_irq( );
        return_porta_irq = _VectorsRam[IRQ_PORTA+16];// save prev isr handler
        return_portcd_irq = _VectorsRam[IRQ_PORTCD+16];// save prev isr handler
        attachInterruptVector( IRQ_PORTA, wakeup_isr );// set snooze isr
        attachInterruptVector( IRQ_PORTCD, wakeup_isr );// set snooze isr
        __enable_irq( );
        NVIC_ENABLE_IRQ( IRQ_PORTA );
        NVIC_ENABLE_IRQ( IRQ_PORTCD );
    }
    _pin = pin;
    while ( __builtin_popcount( _pin ) ) {
        uint32_t pinNumber  = 31 - __builtin_clz( _pin );// get pin
        
        if ( pinNumber > CORE_NUM_INTERRUPT ) break;
        
        uint32_t pin_mode = irqType[pinNumber] >> 4;// get mode
        uint32_t pin_type = irqType[pinNumber] & 0x0F;// get type
        
        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        return_core_pin_config[pinNumber] = *config;// save pin config
        
        if ( pin_mode == INPUT || pin_mode == INPUT_PULLUP ) {// setup pin mode/type/interrupt
            *portModeRegister( pinNumber ) &= ~digitalPinToBitMask( pinNumber );
            *config = PORT_PCR_MUX( 1 );
            if ( pin_mode == INPUT_PULLUP ) {
                *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;// pullup
            }
            else if ( pin_mode == INPUT_PULLDOWN ) {
                *config |= ( PORT_PCR_PE ); // pulldown
                *config &= ~( PORT_PCR_PS );
            }
        
            if (mode == 1) {
                attachDigitalInterrupt( pinNumber, pin_type );// set pin interrupt
            }
            else {
                llwu_configure_pin_mask( pinNumber, pin_type );
            }
        } else {
            //pinMode( pinNumber, pin_mode );
            //digitalWriteFast( pinNumber, pin_type );
        }
        _pin &= ~( ( uint32_t )1 << pinNumber );// remove pin from list
    }
}

/*******************************************************************************
 *  Disable interrupt and configure pin to orignal state.
 *******************************************************************************/
void SnoozeDigital::disableDriver( uint8_t mode ) {
    if (mode == 0) return;

    uint32_t _pin = pin;
    while ( __builtin_popcount( _pin ) ) {
        uint32_t pinNumber = 31 - __builtin_clz( _pin );
        
        if ( pinNumber > 33 ) break;
        
        *portModeRegister( pinNumber ) &= ~digitalPinToBitMask( pinNumber );
        volatile uint32_t *config;
        config = portConfigRegister( pinNumber );
        *config = return_core_pin_config[pinNumber];
        
        _pin &= ~( ( uint32_t )1 << pinNumber );// remove pin from list
    }
    if (mode == 1) {
        NVIC_SET_PRIORITY( IRQ_PORTA,  return_priority_a );//return priority
        NVIC_SET_PRIORITY( IRQ_PORTCD, return_priority_cd );//return priority
        __disable_irq( );
        attachInterruptVector( IRQ_PORTA,  return_porta_irq );// set previous isr func
        attachInterruptVector( IRQ_PORTCD, return_portcd_irq );// set previous isr func
        __enable_irq( );
        if ( return_isr_a_enabled == 0 )  NVIC_DISABLE_IRQ( IRQ_PORTA );
        if ( return_isr_cd_enabled == 0 ) NVIC_DISABLE_IRQ( IRQ_PORTCD );
    }
}

/*******************************************************************************
 *  for LLWU wakeup ISR to call actual digital ISR code
 *******************************************************************************/
void SnoozeDigital::clearIsrFlags( uint32_t ipsr ) {
    //int source = -1;
    
    /*uint32_t isfr_a = PORTA_ISFR;
    PORTA_ISFR = isfr_a;
    uint32_t isfr_c = PORTC_ISFR;
    uint32_t isfr_d = PORTD_ISFR;
    PORTC_ISFR = isfr_c;
    PORTD_ISFR = isfr_d;*/
    
    // return if using deepSleep or hibernate
    if ( sleep_type != 1 ) return;
    uint32_t _pin = isr_pin;
    while ( __builtin_popcount( _pin ) ) {
        uint32_t pinNumber = 31 - __builtin_clz( _pin );
        if ( pinNumber > 33 ) return;
        detachDigitalInterrupt( pinNumber );// remove pin interrupt
        _pin &= ~( ( uint32_t )1 << pinNumber );// remove pin from local list
    }
    
    // get source of interrupt
    /*if ( isfr_a & CORE_PIN3_BITMASK )       source = 3;
    else if ( isfr_a & CORE_PIN4_BITMASK  ) source = 4;
    
    else if ( isfr_c & CORE_PIN9_BITMASK )  source = 9;
    else if ( isfr_c & CORE_PIN10_BITMASK ) source = 10;
    else if ( isfr_c & CORE_PIN11_BITMASK ) source = 11;
    else if ( isfr_c & CORE_PIN12_BITMASK ) source = 12;
    else if ( isfr_c & CORE_PIN13_BITMASK ) source = 13;
    else if ( isfr_c & CORE_PIN15_BITMASK ) source = 15;
    else if ( isfr_c & CORE_PIN22_BITMASK ) source = 22;
    else if ( isfr_c & CORE_PIN23_BITMASK ) source = 23;
    
    else if ( isfr_d & CORE_PIN2_BITMASK )  source = 2;
    else if ( isfr_d & CORE_PIN5_BITMASK )  source = 5;
    else if ( isfr_d & CORE_PIN6_BITMASK )  source = 6;
    else if ( isfr_d & CORE_PIN7_BITMASK )  source = 7;
    else if ( isfr_d & CORE_PIN8_BITMASK )  source = 8;
    else if ( isfr_d & CORE_PIN14_BITMASK ) source = 14;
    else if ( isfr_d & CORE_PIN20_BITMASK ) source = 20;
    else if ( isfr_d & CORE_PIN21_BITMASK ) source = 21;
    wake_source = source;*/
}

/*******************************************************************************
 *  sleep pin wakeups go through NVIC
 *
 *  @param pin  Teensy Pin
 *  @param mode CHANGE, FALLING or LOW, RISING or HIGH
 *******************************************************************************/
void SnoozeDigital::attachDigitalInterrupt( uint8_t pin, int mode ) {
    volatile uint32_t *config;
    uint32_t cfg, mask;
    
    if ( pin >= CORE_NUM_DIGITAL ) return;
    switch ( mode ) {
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
#endif /* __MKL26Z64__ */
