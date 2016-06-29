/*******************************************************************************
 *  llwu.h
 *  Teensy 3.x/LC
 *
 * Purpose: Low Leakage Wakeup Unit
 *
 *******************************************************************************/
#ifndef __LLWU_H__
#define __LLWU_H__
/********************************************************************/
#include "kinetis_ext.h"
/********************************************************************/
// module_enable defines
#define LLWU_LPTMR_MOD      0x01
#ifdef KINETISK
#define LLWU_RTCA_MOD       0x02
#define LLWU_RTCS_MOD       0x04
#endif
#define LLWU_CMP0_MOD       0x08
#define LLWU_CMP1_MOD       0x10
#ifdef KINETISK
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
         if( module & LLWU_LPTMR_MOD )      mask->ME |= LLWU_ME_WUME0;
#ifdef KINETISK
         else if ( module & LLWU_RTCA_MOD ) mask->ME |= LLWU_ME_WUME5;
         else if ( module & LLWU_RTCS_MOD ) mask->ME |= LLWU_ME_WUME7;
#endif
         else if ( module & LLWU_TSI_MOD )  mask->ME |= LLWU_ME_WUME4;
         else if ( module & LLWU_CMP0_MOD ) mask->ME |= LLWU_ME_WUME1;
         else if ( module & LLWU_CMP1_MOD ) mask->ME |= LLWU_ME_WUME2;
#ifdef KINETISK
         else if( module & LLWU_CMP2_MOD )  mask->ME |= LLWU_ME_WUME3;
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
#if defined(HAS_KINETIS_LLWU_32CH)
        uint32_t flag = ( LLWU_PF1 | LLWU_PF2<<8 | LLWU_PF3<<16 | LLWU_MF5<<24 );
        LLWU_PF1 = 0xFF;
        LLWU_PF2 = 0xFF;
        LLWU_PF3 = 0xFF;
        LLWU_PF4 = 0xFF;
        LLWU_MF5 = 0xFF;
        LLWU_FILT1 = 0x80;
        LLWU_FILT2 = 0x80;
        return flag;
#elif defined(HAS_KINETIS_LLWU_16CH)
        uint32_t flag = ( LLWU_F1 | LLWU_F2<<8 | LLWU_F3<<16 );
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
        LLWU_FILT1 = 0x80;
        LLWU_FILT2 = 0x80;
        return flag;
#endif
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
#if defined(HAS_KINETIS_LLWU_32CH)
        LLWU_PE4 = 0;
        LLWU_PE5 = 0;
        LLWU_PE6 = 0;
        LLWU_PE7 = 0;
        LLWU_PE8 = 0;
#endif
        LLWU_ME  = 0;
#if defined(HAS_KINETIS_LLWU_32CH)
        if      ( llwuFlag & LLWU_PF1_WUF0 ) wakeupSource = 26;
        else if ( llwuFlag & LLWU_PF1_WUF3 ) wakeupSource = 33;
        else if ( llwuFlag & LLWU_PF1_WUF4 ) wakeupSource = 4;
        else if ( llwuFlag & LLWU_PF1_WUF5 ) wakeupSource = 16;
        else if ( llwuFlag & LLWU_PF1_WUF6 ) wakeupSource = 22;
        else if ( llwuFlag & LLWU_PF1_WUF7 ) wakeupSource = 9;
        
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF8  ) wakeupSource = 10;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF9  ) wakeupSource = 13;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF10 ) wakeupSource = 11;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF11 ) wakeupSource = 30;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF12 ) wakeupSource = 2;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF13 ) wakeupSource = 7;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF14 ) wakeupSource = 6;
        else if ( ( llwuFlag>>8 ) & LLWU_PF2_WUF15 ) wakeupSource = 21;
        
        else if ( ( llwuFlag>24 ) & LLWU_ME_WUME0 ) wakeupSource = 36;
        else if ( ( llwuFlag>>24 ) & LLWU_ME_WUME1 ) wakeupSource = 34;
        else if ( ( llwuFlag>>24 ) & LLWU_ME_WUME4 ) wakeupSource = 37;
        else if ( ( llwuFlag>>24 ) & LLWU_ME_WUME5 ) wakeupSource = 35;
#elif defined(HAS_KINETIS_LLWU_16CH)
        if      ( llwuFlag & LLWU_F1_WUF0 ) wakeupSource = 26;
        else if ( llwuFlag & LLWU_F1_WUF3 ) wakeupSource = 33;
        else if ( llwuFlag & LLWU_F1_WUF4 ) wakeupSource = 4;
        else if ( llwuFlag & LLWU_F1_WUF5 ) wakeupSource = 16;
        else if ( llwuFlag & LLWU_F1_WUF6 ) wakeupSource = 22;
        else if ( llwuFlag & LLWU_F1_WUF7 ) wakeupSource = 9;
        
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF8  ) wakeupSource = 10;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF9  ) wakeupSource = 13;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF10 ) wakeupSource = 11;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF11 ) wakeupSource = 30;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF12 ) wakeupSource = 2;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF13 ) wakeupSource = 7;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF14 ) wakeupSource = 6;
        else if ( ( llwuFlag>>8 ) & LLWU_F2_WUF15 ) wakeupSource = 21;
        
        else if ( ( llwuFlag>>16 ) & LLWU_ME_WUME0 ) wakeupSource = 36;
        else if ( ( llwuFlag>>16 ) & LLWU_ME_WUME1 ) wakeupSource = 34;
        else if ( ( llwuFlag>>16 ) & LLWU_ME_WUME4 ) wakeupSource = 37;
#ifdef KINETISK
        else if ( ( llwuFlag>>16 ) & LLWU_ME_WUME5 ) wakeupSource = 35;
#endif
#endif
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
#if defined(HAS_KINETIS_LLWU_32CH)
        //LLWU_FILT1 = LLWU_FILT1_FILTF;
        //LLWU_FILT2 = LLWU_FILT2_FILTF;
        //LLWU_FILT3 = LLWU_FILT2_FILTF;
        //LLWU_FILT4 = LLWU_FILT2_FILTF;
#elif defined(HAS_KINETIS_LLWU_16CH)
        //LLWU_FILT1 = LLWU_FILT1_FILTF;
        //LLWU_FILT2 = LLWU_FILT2_FILTF;
#endif
        /*if( filter_en == 1 ) {
            //clear the flag bit and set the others
            temp |= ( LLWU_FILT1_FILTF ) | ( LLWU_FILT1_FILTE( rise_fall ) | LLWU_FILT1_FILTSEL( wu_pin_num ) );
            LLWU_FILT1 = temp;
        }else if ( filter_en == 2 ) {
            //clear the flag bit and set the others
            temp |= ( LLWU_FILT2_FILTF ) | ( LLWU_FILT2_FILTE( rise_fall ) | LLWU_FILT2_FILTSEL( wu_pin_num ) );
            LLWU_FILT2 = temp;
        }else {
            
        }*/
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
        //LLWU_RST = 0x02;//LLWU_RST_LLRSTE_MASK;
    }
    /*******************************************************************************
     *
     *       startup_early_hook -
     *
     *******************************************************************************/
    void startup_early_hook( ) __attribute__ ((weak));
    void startup_early_hook( ) {
#if defined(KINETISK)
        WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;
#elif defined(KINETISL)
        SIM_COPC = 0;  // disable the watchdog
#endif
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
