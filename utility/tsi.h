/*******************************************************************************
 *  tsi.h
 *  Teensy3.x
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
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
    255, 255,  11,   5
};

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
        if( !(SIM_SCGC5 & SIM_SCGC5_TSI) ) return;
        TSI0_GENCS |= TSI_GENCS_OUTRGF;
        TSI0_GENCS |= TSI_GENCS_EOSF;
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
        attachInterruptVector( IRQ_TSI, tsiISR );
        uint8_t pin = mask->pin;
        uint16_t threshold = mask->threshold;
        
        if ( tsi_pins[pin] == 255 ) return;
        
        SIM_SCGC5 |= SIM_SCGC5_TSI;
        
        TSI0_GENCS = 0;
        
        TSI0_THRESHOLD = threshold;
        
        TSI0_GENCS |= TSI_GENCS_EOSF;
        
        TSI0_GENCS |= ((TSI_GENCS_NSCN(9)) |
                       (TSI_GENCS_PS(2)) |
                       (TSI_GENCS_STPE) |
                       (TSI_GENCS_LPSCNITV(7)) |
                       (TSI_GENCS_TSIIE));
        
        TSI0_SCANC |= ((TSI_SCANC_EXTCHRG(2)) |
                       (TSI_SCANC_REFCHRG(3)) |
                       (TSI_SCANC_AMPSC(1)) |
                       (TSI_SCANC_AMCLKS(0)));
        
        TSI0_PEN |= TSI_PEN_LPSP(tsi_pins[pin]);
        
        TSI0_GENCS |= TSI_GENCS_TSIEN;  //Enables TSI
        
        // Wait for any in-progress scans to complete
        while (TSI0_GENCS & TSI_GENCS_SCNIP) yield();
        
        TSI0_GENCS |= TSI_GENCS_OUTRGF;   //Clear all pending flags
        TSI0_GENCS |= TSI_GENCS_EOSF;
        NVIC_ENABLE_IRQ( IRQ_TSI );
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
        detachInterruptVector( IRQ_TSI );
        TSI0_GENCS |= TSI_GENCS_OUTRGF;
        TSI0_GENCS |= TSI_GENCS_EOSF;
        TSI0_GENCS &= ~TSI_GENCS_TSIEN;
        SIM_SCGC5 &= ~SIM_SCGC5_TSI;
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __TSI_H__ */
