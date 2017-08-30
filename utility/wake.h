/***********************************************************************************
 * Low Power Library for Teensy LC/3.x
 * Copyright (c) 2014, Colin Duffy https://github.com/duff2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ***********************************************************************************
 *  wakeup.h
 *  Teensy 3.x/LC
 *
 * Purpose: Wake up control
 *
 ***********************************************************************************/
#ifndef wakeup_h
#define wakeup_h
/********************************************************************/
#include "Arduino.h"
/********************************************************************/
// module_enable defines
#define LLWU_LPTMR_MOD      0x01
//#ifdef KINETISK
#define LLWU_RTCA_MOD       0x02
#define LLWU_RTCS_MOD       0x04
//#endif
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

// FILT1 Bit Fields
#define LLWU_FILT1_FILTSEL_MASK                  0xFu
#define LLWU_FILT1_FILTSEL_SHIFT                 0
#define LLWU_FILT1_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTSEL_SHIFT))&LLWU_FILT1_FILTSEL_MASK)
#define LLWU_FILT1_FILTE_MASK                    0x60u
#define LLWU_FILT1_FILTE_SHIFT                   5
#define LLWU_FILT1_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTE_SHIFT))&LLWU_FILT1_FILTE_MASK)
#define LLWU_FILT1_FILTF_MASK                    0x80u
#define LLWU_FILT1_FILTF_SHIFT                   7
// FILT2 Bit Fields
#define LLWU_FILT2_FILTSEL_MASK                  0xFu
#define LLWU_FILT2_FILTSEL_SHIFT                 0
#define LLWU_FILT2_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTSEL_SHIFT))&LLWU_FILT2_FILTSEL_MASK)
#define LLWU_FILT2_FILTE_MASK                    0x60u
#define LLWU_FILT2_FILTE_SHIFT                   5
#define LLWU_FILT2_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTE_SHIFT))&LLWU_FILT2_FILTE_MASK)
#define LLWU_FILT2_FILTF_MASK                    0x80u
#define LLWU_FILT2_FILTF_SHIFT                   7
/********************************************************************/
typedef struct {
    uint8_t PE1;
    uint8_t PE2;
    uint8_t PE3;
    uint8_t PE4;
#if defined(HAS_KINETIS_LLWU_32CH)
    uint8_t PE5;
    uint8_t PE6;
    uint8_t PE7;
    uint8_t PE8;
#endif
    uint8_t ME;
    uint32_t wakeupSource;
    uint32_t llwuFlag;
} llwu_mask_t;

