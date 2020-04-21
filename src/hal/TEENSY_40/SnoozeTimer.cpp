/***********************************************************************************
 *  SnoozeTimer.h
 *  Teensy 4.0
 *
 * Purpose: Low Power Timer Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#include "SnoozeTimer.h"


#ifdef __cplusplus
extern "C" {
#endif
    extern void wakeup_isr( void );
    extern int nvic_execution_priority( void );
#ifdef __cplusplus
}
#endif

void ( * SnoozeTimer::return_gpt1_irq ) ( void );
volatile uint32_t SnoozeTimer::period;
/*******************************************************************************
 *  Set The Period for the Low Power Timer to wake the processor
 *
 *  @param newPeriod period in seconds
 *******************************************************************************/
void SnoozeTimer::setTimer( uint32_t newPeriod ) {
    isUsed = true;
    // 4096  -> .125
    // 8192  -> .250
    // 12288 -> .375
    // 16384 -> .500
    // 20480 -> .625
    // 24576 -> .750
    // 28672 -> .875
    // 32768 -> 1.000
    if ( newPeriod > 131071 ) newPeriod = 131071;
    period = newPeriod * 32768;
}

/*******************************************************************************
 *  Disables low power timer and returns registers and clock to previous state
 *******************************************************************************/
void SnoozeTimer::disableDriver( uint8_t mode ) {
    if ( mode == 0 ) return;
    //if ( mode <= 2 ) {
        if ( return_isr_enabled == 0 )  NVIC_DISABLE_IRQ( IRQ_GPT1 ); //disable irq
        NVIC_SET_PRIORITY( IRQ_GPT1, return_priority );// return priority
        __disable_irq( );
        attachInterruptVector( IRQ_GPT1, return_gpt1_irq );// return prev interrupt
        __enable_irq( );
    //}
    
    //IOMUXC_GPR_GPR5 = iomuxc_gpr_gpr5;
    GPT1_OCR1 = gpt1_ocr1;
    GPT1_CR   = gpt1_cr;
    GPT1_PR   = gpt1_pr;
    //XTALOSC24M_OSC_CONFIG0 = xtalosc24m_config0;
    //XTALOSC24M_OSC_CONFIG1 = xtalosc24m_config1;
    //XTALOSC24M_OSC_CONFIG2 = xtalosc24m_config2;
    
    if ( CCM_CCGR1_clock_active == false ) {
        CCM_CCGR1_clock_active = true;
        CCM_CCGR1 |= CCM_CCGR1_GPT1_SERIAL( CCM_CCGR_OFF );
    }
}

/*******************************************************************************
 *  Enables low power timer and saves regiater and clock state
 *******************************************************************************/
void SnoozeTimer::enableDriver( uint8_t mode ) {
    if ( mode == 0 ) return;
    //if ( mode == 1 ) {
        return_priority = NVIC_GET_PRIORITY( IRQ_GPT1 );//get current priority
        int priority = nvic_execution_priority( );// get current priority
        // if running from handler mode set priority higher than current handler
        priority = ( priority < 256 ) && ( ( priority - 16 ) > 0 ) ? priority - 16 : 128;
        NVIC_SET_PRIORITY( IRQ_GPT1, priority );//set priority to new level
        __disable_irq( );
        return_gpt1_irq = _VectorsRam[IRQ_GPT1+16];// save prev isr
        attachInterruptVector( IRQ_GPT1, &wakeup_isr );
        __enable_irq( );
    //}
    
    if ( CCM_CCGR1 & CCM_CCGR1_GPT1_SERIAL( CCM_CCGR_ON ) ) CCM_CCGR1_clock_active = true;
    else CCM_CCGR1 |= CCM_CCGR1_GPT1_SERIAL( CCM_CCGR_ON );
    
    //iomuxc_gpr_gpr5 = IOMUXC_GPR_GPR5;
    gpt1_ocr1       = GPT1_OCR1;
    gpt1_cr         = GPT1_CR;
    gpt1_pr         = GPT1_PR;
    //xtalosc24m_config0 = XTALOSC24M_OSC_CONFIG0;
    //xtalosc24m_config1 = XTALOSC24M_OSC_CONFIG1;
    //xtalosc24m_config2 = XTALOSC24M_OSC_CONFIG2;
    
    //IOMUXC_GPR_GPR5 = ( 1 << 28 );
    GPT1_CR |= ( 1 << 15 );
    while (GPT1_CR & ( 1 << 15 ) );
    GPT1_OCR1 = period;
    // use 32k clock since 1Mhz does not work in hibernate mode:)
    GPT1_CR   = GPT_CR_CLKSRC( 0x04 ) | GPT_CR_ENMOD | GPT_CR_WAITEN | GPT_CR_DOZEEN | GPT_CR_STOPEN;
    GPT1_PR   = GPT_PR_PRESCALER( 0 );
    GPT1_IR   = 0x3F;
    /*XTALOSC24M_OSC_CONFIG0 = 0x4000A73;
     XTALOSC24M_OSC_CONFIG1 = 0x40002dc;
     
     uint32_t reg = XTALOSC24M_OSC_CONFIG0;
     reg &= ~( ( 0xF << 16 ) | ( 0xF << 12 ) );
     reg |= ( ( 0x3 << 16 ) | ( 0x3 << 12 ) );
     XTALOSC24M_OSC_CONFIG0 = reg;
     
     reg = XTALOSC24M_OSC_CONFIG2;
     reg &= ~( 0xFFF << 0 | 0x1 << 31 );
     reg |= ( 0x2D7 << 0 );
     XTALOSC24M_OSC_CONFIG2 = reg;
     
     reg = XTALOSC24M_OSC_CONFIG0;*/
    
    return_isr_enabled = NVIC_IS_ENABLED( IRQ_GPT1 );
    if ( return_isr_enabled == 0 ) NVIC_ENABLE_IRQ( IRQ_GPT1 );
    GPT1_CR |= GPT_CR_EN;
}

/*******************************************************************************
 *  clears low power timer flags called from sleep llwu wakeup
 *******************************************************************************/
void SnoozeTimer::clearIsrFlags( uint32_t ipsr ) {
    GPT1_SR = 0x3F;
    systick_millis_count += period / 1000;
}
#endif /* __IMXRT1062__ */
