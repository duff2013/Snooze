/*******************************************************************************
 *  rtc.h
 *  Teensy3.x
 *
 * Purpose: Real Time Clock
 *
 *******************************************************************************/
#ifndef __RTC_H__
#define __RTC_H__
/********************************************************************/
typedef struct {
    time_t alarm;
}rtc_mask_t;
/********************************************************************/
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
     *       rtc_alarm_set
     *
     *******************************************************************************/
    static inline
    void rtc_alarm_set( rtc_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void rtc_alarm_set( rtc_mask_t *mask ) {
        if ( mask->alarm == 0 ) return;
        SIM_SCGC6 |= SIM_SCGC6_RTC;
        RTC_TAR = rtc_get( ) + ( mask->alarm - 1 );
        RTC_IER = RTC_IER_TAIE_MASK;
        NVIC_ENABLE_IRQ(IRQ_RTC_ALARM);
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
        if ( mask->alarm == 0 ) return;
        RTC_IER = 0;
        NVIC_DISABLE_IRQ(IRQ_RTC_ALARM);
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
        if ( !(SIM_SCGC6 & SIM_SCGC6_RTC) ) return;
        RTC_TAR = RTC_TSR+1;
    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __RTC_H__ */