extern llwu_mask_t llwuMask;

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
    void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_pin_mask( uint8_t pin, uint8_t rise_fall ) {
        llwu_mask_t *mask = &llwuMask;
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
        else if ( pin == 59 ) mask->PE1 |= LLWU_PE1_WUPE0( detect_type );
        else if ( pin == 60 ) mask->PE1 |= LLWU_PE1_WUPE1( detect_type );
        else if ( pin == 62 ) mask->PE1 |= LLWU_PE1_WUPE2( detect_type );
    }
    /*******************************************************************************
     *
     *       llwu_configure_module
     *
     *******************************************************************************/
    static inline
    void llwu_configure_modules_mask( uint8_t module )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_modules_mask( uint8_t module ) {
        llwu_mask_t *mask = &llwuMask;
        if( module & LLWU_LPTMR_MOD )      mask->ME |= LLWU_ME_WUME0;
        else if ( module & LLWU_RTCA_MOD ) mask->ME |= LLWU_ME_WUME5;
        else if ( module & LLWU_RTCS_MOD ) mask->ME |= LLWU_ME_WUME7;
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
#if defined(HAS_KINETIS_LLWU_32CH)
        LLWU_PE5 = 0;
        LLWU_PE6 = 0;
        LLWU_PE7 = 0;
        LLWU_PE8 = 0;
#endif
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
    void llwu_set( void )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_set(void ) {
        llwu_mask_t *mask = &llwuMask;
        LLWU_PE1 = mask->PE1;
        LLWU_PE2 = mask->PE2;
        LLWU_PE3 = mask->PE3;
        LLWU_PE4 = mask->PE4;
#if defined(HAS_KINETIS_LLWU_32CH)
        LLWU_PE5 = mask->PE5;
        LLWU_PE6 = mask->PE6;
        LLWU_PE7 = mask->PE7;
        LLWU_PE8 = mask->PE8;
#endif
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
        uint32_t flag = ( LLWU_PF1 | LLWU_PF2 << 8 | LLWU_PF3 << 16 | LLWU_MF5 << 24 );
        LLWU_PF1 = 0xFF;
        LLWU_PF2 = 0xFF;
        LLWU_PF3 = 0xFF;
        LLWU_PF4 = 0xFF;
        LLWU_MF5 = 0xFF;
        LLWU_FILT1 = 0x80;
        LLWU_FILT2 = 0x80;
#elif defined(HAS_KINETIS_LLWU_16CH)
        uint32_t flag = ( LLWU_F1 | LLWU_F2 << 8 | LLWU_F3 << 16 );
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
        LLWU_FILT1 = 0x80;
        LLWU_FILT2 = 0x80;
#endif
        return flag;
    }
    /*******************************************************************************
     *
     *       llwu_disable -
     *
     *******************************************************************************/
    static inline
    int llwu_disable( void )
    __attribute__((always_inline, unused));
    
    static inline
    int llwu_disable( void ) {
        llwu_mask_t *mask = &llwuMask;
#if defined(HAS_KINETIS_LLWU_32CH)
        if      ( mask->llwuFlag & LLWU_PF1_WUF0 ) mask->wakeupSource = 26;
        else if ( mask->llwuFlag & LLWU_PF1_WUF3 ) mask->wakeupSource = 33;
        else if ( mask->llwuFlag & LLWU_PF1_WUF4 ) mask->wakeupSource = 4;
        else if ( mask->llwuFlag & LLWU_PF1_WUF5 ) mask->wakeupSource = 16;
        else if ( mask->llwuFlag & LLWU_PF1_WUF6 ) mask->wakeupSource = 22;
        else if ( mask->llwuFlag & LLWU_PF1_WUF7 ) mask->wakeupSource = 9;
        
        else if ( mask->llwuFlag & LLWU_PF1_WUF0 ) mask->wakeupSource = 59;
        else if ( mask->llwuFlag & LLWU_PF1_WUF1 ) mask->wakeupSource = 60;
        else if ( mask->llwuFlag & LLWU_PF1_WUF2 ) mask->wakeupSource = 62;
        
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF8  ) mask->wakeupSource = 10;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF9  ) mask->wakeupSource = 13;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF10 ) mask->wakeupSource = 11;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF11 ) mask->wakeupSource = 30;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF12 ) mask->wakeupSource = 2;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF13 ) mask->wakeupSource = 7;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF14 ) mask->wakeupSource = 6;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF15 ) mask->wakeupSource = 21;
        
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF15 ) mask->wakeupSource = 21;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_PF2_WUF15 ) mask->wakeupSource = 21;
        
        else if ( ( mask->llwuFlag>>24 ) & LLWU_ME_WUME0 ) mask->wakeupSource = 36;
        else if ( ( mask->llwuFlag>>24 ) & LLWU_ME_WUME1 ) mask->wakeupSource = 34;
        else if ( ( mask->llwuFlag>>24 ) & LLWU_ME_WUME4 ) mask->wakeupSource = 37;
        else if ( ( mask->llwuFlag>>24 ) & LLWU_ME_WUME5 ) mask->wakeupSource = 35;
        
#elif defined(HAS_KINETIS_LLWU_16CH)
        if      ( mask->llwuFlag & LLWU_F1_WUF0 ) mask->wakeupSource = 26;
        else if ( mask->llwuFlag & LLWU_F1_WUF3 ) mask->wakeupSource = 33;
        else if ( mask->llwuFlag & LLWU_F1_WUF4 ) mask->wakeupSource = 4;
        else if ( mask->llwuFlag & LLWU_F1_WUF5 ) mask->wakeupSource = 16;
        else if ( mask->llwuFlag & LLWU_F1_WUF6 ) mask->wakeupSource = 22;
        else if ( mask->llwuFlag & LLWU_F1_WUF7 ) mask->wakeupSource = 9;
        
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF8  ) mask->wakeupSource = 10;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF9  ) mask->wakeupSource = 13;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF10 ) mask->wakeupSource = 11;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF11 ) mask->wakeupSource = 30;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF12 ) mask->wakeupSource = 2;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF13 ) mask->wakeupSource = 7;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF14 ) mask->wakeupSource = 6;
        else if ( ( mask->llwuFlag>>8 ) & LLWU_F2_WUF15 ) mask->wakeupSource = 21;
        
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF0 ) mask->wakeupSource = 36;
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF1 ) mask->wakeupSource = 34;
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF4 ) mask->wakeupSource = 37;
    #ifdef KINETISK
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF3 ) mask->wakeupSource = 34;
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF2 ) mask->wakeupSource = 34;
        else if ( ( mask->llwuFlag>>16 ) & LLWU_F3_MWUF5 ) mask->wakeupSource = 35;
    #endif
