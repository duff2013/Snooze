/***********************************************************************************
 *  rtc.h
 *  Teensy 3.x/LC
 *
 * Purpose: Real Time Clock
 *
 ***********************************************************************************/
#ifndef __RTC_H__
#define __RTC_H__
/***********************************************************************************/
#include <utility/util.h>
/***********************************************************************************/
static void ( * return_rtc_irq ) ( void );
/***********************************************************************************/
typedef struct {
    time_t alarm;
    bool state = false;
}rtc_mask_t;
/***********************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       rtc_configure_alarm_mask
     *
     *******************************************************************************/
    static inline
    void rtc_configure_alarm_mask( uint8_t hours, uint8_t minutes, uint8_t seconds, rtc_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void rtc_configure_alarm_mask( uint8_t hours, uint8_t minutes, uint8_t seconds, rtc_mask_t *mask ) {
        mask->alarm = hours*3600 + minutes*60 + seconds;
        mask->state = true;
    }
    /*******************************************************************************
     *
     *       rtc_set_sync_provider
     *
     *******************************************************************************/
    static inline
    time_t rtc_set_sync_provider( void )
    __attribute__((always_inline, unused));
    
    static inline
    time_t rtc_set_sync_provider( void ) {
        return Teensy3Clock.get();
    }
    /*******************************************************************************
     *
     *       rtcISR
     *
     *******************************************************************************/
    static inline
    void rtcISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void rtcISR( void ) {
        if ( !(SIM_SCGC6 & SIM_SCGC6_RTC) || !( irqEnabledFlag & RTC_IRQ_BIT ) ) return;
        RTC_TAR = RTC_TSR+1;
        irqEnabledFlag &= ~RTC_IRQ_BIT;
        if ( enable_periph_irq ) wakeupSource = 35;
    }
    /*******************************************************************************
     *
     *       rtc_alarm_set
     *
     *******************************************************************************/
    static inline
    void rtc_alarm_set( rtc_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void rtc_alarm_set( rtc_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) {
            
            int priority = nvic_execution_priority( );// get current priority
            // if running from handler set priority higher than current handler
            priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
            NVIC_SET_PRIORITY( IRQ_RTC_ALARM, priority );//set priority to new level
            
            __disable_irq( );
            return_rtc_irq = _VectorsRam[IRQ_RTC_ALARM+16];// save prev isr
            attachInterruptVector( IRQ_RTC_ALARM, rtcISR );
            __enable_irq( );
        }
        SIM_SCGC6 |= SIM_SCGC6_RTC;
        RTC_TAR = rtc_get( ) + ( mask->alarm - 1 );
        RTC_IER = RTC_IER_TAIE_MASK;
        if ( enable_periph_irq ) NVIC_ENABLE_IRQ( IRQ_RTC_ALARM );
        irqEnabledFlag |= RTC_IRQ_BIT;
        
    }
    /*******************************************************************************
     *
     *       rtc_stop
     *
     *******************************************************************************/
    static inline
    void rtc_disable( rtc_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void rtc_disable( rtc_mask_t *mask ) {
        if ( mask->state == false ) return;
        if ( enable_periph_irq ) {
            NVIC_DISABLE_IRQ( IRQ_RTC_ALARM );
            NVIC_SET_PRIORITY( IRQ_RTC_ALARM, 128 );//return priority to core value
            __disable_irq( );
            attachInterruptVector( IRQ_RTC_ALARM, return_rtc_irq );// return prev interrupt
            __enable_irq( );
        }
        RTC_IER = 0;
    }
#ifdef __cplusplus
}
#endif
/***********************************************************************************/
#endif /* __RTC_H__ */
