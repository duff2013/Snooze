/***********************************************************************************
 *  tsi.h
 *  Teensy 3.x/LC
 *
 * Purpose: Touch Sense
 *
 ***********************************************************************************/
#ifndef __TSI_H__
#define __TSI_H__
/***********************************************************************************/
#include <utility/util.h>
/***********************************************************************************/
static void ( * return_tsi0_irq ) ( void );
/***********************************************************************************/
typedef struct {
    uint8_t  pin = 0x03;
    uint16_t threshold;
    bool state = false;
} tsi_mask_t;
/***********************************************************************************/
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
        if( !( SIM_SCGC5 & SIM_SCGC5_TSI ) || !( irqEnabledFlag & TSI_IRQ_BIT ) ) return;
        TSI0_GENCS = TSI_GENCS_OUTRGF | TSI_GENCS_EOSF;
#if defined(KINETISL)
        LPTMR0_CSR = LPTMR_CSR_TCF;
        SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;
#endif
        irqEnabledFlag &= ~TSI_IRQ_BIT;
        if ( enable_periph_irq ) wakeupSource = 37;
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
            
            int priority = nvic_execution_priority( );// get current priority
            // if running from handler set priority higher than current handler
            priority = ( priority  < 256 ) && ( (priority - 16) > 0 ) ? priority - 16 : 128;
            NVIC_SET_PRIORITY( IRQ_TSI, priority );//set priority to new level
            
            __disable_irq( );
            return_tsi0_irq = _VectorsRam[IRQ_TSI+16];// save prev isr
            attachInterruptVector( IRQ_TSI, tsiISR );
            __enable_irq( );
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
                      ( TSI_GENCS_STM )          |
                      ( TSI_GENCS_OUTRGF )       |
                      ( TSI_GENCS_EOSF )         |
                      ( TSI_GENCS_TSIEN )
                      );
        
        SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL( 3 );
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
        LPTMR0_PSR = LPTMR_PSR_PBYP | LPTMR_PSR_PCS(1);// LPO Clock
        LPTMR0_CMR = 1;
        LPTMR0_CSR = LPTMR_CSR_TEN | LPTMR_CSR_TCF;
#endif
        irqEnabledFlag |= TSI_IRQ_BIT;
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
            NVIC_SET_PRIORITY( IRQ_TSI, 128 );// return priority core level
            __disable_irq( );
            attachInterruptVector( IRQ_TSI, return_tsi0_irq );// return prev interrupt
            __enable_irq( );
        }
        TSI0_GENCS &= ~TSI_GENCS_TSIEN;
    }
#ifdef __cplusplus
}
#endif
/***********************************************************************************/
#endif /* __TSI_H__ */