#endif
        LLWU_PE1 = 0;
        LLWU_PE2 = 0;
        LLWU_PE3 = 0;
        LLWU_PE4 = 0;
#if defined(HAS_KINETIS_LLWU_32CH)
        LLWU_PE5 = 0;
        LLWU_PE6 = 0;
        LLWU_PE7 = 0;
        LLWU_PE8 = 0;
#endif
        mask->PE1 = 0;
        mask->PE2 = 0;
        mask->PE3 = 0;
        mask->PE4 = 0;
#if defined(HAS_KINETIS_LLWU_32CH)
        mask->PE5 = 0;
        mask->PE6 = 0;
        mask->PE7 = 0;
        mask->PE8 = 0;
#endif
        LLWU_ME  = mask->ME;
        mask->ME = 0;
        mask->llwuFlag = 0;
        return mask->wakeupSource;
    }
    /*******************************************************************************
     *
     *       llwu_configure_filter -
     *
     *******************************************************************************/
    static inline
    void llwu_configure_filter( unsigned int pin_num, unsigned char filter_en, unsigned char rise_fall )
    __attribute__((always_inline, unused));
    
    static inline
    void llwu_configure_filter( unsigned int pin_num, unsigned char filter_en, unsigned char rise_fall ) {
        //pin_num is the pin number to be written to FILTSEL.  pin_num is not the same as pin_en.
        uint8_t temp;
        
        temp = 0;
        //first clear filter values and clear flag by writing a 1
        LLWU_FILT1 = LLWU_FILT1_FILTF_MASK;
        LLWU_FILT2 = LLWU_FILT2_FILTF_MASK;
        
        if( filter_en == 1 ) {
            //clear the flag bit and set the others
            temp |= ( LLWU_FILT1_FILTF_MASK ) | ( LLWU_FILT1_FILTE( rise_fall ) | LLWU_FILT1_FILTSEL( pin_num ) );
            LLWU_FILT1 = temp;
        }else if ( filter_en == 2 ) {
            //clear the flag bit and set the others
            temp |= ( LLWU_FILT2_FILTF_MASK ) | ( LLWU_FILT2_FILTE( rise_fall ) | LLWU_FILT2_FILTSEL( pin_num ) );
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
        //LLWU_RST = 0x02;//
    }
    /*******************************************************************************
     *
     *       startup_early_hook -
     *
     *******************************************************************************/
    void startup_early_hook( void ) __attribute__ ((weak, noinline));
    void startup_early_hook( void ) {
#if defined(KINETISK)
        WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;
#elif defined(KINETISL)
        SIM_COPC = 0;  // disable the watchdog
#endif
        if ( PMC_REGSC & PMC_REGSC_ACKISO ) {
            llwuMask.llwuFlag = llwu_clear_flags( );// clear flags
            llwuMask.wakeupSource = llwu_disable( );
        }
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/

/*#include "SnoozeBlock.h"
class SnoozeWake : public SnoozeBlockV2 {
private:
    virtual void enableDriver( void );
    virtual void disableDriver( void );
    virtual void clearIsrFlags( void );
    void pinClear( void );
    void moduleClear( void );
    static void isr( void );
    uint8_t PE1;
    uint8_t PE2;
    uint8_t PE3;
    uint8_t PE4;
    uint8_t ME;
    static volatile int flag;
    //static void startup_early_hook( void ) { }
public:
    SnoozeWake( void ) {
        isUsed = true;
        flag = false;
        //attachInterruptVector( IRQ_LLWU, isr );
        //NVIC_ENABLE_IRQ( IRQ_LLWU );
    }
    void pinConfigure( uint8_t pin, uint8_t rise_fall );
    void moduleConfigure( uint8_t module );
    static uint32_t clearFlags( void );
};*/
#endif /* defined(SnoozeWakeup_h) */
