/*******************************************************************************
 *  tsi.h
 *  Teensy 3.x/LC
 *
 * Purpose: Touch Sense
 *
 *******************************************************************************/
#ifndef __TSI_H__
#define __TSI_H__
/*******************************************************************************/
#include <utility/util.h>

typedef struct {
    uint8_t  pin = 0x03;
    uint16_t threshold;
    bool state = false;
} tsi_mask_t;
/********************************************************************/
#if defined(KINETISK)
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
    255, 255,  11,   5
};
#elif defined(KINETISL)
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255, 255, 255
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       tsi_configure_pin_mask
     *
     *******************************************************************************/
    static inline
    void tsi_configure_pin_mask( uint8_t pin, uint16_t threshold, tsi_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void tsi_configure_pin_mask( uint8_t pin, uint16_t threshold, tsi_mask_t *mask ) {
        mask->pin = pin;
        mask->threshold = threshold;
        mask->state = true;
    }
    /*******************************************************************************
     *
     *       tsiISR
     *
     *******************************************************************************/
    static inline
    void tsiISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void tsiISR( void ) {
        if( !( SIM_SCGC5 & SIM_SCGC5_TSI ) ) return;
        TSI0_GENCS = TSI_GENCS_OUTRGF | TSI_GENCS_EOSF;
#if defined(KINETISL)
        LPTMR0_CSR = LPTMR_CSR_TCF;
        SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#endif
        wakeupSource = 35;
    }
    /*******************************************************************************
     *
     *       tsi_set
     *
     *******************************************************************************/
    static inline
    void tsi_set( tsi_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void tsi_set( tsi_mask_t *mask ) {
        if ( mask->state == false ) return;
        uint8_t pin = mask->pin;
        if ( pin >= NUM_DIGITAL_PINS ) return;
        uint16_t threshold = mask->threshold;
        if ( enable_periph_irq ) {
            attachInterruptVector( IRQ_TSI, tsiISR );
            NVIC_ENABLE_IRQ( IRQ_TSI );
        }
        
        uint32_t ch;
        
        ch = tsi_pins[pin];
        if ( ch == 255 ) return;
        
        *portConfigRegister( pin ) = PORT_PCR_MUX( 0 );
        SIM_SCGC5 |= SIM_SCGC5_TSI;
        
        TSI0_GENCS = 0;
#if defined(KINETISK)
        TSI0_THRESHOLD = threshold;
        TSI0_SCANC =  (
                      ( TSI_SCANC_EXTCHRG( 2 ) ) |
                      ( TSI_SCANC_REFCHRG( 3 ) ) |
                      ( TSI_SCANC_AMPSC( 1 ) )   |
                      ( TSI_SCANC_AMCLKS( 0 ) )
                      );
        TSI0_PEN = TSI_PEN_LPSP( ch );
        TSI0_GENCS = (
                      ( TSI_GENCS_NSCN( 9 ) )     |
                      ( TSI_GENCS_PS( 2 ) )       |
                      ( TSI_GENCS_STPE )          |
                      ( TSI_GENCS_LPSCNITV( 7 ) ) |
                      ( TSI_GENCS_TSIIE )         |
                      ( TSI_GENCS_OUTRGF )        |
                      ( TSI_GENCS_EOSF )          |
                      ( TSI_GENCS_TSIEN )
                      );
#elif defined(KINETISL)
        TSI0_TSHD = ( threshold << 16) & 0xFFFF0000;
        TSI0_DATA = TSI_DATA_TSICH( ch );
        TSI0_GENCS = (
                      ( TSI_GENCS_NSCN( 9 ) )    |
                      ( TSI_GENCS_PS( 2 ) )      |
                      ( TSI_GENCS_STPE )         |
                      ( TSI_GENCS_REFCHRG( 4 ) ) |
                      ( TSI_GENCS_EXTCHRG( 3 ) ) |
                      ( TSI_GENCS_TSIIEN )       |
                      ( TSI_GENCS_DVOLT( 0 ) )   |
                      //TSI_GENCS_ESOR             |
                      ( TSI_GENCS_STM )          |
                      ( TSI_GENCS_OUTRGF )       |
                      ( TSI_GENCS_EOSF )         |
                      ( TSI_GENCS_TSIEN )
                      );
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        LPTMR0_CMR = 1;
        LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
#endif

    }
    /*******************************************************************************
     *
     *       tsi_disable
     *
     *******************************************************************************/
    static inline
    void tsi_disable( tsi_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void tsi_disable( tsi_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) {
            NVIC_DISABLE_IRQ( IRQ_TSI );
            detachInterruptVector( IRQ_TSI );
        }
        TSI0_GENCS = TSI_GENCS_OUTRGF | TSI_GENCS_EOSF;
        TSI0_GENCS &= ~TSI_GENCS_TSIEN;
        SIM_SCGC5 &= ~SIM_SCGC5_TSI;
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __TSI_H__ */
