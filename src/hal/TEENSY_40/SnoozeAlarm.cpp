/***********************************************************************************
 *  SnoozeAlarm.h
 *  Teensy 4.0
 *
 * Purpose: RTC Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)
#include "SnoozeAlarm.h"
//#include "TimeLib.h"

#define SNVS_DEFAULT_PGD_VALUE (0x41736166U)
#define SNVS_LPSR_PGD_MASK                       (0x8U)
#define SNVS_LPSRTCMR      (IMXRT_SNVS.offset050)
#define SNVS_LPSRTCLR      (IMXRT_SNVS.offset054)

#define SNVS_LPCR_LPTA_EN  (1 << 1)
#define SNVS_LPSR_LPTA    (1 << 0)

#ifdef __cplusplus
extern "C" {
#endif
    extern int nvic_execution_priority( void );
    extern void wakeup_isr( void );
#ifdef __cplusplus
}
#endif

volatile uint8_t SnoozeAlarm::sleep_type = 0;

static void rtc_sync_lp( void ) {
    uint32_t count1, count2, count3;
    // Wait for 3 CKIL cycles
    for ( int i = 0; i < 3; i++ ) {
        do {
            count1 = SNVS_LPSRTCLR;
            count2 = SNVS_LPSRTCLR;
        } while ( count1 != count2 );
        // Now wait until counter value changes
        do {
            do {
                count2 = SNVS_LPSRTCLR;
                count3 = SNVS_LPSRTCLR;
            } while ( count2 != count3 );
        } while ( count3 == count1 );
    }
}
/*******************************************************************************
 *  <#Description#>
 *
 *  @return current time
 *******************************************************************************/
time_t rtc_set_sync_provider( void ) {
    return Teensy3Clock.get( );
}

/*******************************************************************************
 *  <#Description#> Sets an alarm for a specific amount of time once the driver
 *                  is enabled. Is dependent of when the driver is enabled.
 *
 *  @param hours   0 - 24
 *  @param minutes 0 - 60
 *  @param seconds 0 - 60
 *******************************************************************************/
void SnoozeAlarm::setRtcTimer( uint8_t hours, uint8_t minutes, uint8_t seconds ) {
    isUsed = true;
    alarm = ( hours * 3600 ) + ( minutes * 60 ) + seconds;
    timer_ = true;
    //SNVS_LPCR &= ~SNVS_LPCR_LPTA_EN;
    //rtc_sync_lp();
    //SNVS_HPCR &= ~SNVS_HPCR_RTC_EN;
    //while ( SNVS_HPCR & SNVS_HPCR_RTC_EN );
    SNVS_LPGPR = SNVS_DEFAULT_PGD_VALUE;
    SNVS_LPSR = SNVS_LPSR_PGD_MASK | SNVS_LPSR_LPTA;
    
    if ( SRC_SRSR == 0 ) {
        return_rtc_irq = _VectorsRam[IRQ_SNVS_IRQ+16];
        attachInterruptVector( IRQ_SNVS_IRQ, &wakeup_isr );
        attachInterruptVector( IRQ_SNVS_IRQ, &wakeup_isr );
        NVIC_ENABLE_IRQ( IRQ_SNVS_IRQ );
    }
}

/*******************************************************************************
 *  <#Description#> Override of original setAlarm function. Works more like a
 *                  standard alarm. Is independent of when the driver is enabled.
 *                  (So long as it is enabled before the alarmTime)
 *
 *  @param alarmTime <#time_t number of specific date & time to set alarm#>
 *******************************************************************************/
void SnoozeAlarm::setAlarm( time_t alarmTime ){
    isUsed = true;
    alarm = alarmTime;
    timer_ = false;
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::disableDriver( uint8_t mode ) {
    if (mode == 0) return;
    if ( return_isr_enabled == 0 )  NVIC_DISABLE_IRQ( IRQ_SNVS_IRQ );
    // return priority
    NVIC_SET_PRIORITY( IRQ_SNVS_IRQ, return_priority );
    __disable_irq( );
    // return prev interrupt
    attachInterruptVector( IRQ_SNVS_IRQ, return_rtc_irq );
    __enable_irq( );
    
    if ( CCM_CCGR2_clock_active == false ) {
        CCM_CCGR2 &= ~CCM_CCGR2_IOMUXC_SNVS( CCM_CCGR_ON );
        CCM_CCGR2_clock_active = false;
    }
    SNVS_LPCR &= ~SNVS_LPCR_LPTA_EN;
    rtc_sync_lp( );
    //SNVS_HPCR |= SNVS_HPCR_RTC_EN;
    //while ( !(SNVS_HPCR & SNVS_HPCR_RTC_EN) );
    //setSyncProvider( rtc_set_sync_provider );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::enableDriver( uint8_t mode ) {
    if (mode == 0) return;
    return_isr_enabled = NVIC_IS_ENABLED( IRQ_SNVS_IRQ );
    if ( return_isr_enabled == 1 ) {
        NVIC_DISABLE_IRQ( IRQ_SNVS_IRQ );
    }
    // save prev isr
    return_rtc_irq = _VectorsRam[IRQ_SNVS_IRQ+16];
    __disable_irq( );
    attachInterruptVector( IRQ_SNVS_IRQ, &wakeup_isr );
    __enable_irq( );
    
    //get current priority
    return_priority = NVIC_GET_PRIORITY( IRQ_SNVS_IRQ );
    // get current priority
    int priority = nvic_execution_priority( );
    // if running from handler set priority higher than current handler
    priority = ( priority  < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
    //set priority to new level
    NVIC_SET_PRIORITY( IRQ_SNVS_IRQ, priority );
    if ( return_isr_enabled == 0 ) {
        NVIC_ENABLE_IRQ( IRQ_SNVS_IRQ );
    }
    
    if ( CCM_CCGR2 & CCM_CCGR2_IOMUXC_SNVS( CCM_CCGR_ON ) ) {
        CCM_CCGR2_clock_active = true;
    }
    else {
        CCM_CCGR2 &= ~CCM_CCGR2_IOMUXC_SNVS( CCM_CCGR_ON );
        CCM_CCGR2 |= CCM_CCGR2_IOMUXC_SNVS( CCM_CCGR_ON );
    }
    
    //SNVS_LPSR = 0xffffffff;
    
    /*SNVS_HPCR &= ~SNVS_HPCR_RTC_EN;
    while ( SNVS_HPCR & SNVS_HPCR_RTC_EN );*/
    
    SNVS_LPCR &= ~SNVS_LPCR_LPTA_EN;
    rtc_sync_lp( );
    
    // If setting timer style alarm
    if( timer_ ) SNVS_LPTAR = rtc_get( ) + ( alarm  );
    // else, setting true alarm
    else SNVS_LPTAR = rtc_get( ) + ( alarm );
    
    SNVS_LPCR |= SNVS_LPCR_LPTA_EN | SNVS_LPCR_LPWUI_EN;
    rtc_sync_lp( );
}

/*******************************************************************************
 *  <#Description#>
 *******************************************************************************/
void SnoozeAlarm::clearIsrFlags( uint32_t ipsr ) {
    if ( !( CCM_CCGR2 & CCM_CCGR2_IOMUXC_SNVS( CCM_CCGR_ON ) ) ) return;
    SNVS_LPSR |= 0x03;//0xffffffff;
    __asm__ volatile("ISB");
    __asm__ volatile("DSB");
}
#endif /* __IMXRT1062__ */
