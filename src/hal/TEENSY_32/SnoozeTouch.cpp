/***********************************************************************************
 *  SnoozeTouch.h
 *  Teensy 3.2
 *
 *  Purpose: Touch (TSI) Driver
 *
 **********************************************************************************/
#if defined(__MK20DX256__)

#include "SnoozeTouch.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern void llwu_configure_modules_mask( uint8_t module );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

#define LLWU_TSI_MOD         ( uint8_t )0x40

    #define TSI_PEN_LPSP_MASK    0xF0000u
    #define TSI_PEN_LPSP_SHIFT   16
    #define TSI_PEN_LPSP(x)      (((uint32_t)(((uint32_t)(x))<<TSI_PEN_LPSP_SHIFT))&TSI_PEN_LPSP_MASK)

#if defined(__MK20DX256__) || defined(__MK20DX128__)
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
    255, 255,  11,   5
};
#elif defined(__MKL26Z64__)
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255, 255, 255
};
#elif defined(__MK66FX1M0__)
static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
    12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
#endif

/*******************************************************************************
 *  <#Description#>
 *
 *  @param _pin   <#_pin description#>
 *  @param thresh <#thresh description#>
 *******************************************************************************/
void SnoozeTouch::pinMode( int _pin, int thresh ) {
    isUsed = true;
    pin = _pin;
    threshold = thresh;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::disableDriver( uint8_t mode ) {
    if ( mode <= 1 ) return;
    uint8_t _pin = pin;
    
    TSI0_GENCS     &= ~TSI_GENCS_TSIEN;
    TSI0_GENCS      = GENCS;
    TSI0_THRESHOLD  = THRESHOLD;
    TSI0_SCANC      = SCANC;
    TSI0_PEN        = PEN;
    volatile uint32_t *config;
    config = portConfigRegister( _pin );
    *config = return_core_pin_config;
    
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_TSI;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::enableDriver( uint8_t mode ) {
    if ( mode <= 1 ) return;
    uint8_t _pin = pin;
    if ( _pin >= NUM_DIGITAL_PINS ) return;
    
    uint16_t _threshold = threshold;
    GENCS     = TSI0_GENCS;
    THRESHOLD = TSI0_THRESHOLD;
    SCANC     = TSI0_SCANC;
    PEN       = TSI0_PEN;
    uint32_t ch;
    ch = tsi_pins[pin];
    if ( ch == 255 ) return;
    
    volatile uint32_t *config;
    config = portConfigRegister( _pin );
    return_core_pin_config = *config;
    
    *portConfigRegister( _pin ) = PORT_PCR_MUX( 0 );
    
    if ( SIM_SCGC5  & SIM_SCGC5_TSI ) SIM_SCGC5_clock_active = true;
    else SIM_SCGC5 |= SIM_SCGC5_TSI;
    
    TSI0_GENCS = 0;
    TSI0_THRESHOLD = _threshold;
    TSI0_SCANC =  (
                   ( TSI_SCANC_EXTCHRG( 2 ) ) |
                   ( TSI_SCANC_REFCHRG( 3 ) ) |
                   ( TSI_SCANC_AMPSC( 1 )   ) |
                   ( TSI_SCANC_AMCLKS( 0 )  )
                   );
    TSI0_PEN = TSI_PEN_LPSP( ch );
    TSI0_GENCS = (
                  ( TSI_GENCS_NSCN( 9 )     ) |
                  ( TSI_GENCS_PS( 2 )       ) |
                  ( TSI_GENCS_LPSCNITV( 7 ) ) |
                  ( TSI_GENCS_STPE          ) |
                  ( TSI_GENCS_TSIIE         ) |
                  ( TSI_GENCS_OUTRGF        ) |
                  ( TSI_GENCS_EOSF          ) |
                  ( TSI_GENCS_TSIEN         )
                  );
    llwu_configure_modules_mask( LLWU_TSI_MOD );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::clearIsrFlags( uint32_t ipsr ) {
    if ( !( SIM_SCGC5 & SIM_SCGC5_TSI ) ) return;
    TSI0_GENCS = TSI_GENCS_OUTRGF | TSI_GENCS_EOSF;
}
#endif /* __MK20DX256__ */
