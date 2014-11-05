/*******************************************************************************
 *  digital.h
 *  Teensy3.x
 *
 * Purpose: Digital Pins
 *
 *******************************************************************************/
#ifndef __DIGITAL_H__
#define __DIGITAL_H__
/********************************************************************/
typedef struct {
    uint64_t pin = 0;
    uint8_t mode_irqType[33];
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
        mask->pin  |= ( (uint64_t)0x01 << pin );
        mask->mode_irqType[pin] = type;
        mask->mode_irqType[pin] |= mode << 4;
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
        uint64_t _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            int pin = 63 - __builtin_clzll( _pin );
            int mode = mask->mode_irqType[pin] >> 4;
            int type = mask->mode_irqType[pin] & 0x0F;
            
            volatile uint32_t *config;
            config = portConfigRegister( pin );
            *portModeRegister( pin ) = 0;
            if ( mode == 0 ) *config = PORT_PCR_MUX( 1 );
            else *config = PORT_PCR_MUX( 1 ) | PORT_PCR_PE | PORT_PCR_PS;

            attachInterrupt(pin, digitalISR, type);
            _pin &= ~( (uint64_t)1<<pin );
        }
    }
    /*******************************************************************************
     *
     *       digital_disable
     *
     *******************************************************************************/
    static inline
    void digital_disable( digital_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void digital_disable( digital_mask_t *mask ) {
        uint64_t _pin = mask->pin;
        while ( __builtin_popcountll( _pin ) ) {
            int pin = 63 - __builtin_clzll( _pin );
            detachInterrupt(pin);
            _pin &= ~( (uint64_t)1<<pin );
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __DIGITAL_H__ */