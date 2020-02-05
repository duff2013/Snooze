/***********************************************************************************
 *  SnoozeTouch.h
 *  Teensy 3.6
 *
 * Purpose: Touch (TSI) Driver
 *
 **********************************************************************************/
#if defined(__MK66FX1M0__)

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

#define MCG_S_CLKST_SHIFT    ( uint8_t )0x02
#define LPTMR_MCGIRCLK       ( uint8_t )0x00
#define LPTMR_LPO            ( uint8_t )0x01
#define LPTMR_ERCLK32K       ( uint8_t )0x02
#define LPTMR_OSCERCLK       ( uint8_t )0x03
#define MCG_C1_FRDIV4_BIT    ( uint8_t )0x05


static const uint8_t tsi_pins[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
    12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

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
void SnoozeTouch::disableDriver( uint8_t type ) {
    //if ( mode == RUN_LP || mode == VLPW ) return;
    if ( type <= 1 ) return;
    uint8_t _pin = pin;
    
    LPTMR0_PSR = PSR;
    LPTMR0_CMR = CMR;
    LPTMR0_CSR = CSR;
    if ( !timer_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
    
    TSI0_GENCS     &= ~TSI_GENCS_TSIEN;
    TSI0_DATA       = DATA;
    TSI0_TSHD       = THRESHOLD;
    volatile uint32_t *config;
    config = portConfigRegister( _pin );
    *config = return_core_pin_config;
    
    if ( !SIM_SCGC5_clock_active ) SIM_SCGC5 &= ~SIM_SCGC5_TSI;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::enableDriver( uint8_t type ) {
    //if ( mode == RUN_LP || mode == VLPW ) return;
    if ( type <= 1 ) return;
    uint8_t _pin = pin;
    if ( _pin >= NUM_DIGITAL_PINS ) return;
    
    if ( SIM_SCGC5 & SIM_SCGC5_LPTIMER ) timer_clock_active = true;
    else SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    PSR = LPTMR0_PSR;
    CMR = LPTMR0_CMR;
    CSR = LPTMR0_CSR;
    
    uint16_t _threshold = threshold;
    GENCS     = TSI0_GENCS;
    DATA      = TSI0_DATA;
    THRESHOLD = TSI0_TSHD;
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
    TSI0_TSHD = ( _threshold << 16 ) & 0xFFFF0000;
    TSI0_DATA = TSI_DATA_TSICH( ch );
    TSI0_GENCS =  (
                  ( TSI_GENCS_NSCN( 9 )    ) |
                  ( TSI_GENCS_PS( 2 )      ) |
                  ( TSI_GENCS_STPE         ) |
                  ( TSI_GENCS_REFCHRG( 4 ) ) |
                  ( TSI_GENCS_EXTCHRG( 3 ) ) |
                  ( TSI_GENCS_TSIIEN       ) |
                  ( TSI_GENCS_STM          ) |
                  ( TSI_GENCS_OUTRGF       ) |
                  ( TSI_GENCS_EOSF         ) |
                  ( TSI_GENCS_TSIEN        )
                  );
    
    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL( 3 );
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
    LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS( LPTMR_LPO );// LPO Clock
    LPTMR0_CMR = 1;
    LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
    llwu_configure_modules_mask( LLWU_TSI_MOD );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::clearIsrFlags( uint32_t ipsr ) {
    isr( );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeTouch::isr( void ) {
    if ( !( SIM_SCGC5 & SIM_SCGC5_TSI ) ) return;
    TSI0_GENCS = TSI_GENCS_OUTRGF | TSI_GENCS_EOSF;
}
#endif
