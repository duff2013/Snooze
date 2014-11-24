/*******************************************************************************
 *  llwu.h
 *  Teensy3.x
 *
 * Purpose: Low Leakage Wakeup Unit
 *
 *******************************************************************************/
#ifndef __LLWU_H__
#define __LLWU_H__
/********************************************************************/
#include "kinetis_ext.h"
#include "bitband.h"
/********************************************************************/
// module_enable defines
#define LLWU_LPTMR_MOD      0x01
#define LLWU_RTCA_MOD       0x02
#define LLWU_RTCS_MOD       0x04
#define LLWU_CMP0_MOD       0x08
#define LLWU_CMP1_MOD       0x10
#if defined(__MK20DX256__)
#define LLWU_CMP2_MOD       0x20
#endif
#define LLWU_TSI_MOD        0x40
// external pin interrupt type defines
#define LLWU_PIN_DIS        0x00
#define LLWU_PIN_RISING     0x01
#define LLWU_PIN_FALLING    0x02
#define LLWU_PIN_ANY        0x03
/********************************************************************/
typedef struct {
    uint8_t PE1;
    uint8_t PE2;
    uint8_t PE3;
    uint8_t PE4;
    uint8_t ME;
} llwu_mask_t;

#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       llwu configures -
     *
     * description: Set up the LLWU for wakeup of the MCU from LLS and VLLSx modes
     * from the selected pin or module.
     *
     * inputs:  pin - indicates the wakeup pin is enabled.
     *          rise_fall - 0x00 = External input disabled as wakeup
     *                      0x01 - External input enabled as rising edge detection
     *                      0x02 - External input enabled as falling edge detection
     *                      0x03 - External input enabled as any edge detection
     *          module_en - indicates the wakeup module is enabled.
     *******************************************************************************/
    /*******************************************************************************
     *
     *       llwu_configure_module
     *
     *******************************************************************************/
    static inline
    void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall, llwu_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall, llwu_mask_t *mask ) {
        uint8_t detect_type;
        if ( rise_fall == RISING )       detect_type = LLWU_PIN_RISING;
        else if ( rise_fall == FALLING ) detect_type = LLWU_PIN_FALLING;
        else if ( rise_fall == CHANGE )  detect_type = LLWU_PIN_ANY;
        else                             detect_type = LLWU_PIN_ANY;

        if ( pin == 2 )       mask->PE4 |= LLWU_PE4_WUPE12( detect_type );
        else if ( pin == 4 )  mask->PE2 |= LLWU_PE2_WUPE4 ( detect_type );
        else if ( pin == 6 )  mask->PE4 |= LLWU_PE4_WUPE14( detect_type );
        else if ( pin == 7 )  mask->PE4 |= LLWU_PE4_WUPE13( detect_type );
        else if ( pin == 9 )  mask->PE2 |= LLWU_PE2_WUPE7 ( detect_type );
        else if ( pin == 10 ) mask->PE3 |= LLWU_PE3_WUPE8 ( detect_type );
        else if ( pin == 11 ) mask->PE3 |= LLWU_PE3_WUPE10( detect_type );
        else if ( pin == 13 ) mask->PE3 |= LLWU_PE3_WUPE9 ( detect_type );
        else if ( pin == 16 ) mask->PE2 |= LLWU_PE2_WUPE5 ( detect_type );
        else if ( pin == 21 ) mask->PE4 |= LLWU_PE4_WUPE15( detect_type );
        else if ( pin == 22 ) mask->PE2 |= LLWU_PE2_WUPE6 ( detect_type );
        else if ( pin == 26 ) mask->PE1 |= LLWU_PE1_WUPE0 ( detect_type );
        else if ( pin == 30 ) mask->PE3 |= LLWU_PE3_WUPE11( detect_type );
        else if ( pin == 33 ) mask->PE1 |= LLWU_PE1_WUPE3 ( detect_type );
    }
    /*******************************************************************************
     *
     *       llwu_configure_module
     *
     *******************************************************************************/
    static inline
    void llwu_configure_modules_mask( uint8_t module, llwu_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_modules_mask( uint8_t module, llwu_mask_t *mask ) {
         if( module & LLWU_LPTMR_MOD)     mask->ME |= LLWU_ME_WUME0_MASK;
         else if ( module & LLWU_RTCA_MOD) mask->ME |= LLWU_ME_WUME5_MASK;
         else if ( module & LLWU_RTCS_MOD) mask->ME |= LLWU_ME_WUME7_MASK;
         else if ( module & LLWU_TSI_MOD ) mask->ME |= LLWU_ME_WUME4_MASK;
         else if ( module & LLWU_CMP0_MOD) mask->ME |= LLWU_ME_WUME1_MASK;
         else if ( module & LLWU_CMP1_MOD) mask->ME |= LLWU_ME_WUME2_MASK;
    #if defined(__MK20DX256__)
         else if( module & LLWU_CMP2_MOD) mask->ME |= LLWU_ME_WUME3_MASK;
    #endif
    }
    /*******************************************************************************
     *
     *       llwu_clear_pins
     *
     *******************************************************************************/
    static inline
    void llwu_clear_pins( void )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_clear_pins( void ) {
        LLWU_PE1 = 0;
        LLWU_PE2 = 0;
        LLWU_PE3 = 0;
        LLWU_PE4 = 0;
    }
    /*******************************************************************************
     *
     *       llwu_clear_modules-
     *
     *******************************************************************************/
    // clears all llwu module wakeups
    static inline
    void llwu_clear_modules( void )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_clear_modules( void ) {
        LLWU_ME = 0;
    }
    /*******************************************************************************
     *
     *       llwu_set -
     *
     *******************************************************************************/
    static inline
    void llwu_set( llwu_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_set( llwu_mask_t *mask ) {
        NVIC_ENABLE_IRQ( IRQ_LLWU );
        LLWU_PE1 = mask->PE1;
        LLWU_PE2 = mask->PE2;
        LLWU_PE3 = mask->PE3;
        LLWU_PE4 = mask->PE4;
        LLWU_ME  = mask->ME;
    }
    /*******************************************************************************
     *
     *       llwu_clear_flags -
     *
     *******************************************************************************/
    static inline
    uint32_t llwu_clear_flags( void )
    __attribute__((always_inline, unused));
    
    static inline
    uint32_t llwu_clear_flags( void ) {
        uint32_t flag = ( LLWU_F1 | LLWU_F2<<8 | LLWU_F3<<16 );
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
        LLWU_FILT1 = 0x80;
        LLWU_FILT2 = 0x80;
        return flag;
    }
    /*******************************************************************************
     *
     *       llwu_disable -
     *
     *******************************************************************************/
    static inline
    void llwu_disable( void )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_disable( void ) {
        LLWU_PE1 = 0;
        LLWU_PE2 = 0;
        LLWU_PE3 = 0;
        LLWU_PE4 = 0;
        LLWU_ME  = 0;
        if      ( llwuFlag & LLWU_F1_WUF0_MASK ) wakeupSource = 26;
        else if ( llwuFlag & LLWU_F1_WUF3_MASK ) wakeupSource = 33;
        else if ( llwuFlag & LLWU_F1_WUF4_MASK ) wakeupSource = 4;
        else if ( llwuFlag & LLWU_F1_WUF5_MASK ) wakeupSource = 16;
        else if ( llwuFlag & LLWU_F1_WUF6_MASK ) wakeupSource = 22;
        else if ( llwuFlag & LLWU_F1_WUF7_MASK ) wakeupSource = 9;
        
        else if ( (llwuFlag>>8) & LLWU_F2_WUF8_MASK  ) wakeupSource = 10;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF9_MASK  ) wakeupSource = 13;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF10_MASK ) wakeupSource = 11;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF11_MASK ) wakeupSource = 30;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF12_MASK ) wakeupSource = 2;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF13_MASK ) wakeupSource = 7;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF14_MASK ) wakeupSource = 6;
        else if ( (llwuFlag>>8) & LLWU_F2_WUF15_MASK ) wakeupSource = 21;
        
        else if ( (llwuFlag>>16) & LLWU_ME_WUME0_MASK ) wakeupSource = 36;
        else if ( (llwuFlag>>16) & LLWU_ME_WUME1_MASK ) wakeupSource = 34;
        else if ( (llwuFlag>>16) & LLWU_ME_WUME4_MASK ) wakeupSource = 37;
        else if ( (llwuFlag>>16) & LLWU_ME_WUME5_MASK ) wakeupSource = 35;
        llwuFlag = 0;
    }
    /*******************************************************************************
     *
     *       llwu_configure_filter -
     *
     *******************************************************************************/
    static inline
    void llwu_configure_filter( unsigned int wu_pin_num, unsigned char filter_en, unsigned char rise_fall )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_filter( unsigned int wu_pin_num, unsigned char filter_en, unsigned char rise_fall ) {
        //wu_pin_num is the pin number to be written to FILTSEL.  wu_pin_num is not the same as pin_en.
        uint8_t temp;
        
        temp = 0;
        //first clear filter values and clear flag by writing a 1
        LLWU_FILT1 = LLWU_FILT1_FILTF_MASK;
        LLWU_FILT2 = LLWU_FILT2_FILTF_MASK;
        
        if(filter_en == 1) {
            //clear the flag bit and set the others
            temp |= (LLWU_FILT1_FILTF_MASK) | (LLWU_FILT1_FILTE(rise_fall) | LLWU_FILT1_FILTSEL(wu_pin_num));
            LLWU_FILT1 = temp;
        }else if (filter_en == 2) {
            //clear the flag bit and set the others
            temp |= (LLWU_FILT2_FILTF_MASK) | (LLWU_FILT2_FILTE(rise_fall) | LLWU_FILT2_FILTSEL(wu_pin_num));
            LLWU_FILT2 = temp;
        }else {
            
        }
    }
    /*******************************************************************************
     *
     *       llwu_reset_enable -
     *
     *******************************************************************************/
    static inline
    void llwu_reset_enable( void )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_reset_enable( void ) {
        LLWU_RST = 0x02;//LLWU_RST_LLRSTE_MASK;
    }
    /*******************************************************************************
     *
     *       startup_early_hook -
     *
     *******************************************************************************/
    void startup_early_hook() __attribute__ ((weak));
    void startup_early_hook() {
        WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;
        if ( PMC_REGSC & PMC_REGSC_ACKISO ) {
            llwuFlag = llwu_clear_flags( );// clear flags
            llwu_disable( );
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* defined(__LLWU_H__) */
