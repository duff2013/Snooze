/*
 ||
 || @file 		Snooze.h
 || @version 	0
 || @author 	duff
 || @contact    http://forum.pjrc.com/members/25610-duff
 ||
 || @description
 || # Low Power Library for Teensy 3.0/3.1.
 ||
 || @license
 || | Copyright (c) 2014 Colin Duffy
 || | This library is free software; you can redistribute it and/or
 || | modify it under the terms of the GNU Lesser General Public
 || | License as published by the Free Software Foundation; version
 || | 2.1 of the License.
 || |
 || | This library is distributed in the hope that it will be useful,
 || | but WITHOUT ANY WARRANTY; without even the implied warranty of
 || | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 || | Lesser General Public License for more details.
 || |
 || | You should have received a copy of the GNU Lesser General Public
 || | License along with this library; if not, write to the Free Software
 || | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 || #
 ||
 */
#ifndef Snooze_h
#define Snooze_h

#if(!defined(__arm__) && defined(TEENSYDUINO))
#error Teensyduino 1.20 and Teensy 3.x only.
#endif

#include "Arduino.h"
#include "utility/lvd.h"
#include "utility/mcg.h"
#include "utility/smc.h"
#include "utility/rtc.h"
#include "utility/cmp.h"
#include "utility/tsi.h"
#include "utility/util.h"
#include "utility/llwu.h"
#include "utility/lptmr.h"
#include "utility/digital.h"
#include "utility/peripheral.h"

/*  USE_HIBERNATE is advanced feature that allows
    the Teensy to achieve 10 uA Sleep. This puts 
    the USB regulator into a low power state and
    configures pin PTA3 as TSI. There are times
    that the mcu will not see any valid reprogram
    signal from the USB when in this sleep mode.
    Use this only when you verfied that your
    program sleep works correctly with deepSleep! 
    
    For your Teensy's saftey I choose to add a small
    delay in the constructor to allow a small window
    for reprogramming if you go straight to sleep.
*/
//#define USE_HIBERNATE

/* extended pinMode types */
typedef enum {
    TSI          = 3,
    CMP          = 4,
} PIN_TYPE;

/* Deep Sleep Modes */
typedef enum {
    LLS,
    VLLS3,
    VLLS2,
    VLLS1,
    VLLS0
} SLEEP_MODE;
/****************************************************************************************/
class SnoozeBlock {
private:
    friend class SnoozeClass;
    typedef void* ISR;
    digital_mask_t      digital_mask;
    lptmr_mask_t        lptmr_mask;
    llwu_mask_t         llwu_mask;
    tsi_mask_t          tsi_mask;
    rtc_mask_t          rtc_mask;
    cmp_mask_t          cmp_mask;
    lvd_mask_t          lvd_mask;
    /* Peripherals */
    union periph_t {
        peripheral_mask_t   periph_on_mask;
        peripheral_mask_t   periph_off_mask;
        void operator = ( const SCGC4_OFF_t &rhs ) { peripheral_configure_scgc4_mask( rhs, &periph_off_mask ); }
        void operator = ( const SCGC5_OFF_t &rhs ) { peripheral_configure_scgc5_mask( rhs, &periph_off_mask ); }
        void operator = ( const SCGC6_OFF_t &rhs ) { peripheral_configure_scgc6_mask( rhs, &periph_off_mask ); }
        void operator = ( const SCGC4_ON_t &rhs  ) { peripheral_configure_scgc4_mask( rhs, &periph_on_mask ); }
        void operator = ( const SCGC5_ON_t &rhs  ) { peripheral_configure_scgc5_mask( rhs, &periph_on_mask ); }
        void operator = ( const SCGC6_ON_t &rhs  ) { peripheral_configure_scgc6_mask( rhs, &periph_on_mask ); }
    };
public:
    SnoozeBlock( void ) { };
    /* GPIO, TSI, COMPARE Config */
    void pinMode ( int pin, int mode, int val );
    void pinMode ( int pin, int mode, int type, double val );
    /* LPTMR Config */
    void setTimer( uint32_t period );
    /* RTC Config */
    void setAlarm( uint8_t hours, uint8_t minutes, uint8_t seconds );
    /* Low Voltage Config */
    void setLowVoltage( double threshold );
    /* Peripherals Config */
    periph_t setPeripheral;
};
/****************************************************************************************/
class SnoozeClass {
private:
    static SLEEP_MODE sleep_mode;
    static CLOCK_MODE clock_mode;
    static void wakeupISR( void );
public:
    SnoozeClass  ( void );
    /* helpers functions */
    void debug   ( Stream *port );
    int source   ( void );
    /* sleep functions */
    void idle    ( void );
    int sleep    ( SnoozeBlock &configuration );
    int deepSleep( SnoozeBlock &configuration, SLEEP_MODE mode = LLS );
#if defined( USE_HIBERNATE )
    int hibernate( SnoozeBlock &configuration, SLEEP_MODE mode = LLS );
#endif
};

extern SnoozeClass Snooze;
#endif