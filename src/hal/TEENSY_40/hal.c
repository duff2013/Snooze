#if defined(__IMXRT1062__)

#include "Arduino.h"
#include "common.h"
#include "hal.h"

#define COMPARE_WAKE    34
#define ALARM_WAKE      35
#define TIMER_WAKE      36
#define TOUCH_WAKE      37

#define CCM_ANALOG_PFD_480_PFD2_CLKGATE    ((uint32_t)(1 << 23))
#define CCM_ANALOG_PFD_528_PFD2_CLKGATE    ((uint32_t)(1 << 23))
#define XTALOSC24M_LOWPWR_CTRL_SET_OSC_SEL ((uint32_t)(0x10U))
//#define CCM_ANALOG_MISC0_XTAL_24M_PWD      ((uint32_t)(0x40000000U))
#define XTALOSC24M_LOWPWR_CTRL_LPBG_SEL     (0x20U)

#define CCM_ANALOG_PFD_528_PFD0_STABLE ((uint32_t)(1<<6))
#define CCM_ANALOG_PFD_528_PFD2_STABLE ((uint32_t)(1<<22))
#define XTALOSC24M_LOWPWR_CTRL_XTALOSC_PWRUP_STAT ((uint32_t)(1<<16))

#define XTALOSC24M_LOWPWR_CTRL_SET_OSC_SEL_MASK  (0x10U)
#define CCM_ANALOG_MISC0_XTAL_24M_PWD_MASK       (0x40000000U)

#define DCDC_REG2_LOOPCTRL_EN_RCSCALE1(n)        ((uint32_t)(((n) & 0x07) << 9))

// SCB D-Cache Clean by Set-way Register Definitions
#define SCB_DCCSW_SET_POS   5
#define SCB_DCCSW_SET_MASK  (0x1FF << SCB_DCCSW_SET_POS)

#define SCB_DCCSW_WAY_POS   30
#define SCB_DCCSW_WAY_Msk   (3 << SCB_DCCSW_WAY_POS)

// Cache Size ID Register Macros
#define CCSIDR_WAYS(x)  (((x) & SCB_CCSIDR_ASSOCIATIVITY_MASK) >> SCB_CCSIDR_ASSOCIATIVITY_POS)
#define CCSIDR_SETS(x)  (((x) & SCB_CCSIDR_NUMSETS_MASK) >> SCB_CCSIDR_NUMSETS_POS      )

#define SCB_CCSIDR_ASSOCIATIVITY_POS    3
#define SCB_CCSIDR_ASSOCIATIVITY_MASK   (0x3FF << SCB_CCSIDR_ASSOCIATIVITY_POS)

#define SCB_CCSIDR_NUMSETS_POS          13
#define SCB_CCSIDR_NUMSETS_MASK         (0x7FFF << SCB_CCSIDR_NUMSETS_POS)

#define SCB_DCCISW_SET_POS              5
#define SCB_DCCISW_SET_MASK             (0x1FF << SCB_DCCISW_SET_POS)

#define SCB_DCCISW_WAY_POS              30
#define SCB_DCCISW_WAY_MASK              (3 << SCB_DCCISW_WAY_POS)

#define SCB_CCR_DC_POS                  16
#define SCB_CCR_DC_MASK                 (1 << SCB_CCR_DC_POS)

uint32_t pll_sys;
uint32_t pll_usb1;
uint32_t pll_usb2;
uint32_t pll_audio;
uint32_t pll_video;
uint32_t pll_enet;
uint32_t pfd_480;
uint32_t pfd_528;
uint32_t usbphy1;
uint32_t usbphy2;

extern volatile uint32_t usb_cdc_line_rtsdtr_millis;
extern volatile uint8_t  usb_cdc_line_rtsdtr;
extern volatile uint32_t set_arm_clock(uint32_t frequency);

FASTRUN uint32_t set_osc_clock(uint32_t frequency);

void start_up( );

volatile DMAMEM int wake_source = -1;

void ( * clear_flags )( uint32_t );

/*
// Context Restore
extern unsigned long _estack;
// R4, R5, R6, R7, R8, R9, R10, R11, R12, LR
#define CORE_SAVE_COMMON_REGS_NUM  12
// PRIMEMASK, CONTROL, MSP, PSP
#define CORE_SAVE_SPECIAL_REGS_NUM 4
DMAMEM uint32_t s_common_regs_context_buf[CORE_SAVE_COMMON_REGS_NUM+2] = { 0 };
DMAMEM uint32_t s_special_regs_context_buf[CORE_SAVE_SPECIAL_REGS_NUM+2] = { 0 };
DMAMEM uint32_t *gpComRegsContextStackTop = s_common_regs_context_buf + CORE_SAVE_COMMON_REGS_NUM;
DMAMEM uint32_t *gpComRegsContextStackBottom = s_common_regs_context_buf;
DMAMEM uint32_t *gpSpecRegsContextStackTop = s_special_regs_context_buf + CORE_SAVE_SPECIAL_REGS_NUM;
DMAMEM uint32_t *gpSpecRegsContextStackBottom = s_special_regs_context_buf;
 */
//----------------------------------------------------------------------------------
void SCB_CleanDCache (void) {
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;
    
    SCB_ID_CSSELR = 0U; // Level 1 data cache
    __asm volatile ( "dsb \n" );
    
    ccsidr = SCB_ID_CCSIDR;
    
    // clean D-Cache
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
        ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
        do {
            SCB_CACHE_DCCSW = (((sets << SCB_DCCSW_SET_POS) & SCB_DCCSW_SET_MASK) |
                               ((ways << SCB_DCCSW_WAY_POS) & SCB_DCCSW_WAY_Msk)  );
        } while (ways-- != 0U);
    } while(sets-- != 0U);
    __asm volatile ( "dsb \n" );
    __asm volatile ( "isb \n" );
}
//----------------------------------------------------------------------------------
void SCB_DisableDCache (void) {
    register uint32_t ccsidr;
    register uint32_t sets;
    register uint32_t ways;
    
    SCB_ID_CSSELR = 0U; // Level 1 data cache
    __asm volatile ( "dsb \n" );
    
    SCB_CCR &= ~(uint32_t)SCB_CCR_DC_MASK;  // disable D-Cache
    __asm volatile ( "dsb \n" );
    
    ccsidr = SCB_ID_CCSIDR;
    
    // clean & invalidate D-Cache
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
        ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
        do {
            SCB_CACHE_DCCISW = (((sets << SCB_DCCISW_SET_POS) & SCB_DCCISW_SET_MASK) |
                                ((ways << SCB_DCCISW_WAY_POS) & SCB_DCCISW_WAY_MASK)  );
        } while (ways-- != 0U);
    } while(sets-- != 0U);
    __asm volatile ( "dsb \n" );
    __asm volatile ( "isb \n" );
}
//---------------------------------------------------------------------------------
int nvic_execution_priority( void ) {
    uint32_t priority=256;
    uint32_t primask, faultmask, basepri, ipsr;
    
    // full algorithm in ARM DDI0403D, page B1-639
    // this isn't quite complete, but hopefully good enough
    __asm__ volatile( "mrs %0, faultmask\n" : "=r" ( faultmask ):: );
    if ( faultmask ) return -1;
    __asm__ volatile( "mrs %0, primask\n" : "=r" ( primask ):: );
    if ( primask ) return 0;
    __asm__ volatile( "mrs %0, ipsr\n" : "=r" ( ipsr ):: );
    if ( ipsr) {
        if ( ipsr < 16 ) priority = 0; // could be non-zero
        else priority = NVIC_GET_PRIORITY( ipsr - 16 );
    }
    __asm__ volatile( "mrs %0, basepri\n" : "=r" ( basepri ):: );
    if ( basepri > 0 && basepri < priority ) priority = basepri;
    return priority;
}
//---------------------------------------------------------------------------------
FASTRUN uint32_t set_clock_osc( uint32_t frequency ) {
    if ( 24000000 % frequency || frequency > 24000000 ) return 0;
    uint32_t cbcdr = CCM_CBCDR;
    uint32_t cbcmr = CCM_CBCMR;
    uint32_t dcdc = DCDC_REG3;
    
    uint32_t div_clk2 = 1;
    uint32_t div_ahb = 1;
    uint32_t freq_div = 24000000/frequency;
    for ( int i = 1; i < 9; i++ ) {
        div_ahb = freq_div / i;
        div_clk2 = i;
        if ( div_ahb <= 8 ) break;
    }
    
    cbcdr &= ~CCM_CBCDR_PERIPH_CLK2_PODF_MASK;
    cbcdr |= CCM_CBCDR_PERIPH_CLK2_PODF( div_clk2 - 1 );
    CCM_CBCDR = cbcdr;
    
    cbcmr &= ~CCM_CBCMR_PERIPH_CLK2_SEL_MASK;
    cbcmr |= CCM_CBCMR_PERIPH_CLK2_SEL( 1 );
    CCM_CBCMR = cbcmr;
    while ( CCM_CDHIPR & CCM_CDHIPR_PERIPH2_CLK_SEL_BUSY );
    
    cbcdr &= ~CCM_CBCDR_AHB_PODF_MASK;
    cbcdr |= CCM_CBCDR_AHB_PODF( div_ahb - 1 );
    CCM_CBCDR = cbcdr;
    while ( CCM_CDHIPR & CCM_CDHIPR_AHB_PODF_BUSY );
    
    cbcdr |= CCM_CBCDR_PERIPH_CLK_SEL;
    CCM_CBCDR = cbcdr;
    while ( CCM_CDHIPR & CCM_CDHIPR_PERIPH_CLK_SEL_BUSY );
    
    cbcdr &= ~CCM_CBCDR_IPG_PODF_MASK;
    cbcdr |= CCM_CBCDR_IPG_PODF( 0 );
    
    CCM_CBCDR = cbcdr;
    
    CCM_ANALOG_PLL_ARM_SET = CCM_ANALOG_PLL_ARM_BYPASS;
    CCM_ANALOG_PLL_ARM_SET = CCM_ANALOG_PLL_ARM_POWERDOWN;
    
    F_CPU_ACTUAL = frequency;
    F_BUS_ACTUAL = frequency;
    scale_cpu_cycles_to_microseconds = 0xFFFFFFFFu / ( uint32_t )( frequency / 1000000u );
    
    dcdc &= ~DCDC_REG3_TRG_MASK;
    dcdc |= DCDC_REG3_TRG( 6 );
    DCDC_REG3 = dcdc;
    while ( !( DCDC_REG0 & DCDC_REG0_STS_DC_OK ) );
    
    return frequency;
}
//----------------------------------------------------------------------------------
FASTRUN void set_clock_rc_osc( void ) {
    // Switch to RC-OSC
    XTALOSC24M_LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_SET_OSC_SEL;
    // Turn off XTAL-OSC
    CCM_ANALOG_MISC0_SET = CCM_ANALOG_MISC0_XTAL_24M_PWD;
    while ( CCM_CDHIPR != 0 );
    for ( uint32_t i = 0; i < 5 * 50; i++ ) asm volatile( "nop \n" );
}
//----------------------------------------------------------------------------------
FASTRUN void power_down_subsytem( void ) {
    // Power down
    CCM_ANALOG_MISC0_SET = CCM_ANALOG_MISC0_XTAL_24M_PWD;
    // Lower OSC current by 37.5%
    CCM_ANALOG_MISC0_SET = 0x6000;//CCM_ANALOG_MISC0_OSC_I( 3 );
    // Enable FET ODRIVE
    PMU_REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE;
    // Disconnect vdd_high_in and connect vdd_snvs_in
    CCM_ANALOG_MISC0_SET = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS;
    // Switch bandgap
    // TODO: See about STOP_MODE_CONFIG Bit in PMU_MIS0
    PMU_MISC0_SET = 0x00000004;
    XTALOSC24M_LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL;
    PMU_MISC0_SET = CCM_ANALOG_MISC0_REFTOP_PWD;
    while ( CCM_CDHIPR != 0 );
    for ( uint32_t i = 0; i < 5 * 50; i++ ) asm volatile( "nop \n" );
    CCM_CCR = ( CCM_CCR_COSC_EN | CCM_CCR_OSCNT( 0xAF ) );
    GPC_CNTR |= GPC_CNTR_PDRAM0_PGE;
}
//----------------------------------------------------------------------------------
FASTRUN void set_clock_xtal_osc( void ) {
    // Turn on XTAL-OSC
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_XTAL_24M_PWD;
    while ((XTALOSC24M_LOWPWR_CTRL & XTALOSC24M_LOWPWR_CTRL_XTALOSC_PWRUP_STAT) == 0);
    // Detect freq
    CCM_ANALOG_MISC0_SET = CCM_ANALOG_MISC0_OSC_XTALOK_EN;
    while ((CCM_ANALOG_MISC0 & CCM_ANALOG_MISC0_OSC_XTALOK) == 0);
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_OSC_XTALOK_EN;
    // Switch to XTAL-OSC
    XTALOSC24M_LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_SET_OSC_SEL;
    // Turn off XTAL-OSC detector
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_OSC_XTALOK_EN;
    while ( CCM_CDHIPR != 0 );
    for ( uint32_t i = 0; i < 5 * 40; i++ ) asm volatile( "nop \n" );
    // Restore bandgap
    // Turn on regular bandgap and wait for stable
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_REFTOP_PWD;
    while ((CCM_ANALOG_MISC0 & CCM_ANALOG_MISC0_REFTOP_VBGUP) == 0);
    // Low power band gap disable
    XTALOSC24M_LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL;
    PMU_MISC0_CLR = 0x00000004;
    while ( CCM_CDHIPR != 0 );
    for ( uint32_t i = 0; i < 5 * 40; i++ ) asm volatile( "nop \n" );
}
//----------------------------------------------------------------------------------
FASTRUN void set_clock_lp_dcdc( void ) {
    uint32_t dcdc = DCDC_REG0 &
    ~(DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |
      DCDC_REG0_PWD_OSC_INT);
    
    dcdc |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH;
    DCDC_REG0 = dcdc;
}
//---------------------------------------------------------------------------------
FASTRUN void start_usb_pll( void ) {
    while ( 1 ) {
        uint32_t n = CCM_ANALOG_PLL_USB1;
        if ( n & CCM_ANALOG_PLL_USB1_DIV_SELECT ) {
            // bypass 24 MHz
            CCM_ANALOG_PLL_USB1_CLR = 0xC000;
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS; // bypass
            CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER | // power down
            CCM_ANALOG_PLL_USB1_DIV_SELECT |                      // use 480 MHz
            CCM_ANALOG_PLL_USB1_ENABLE |                          // disable
            CCM_ANALOG_PLL_USB1_EN_USB_CLKS;                      // disable usb
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_ENABLE ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_ENABLE;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_POWER ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_POWER;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_LOCK ) ) {
            
            continue;
        }
        if ( n & CCM_ANALOG_PLL_USB1_BYPASS ) {
            CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_BYPASS;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_EN_USB_CLKS ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_EN_USB_CLKS;
            continue;
        }
        break;
    }
}
//---------------------------------------------------------------------------------
FASTRUN void disable_plls( void ) {
    pll_sys   = CCM_ANALOG_PLL_SYS;
    pll_usb1  = CCM_ANALOG_PLL_USB1;
    pll_usb2  = CCM_ANALOG_PLL_USB2;
    pll_audio = CCM_ANALOG_PLL_AUDIO;
    pll_video = CCM_ANALOG_PLL_VIDEO;
    pll_enet  = CCM_ANALOG_PLL_ENET;
    pfd_480   = CCM_ANALOG_PFD_480;
    pfd_528   = CCM_ANALOG_PFD_528;
    usbphy1   = USBPHY1_CTRL;
    usbphy2   = USBPHY2_CTRL;
    
    if ( !( pll_sys & CCM_ANALOG_PLL_SYS_POWERDOWN ) ) {
        CCM_ANALOG_PLL_SYS_SET = CCM_ANALOG_PLL_SYS_BYPASS;
        CCM_ANALOG_PLL_SYS_SET = CCM_ANALOG_PLL_SYS_POWERDOWN;
        CCM_ANALOG_PFD_528 |= 0x80808080;
        //CCM_ANALOG_PFD_528_SET = CCM_ANALOG_PFD_528_PFD2_CLKGATE;
    }
    
    //CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS;
    //CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER;
    //CCM_ANALOG_PFD_480_SET = CCM_ANALOG_PFD_480_PFD2_CLKGATE;
    
    /*if ( pll_usb1 & CCM_ANALOG_PLL_USB1_POWER ) {
     USBPHY1_PWD  = 0xFFFFFFFF;
     USBPHY2_PWD  = 0xFFFFFFFF;
     NVIC_DISABLE_IRQ( IRQ_USB1 );
     CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS;
     CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_EN_USB_CLKS | // disable usb
     CCM_ANALOG_PLL_USB1_ENABLE |      // disable
     CCM_ANALOG_PLL_USB1_DIV_SELECT |  // use 480 MHz
     CCM_ANALOG_PLL_USB1_POWER;        // power down
     CCM_ANALOG_PFD_480 |= 0x80808080;
     //CCM_ANALOG_PFD_480_SET  = CCM_ANALOG_PFD_480_PFD2_CLKGATE;
     //
     }*/
    
    if ( pll_usb2 & CCM_ANALOG_PLL_USB2_POWER ) {
        CCM_ANALOG_PLL_USB2_SET = CCM_ANALOG_PLL_USB2_BYPASS;
        CCM_ANALOG_PLL_USB2_CLR = CCM_ANALOG_PLL_USB2_POWER;
    }
    
    if ( ! ( pll_audio & CCM_ANALOG_PLL_AUDIO_POWERDOWN ) ) {
        CCM_ANALOG_PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_BYPASS;
        CCM_ANALOG_PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_POWERDOWN;
    }
    
    if ( ! ( pll_video & CCM_ANALOG_PLL_VIDEO_POWERDOWN ) ) {
        CCM_ANALOG_PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_BYPASS;
        CCM_ANALOG_PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_POWERDOWN;
    }
    
    if ( ! ( pll_enet & CCM_ANALOG_PLL_ENET_POWERDOWN ) ) {
        CCM_ANALOG_PLL_ENET_SET = CCM_ANALOG_PLL_ENET_BYPASS;
        CCM_ANALOG_PLL_ENET_SET = CCM_ANALOG_PLL_ENET_POWERDOWN;
    }
}

FASTRUN void enable_plls( void ) {
    //CCM_ANALOG_PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN;
    //CCM_ANALOG_PLL_ARM_SET = CCM_ANALOG_PLL_ARM_ENABLE | CCM_ANALOG_PLL_ARM_BYPASS;
    
    if ( !( pll_sys & CCM_ANALOG_PLL_SYS_POWERDOWN ) ) {
        CCM_ANALOG_PLL_SYS_SET = CCM_ANALOG_PLL_SYS_BYPASS;
        CCM_ANALOG_PLL_SYS_CLR = CCM_ANALOG_PLL_SYS_POWERDOWN;
        CCM_ANALOG_PLL_SYS = pll_sys;
        while ( ( CCM_ANALOG_PLL_SYS & CCM_ANALOG_PLL_SYS_LOCK ) == 0 );
        CCM_ANALOG_PFD_528 = pfd_528;
    }
    
    if ( pll_usb2 & CCM_ANALOG_PLL_USB2_POWER ) {
        CCM_ANALOG_PLL_USB2_SET = CCM_ANALOG_PLL_USB2_BYPASS;
        CCM_ANALOG_PLL_USB2_SET = CCM_ANALOG_PLL_USB2_POWER;
        CCM_ANALOG_PLL_USB2  = pll_usb2;
        while ( ( CCM_ANALOG_PLL_USB2 & CCM_ANALOG_PLL_USB2_LOCK ) == 0 );
        CCM_ANALOG_PFD_480 = pfd_480;
    }
    
    if ( !( pll_audio & CCM_ANALOG_PLL_AUDIO_POWERDOWN ) ) {
        CCM_ANALOG_PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_BYPASS;
        CCM_ANALOG_PLL_AUDIO_CLR = CCM_ANALOG_PLL_AUDIO_POWERDOWN;
        CCM_ANALOG_PLL_AUDIO = pll_audio;
        while ( ( CCM_ANALOG_PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_LOCK ) == 0 );
    }
    
    if ( !( pll_video & CCM_ANALOG_PLL_VIDEO_POWERDOWN ) ) {
        CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_BYPASS;
        CCM_ANALOG_PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_POWERDOWN;
        CCM_ANALOG_PLL_VIDEO = pll_video;
        while ( ( CCM_ANALOG_PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_LOCK ) == 0 );
    }
    
    if ( !( pll_enet & CCM_ANALOG_PLL_ENET_POWERDOWN ) ) {
        CCM_ANALOG_PLL_ENET_SET = CCM_ANALOG_PLL_ENET_BYPASS;
        CCM_ANALOG_PLL_ENET_SET = CCM_ANALOG_PLL_ENET_POWERDOWN;
        CCM_ANALOG_PLL_ENET = pll_enet;
        while ( ( CCM_ANALOG_PLL_ENET & CCM_ANALOG_PLL_ENET_LOCK ) == 0 );
    }
    
    /*if ( pll_usb1 & CCM_ANALOG_PLL_USB1_POWER ) {
     USBPHY1_CTRL_CLR = USBPHY_CTRL_CLKGATE;
     USBPHY1_PWD_CLR = 0xFFFFFFFF;
     USBPHY2_PWD_CLR = 0xFFFFFFFF;
     NVIC_ENABLE_IRQ( IRQ_USB1 );
     //usb_pll();
     }*/
}
//----------------------------------------------------------------------------------
void wakeup_isr( void ) {
    int source = -1;
    uint32_t ipsr;
    __asm__ volatile( "mrs %0, ipsr\n" : "=r" ( ipsr ):: );
    switch ( ipsr - 16 ) {
        case IRQ_SNVS_ONOFF: {
            source = ALARM_WAKE;
            break;
        }
        case IRQ_SNVS_SECURITY: {
            source = ALARM_WAKE;
            break;
        }
        case IRQ_SNVS_IRQ: {
            source = ALARM_WAKE;
            break;
        }
        case IRQ_ACMP1: {
            source = COMPARE_WAKE;
            break;
        }
        case IRQ_ACMP2: {
            source = COMPARE_WAKE;
            break;
        }
        case IRQ_ACMP3: {
            source = COMPARE_WAKE;
            break;
        }
        case IRQ_ACMP4: {
            source = COMPARE_WAKE;
            break;
        }
        case IRQ_GPT1: {
            source = TIMER_WAKE;
            break;
        }
        case IRQ_GPIO1_0_15:
        case IRQ_GPIO1_16_31:
        case IRQ_GPIO2_0_15:
        case IRQ_GPIO2_16_31:
        case IRQ_GPIO3_0_15:
        case IRQ_GPIO3_16_31:
        case IRQ_GPIO4_0_15:
        case IRQ_GPIO4_16_31: {
            uint32_t isfr_1 = GPIO1_ISR & GPIO1_IMR;
            uint32_t isfr_2 = GPIO2_ISR & GPIO2_IMR;
            uint32_t isfr_3 = GPIO3_ISR & GPIO3_IMR;
            uint32_t isfr_4 = GPIO4_ISR & GPIO4_IMR;
            
            if ( isfr_1 & CORE_PIN0_BITMASK ) source = 0;
            else if ( isfr_1 & CORE_PIN1_BITMASK ) source = 1;
            else if ( isfr_1 & CORE_PIN14_BITMASK ) source = 14;
            else if ( isfr_1 & CORE_PIN15_BITMASK ) source = 15;
            else if ( isfr_1 & CORE_PIN16_BITMASK ) source = 16;
            else if ( isfr_1 & CORE_PIN17_BITMASK ) source = 17;
            else if ( isfr_1 & CORE_PIN18_BITMASK ) source = 18;
            else if ( isfr_1 & CORE_PIN19_BITMASK ) source = 19;
            else if ( isfr_1 & CORE_PIN20_BITMASK ) source = 20;
            else if ( isfr_1 & CORE_PIN21_BITMASK ) source = 21;
            else if ( isfr_1 & CORE_PIN22_BITMASK ) source = 22;
            else if ( isfr_1 & CORE_PIN23_BITMASK ) source = 23;
            else if ( isfr_1 & CORE_PIN24_BITMASK ) source = 24;
            else if ( isfr_1 & CORE_PIN25_BITMASK ) source = 25;
            else if ( isfr_1 & CORE_PIN26_BITMASK ) source = 26;
            else if ( isfr_1 & CORE_PIN27_BITMASK ) source = 27;
            else if ( isfr_1 & CORE_PIN38_BITMASK ) source = 38;
            else if ( isfr_1 & CORE_PIN39_BITMASK ) source = 39;
            //else if ( isfr_6 & CORE_PIN40_BITMASK ) source = 40;
            //else if ( isfr_6 & CORE_PIN41_BITMASK ) source = 41;
            else if ( isfr_2 & CORE_PIN6_BITMASK ) source = 6;
            else if ( isfr_2 & CORE_PIN7_BITMASK ) source = 7;
            else if ( isfr_2 & CORE_PIN8_BITMASK ) source = 8;
            else if ( isfr_2 & CORE_PIN9_BITMASK ) source = 9;
            else if ( isfr_2 & CORE_PIN10_BITMASK ) source = 10;
            else if ( isfr_2 & CORE_PIN11_BITMASK ) source = 11;
            else if ( isfr_2 & CORE_PIN12_BITMASK ) source = 12;
            else if ( isfr_2 & CORE_PIN13_BITMASK ) source = 13;
            else if ( isfr_2 & CORE_PIN32_BITMASK ) source = 32;
            else if ( isfr_2 & CORE_PIN33_BITMASK ) source = 33;
            else if ( isfr_2 & CORE_PIN34_BITMASK ) source = 34;
            else if ( isfr_2 & CORE_PIN35_BITMASK ) source = 35;
            else if ( isfr_2 & CORE_PIN36_BITMASK ) source = 36;
            else if ( isfr_2 & CORE_PIN37_BITMASK ) source = 37;
            else if ( isfr_3 & CORE_PIN28_BITMASK ) source = 28;
            else if ( isfr_3 & CORE_PIN30_BITMASK ) source = 30;
            else if ( isfr_3 & CORE_PIN31_BITMASK ) source = 31;
            //else if ( isfr_3 & CORE_PIN42_BITMASK ) source = 42;
            //else if ( isfr_3 & CORE_PIN43_BITMASK ) source = 43;
            //else if ( isfr_3 & CORE_PIN44_BITMASK ) source = 44;
            //else if ( isfr_3 & CORE_PIN45_BITMASK ) source = 45;
            //else if ( isfr_3 & CORE_PIN46_BITMASK ) source = 46;
            else if ( isfr_4 & CORE_PIN2_BITMASK ) source = 2;
            else if ( isfr_4 & CORE_PIN3_BITMASK ) source = 3;
            else if ( isfr_4 & CORE_PIN4_BITMASK ) source = 4;
            else if ( isfr_4 & CORE_PIN5_BITMASK ) source = 5;
            else if ( isfr_4 & CORE_PIN29_BITMASK ) source = 29;
            else if ( isfr_4 & CORE_PIN33_BITMASK ) source = 33;
            break;
        }
            
        case IRQ_GPIO6789: {
            uint32_t isfr_6 = GPIO6_ISR & GPIO6_IMR;
            uint32_t isfr_7 = GPIO7_ISR & GPIO7_IMR;
            uint32_t isfr_8 = GPIO8_ISR & GPIO8_IMR;
            uint32_t isfr_9 = GPIO9_ISR & GPIO9_IMR;
            
            if ( isfr_6 & CORE_PIN0_BITMASK ) source = 0;
            else if ( isfr_6 & CORE_PIN1_BITMASK ) source = 1;
            else if ( isfr_6 & CORE_PIN14_BITMASK ) source = 14;
            else if ( isfr_6 & CORE_PIN15_BITMASK ) source = 15;
            else if ( isfr_6 & CORE_PIN16_BITMASK ) source = 16;
            else if ( isfr_6 & CORE_PIN17_BITMASK ) source = 17;
            else if ( isfr_6 & CORE_PIN18_BITMASK ) source = 18;
            else if ( isfr_6 & CORE_PIN19_BITMASK ) source = 19;
            else if ( isfr_6 & CORE_PIN20_BITMASK ) source = 20;
            else if ( isfr_6 & CORE_PIN21_BITMASK ) source = 21;
            else if ( isfr_6 & CORE_PIN22_BITMASK ) source = 22;
            else if ( isfr_6 & CORE_PIN23_BITMASK ) source = 23;
            else if ( isfr_6 & CORE_PIN24_BITMASK ) source = 24;
            else if ( isfr_6 & CORE_PIN25_BITMASK ) source = 25;
            else if ( isfr_6 & CORE_PIN26_BITMASK ) source = 26;
            else if ( isfr_6 & CORE_PIN27_BITMASK ) source = 27;
            else if ( isfr_6 & CORE_PIN38_BITMASK ) source = 38;
            else if ( isfr_6 & CORE_PIN39_BITMASK ) source = 39;
            //else if ( isfr_6 & CORE_PIN40_BITMASK ) source = 40;
            //else if ( isfr_6 & CORE_PIN41_BITMASK ) source = 41;
            else if ( isfr_7 & CORE_PIN6_BITMASK ) source = 6;
            else if ( isfr_7 & CORE_PIN7_BITMASK ) source = 7;
            else if ( isfr_7 & CORE_PIN8_BITMASK ) source = 8;
            else if ( isfr_7 & CORE_PIN9_BITMASK ) source = 9;
            else if ( isfr_7 & CORE_PIN10_BITMASK ) source = 10;
            else if ( isfr_7 & CORE_PIN11_BITMASK ) source = 11;
            else if ( isfr_7 & CORE_PIN12_BITMASK ) source = 12;
            else if ( isfr_7 & CORE_PIN13_BITMASK ) source = 13;
            else if ( isfr_7 & CORE_PIN32_BITMASK ) source = 32;
            else if ( isfr_7 & CORE_PIN33_BITMASK ) source = 33;
            else if ( isfr_7 & CORE_PIN34_BITMASK ) source = 34;
            else if ( isfr_7 & CORE_PIN35_BITMASK ) source = 35;
            else if ( isfr_7 & CORE_PIN36_BITMASK ) source = 36;
            else if ( isfr_7 & CORE_PIN37_BITMASK ) source = 37;
            else if ( isfr_8 & CORE_PIN28_BITMASK ) source = 28;
            else if ( isfr_8 & CORE_PIN30_BITMASK ) source = 30;
            else if ( isfr_8 & CORE_PIN31_BITMASK ) source = 31;
            //else if ( isfr_8 & CORE_PIN42_BITMASK ) source = 42;
            //else if ( isfr_8 & CORE_PIN43_BITMASK ) source = 43;
            //else if ( isfr_8 & CORE_PIN44_BITMASK ) source = 44;
            //else if ( isfr_8 & CORE_PIN45_BITMASK ) source = 45;
            //else if ( isfr_8 & CORE_PIN46_BITMASK ) source = 46;
            else if ( isfr_9 & CORE_PIN2_BITMASK ) source = 2;
            else if ( isfr_9 & CORE_PIN3_BITMASK ) source = 3;
            else if ( isfr_9 & CORE_PIN4_BITMASK ) source = 4;
            else if ( isfr_9 & CORE_PIN5_BITMASK ) source = 5;
            else if ( isfr_9 & CORE_PIN29_BITMASK ) source = 29;
            else if ( isfr_9 & CORE_PIN33_BITMASK ) source = 33;
            break;
        }
        default:
            source = -1;
            break;
    }
    clear_flags( ipsr );
    wake_source = source;
}
//----------------------------------------------------------------------------------
//void save_context( uint32_t *common, uint32_t *special );
//void save_context( uint32_t *common, uint32_t *special ) {
void save_context( void ) {
    //volatile uint32_t *common  = &s_common_regs_context_buf;
    //volatile uint32_t *special = &s_special_regs_context_buf;
    /*__asm__ volatile (
     "mov r0, %[com] \n"
     "stmia r0,{r4-r12, lr}"
     :  [com] "+r" ( common ) // outputs
     :  // inputs
     :  // clobbers
     );
     
     __asm__ volatile (
     "mov r1, %[spec] \n"
     "mrs r2, msp \n"
     "mrs r3, psp \n"
     "mrs r4, primask \n"
     "mrs r5, control \n"
     "stmia r1,{r2-r5}"
     :  [spec] "+r" ( special ) // outputs
     :  // inputs
     :  "r2", "r3", "r4", "r5"// clobbers
     );*/
}
//----------------------------------------------------------------------------------
void load_context( void ) {
    //volatile uint32_t *common  = &s_common_regs_context_buf;
    //volatile uint32_t *special = &s_special_regs_context_buf;
    /*__asm__ volatile (
     "mov r0, %[com] \n"
     "ldmia r0,{r4-r12, lr}"
     :  // outputs
     :  [com] "r" ( common ) // inputs
     :  // clobbers
     );
     
     __asm__ volatile (
     "mov   r1, %[spec] \n"
     "ldmia r1,{r2-r5} \n"
     "msr   msp, r2 \n"
     "msr   psp, r3 \n"
     "msr   primask, r4 \n"
     "msr   control, r5 \n"
     :  // outputs
     :  [spec] "r" ( special ) // inputs
     :  "r2", "r3", "r4", "r5"// clobbers
     );*/
}
//----------------------------------------------------------------------------------
void start_up( void ) {
    /*__asm__ volatile( "LDR     R1, =gpSpecRegsContextStackBottom" );
     __asm__ volatile( "LDR     R0, [R1]" );
     __asm__ volatile( "LDMIA   R0!, {R2-R5}" );
     __asm__ volatile( "MSR     MSP, R2" );
     __asm__ volatile( "MSR     PSP, R3" );
     __asm__ volatile( "MSR     PRIMASK, R4" );
     __asm__ volatile( "MSR     CONTROL, R5" );
     __asm__ volatile( "LDR     R1, =gpComRegsContextStackBottom" );
     __asm__ volatile( "LDR R0, [R1]" );
     __asm__ volatile( "LDMIA   R0!, {R4-R12, LR}" );
     __asm__ volatile( "MOV R0, LR" );
     __asm__ volatile( "ADD     LR, LR, #12" );
     __asm__ volatile( "MOV PC, LR" );*/
}
//----------------------------------------------------------------------------------
void startup_early_hook( void ) {
    if ( SRC_SRSR == 0x0 ) {
        IOMUXC_GPR_GPR1 &= ~IOMUXC_GPR_GPR1_GINT;
        /*GPIO1_ISR = 0;//0xFFFFFFFF;
         GPIO2_ISR = 0;//0xFFFFFFFF;
         GPIO3_ISR = 0;//0xFFFFFFFF;
         GPIO4_ISR = 0;//0xFFFFFFFF;
         GPIO5_ISR = 0xFFFFFFFF;
         GPIO6_ISR = 0xFFFFFFFF;
         GPIO7_ISR = 0xFFFFFFFF;
         GPIO8_ISR = 0xFFFFFFFF;
         GPIO9_ISR = 0xFFFFFFFF;*/
        SNVS_LPSR = 0xffffffff;
        // recover handshaking
        IOMUXC_GPR_GPR4  = 0x00000000;
        IOMUXC_GPR_GPR7  = 0x00000000;
        IOMUXC_GPR_GPR8  = 0x00000000;
        IOMUXC_GPR_GPR12 = 0x00000000;
        //load_context( );
        //start_up( );
    }
    
    //
}
//----------------------------------------------------------------------------------
void hal_initialize( void ( * ptr )( uint32_t ) ) {
    clear_flags = ptr;
}
//----------------------------------------------------------------------------------
void hal_idle ( void ) {
    
}
//----------------------------------------------------------------------------------
int source ( void ) {
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_sleep ( void ) {
    SCB_SCR = SCB_SCR_SLEEPDEEP;
    SYST_CSR &= ~SYST_CSR_TICKINT;
    uint32_t f_cpu = F_CPU_ACTUAL;
    /**************************************************************/
    // Wait mode sequence errata
    IOMUXC_GPR_GPR1 |= IOMUXC_GPR_GPR1_GINT;
    //NVIC_SET_PENDING(IRQ_GPR_IRQ);
    //NVIC_ENABLE_IRQ(IRQ_GPR_IRQ);
    GPC_IMR2 &= ~0x200;
    uint32_t clpcr = CCM_CLPCR & ( ~( 0x03 | CCM_CLPCR_ARM_CLK_DIS_ON_LPM ) );
    clpcr = CCM_CLPCR | CCM_CLPCR_LPM( 0x01 ) | CCM_CLPCR_ARM_CLK_DIS_ON_LPM | CCM_CLPCR_STBY_COUNT( 0x03 ) | 0x1C | 0x08280000;
    CCM_CLPCR = clpcr;
    GPC_IMR2 |= 0x200;
    //NVIC_DISABLE_IRQ(IRQ_GPR_IRQ);
    //NVIC_CLEAR_PENDING(IRQ_GPR_IRQ);
    /**************************************************************/
    uint32_t ccgr0 = CCM_CCGR0;
    uint32_t ccgr1 = CCM_CCGR1;
    uint32_t ccgr2 = CCM_CCGR2;
    uint32_t ccgr3 = CCM_CCGR3;
    uint32_t ccgr4 = CCM_CCGR4;
    uint32_t ccgr5 = CCM_CCGR5;
    uint32_t ccgr6 = CCM_CCGR6;
    CCM_CCGR0 = 0xC5;
    CCM_CCGR1 = 0 | ( CCM_CCGR1 & ( CCM_CCGR1_GPT1_SERIAL( 3 ) | CCM_CCGR1_GPT1_BUS( 3 ) ) );
    CCM_CCGR2 = 0x3 | ( CCM_CCGR2 & CCM_CCGR2_IOMUXC_SNVS( 3 ) );
    CCM_CCGR3 = 0x10000000 | ( CCM_CCGR3 & ( CCM_CCGR3_ACMP1( 3 ) | CCM_CCGR3_ACMP2( 3 ) | CCM_CCGR3_ACMP3( 3 ) | CCM_CCGR3_ACMP4( 3 ) ) );
    CCM_CCGR4 = 0x300;
    CCM_CCGR5 = 0x50001105;
    // We can enable DCDC when need to config it and close it after configuration
    CCM_CCGR6 = 0x400000;
    
    set_arm_clock( 132000000 );
    
    //USBPHY1_CTRL = 0xFFFFFFFF;
    //USBPHY2_CTRL = 0xFFFFFFFF;
    
    disable_plls( );
    
    //Disconnected the internal load resistor
    DCDC_REG1 &= ~DCDC_REG1_REG_RLOAD_SW;
    
    uint32_t reg0 = DCDC_REG0 & ~( DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |DCDC_REG0_PWD_OSC_INT );
    
    reg0 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK | DCDC_REG0_PWD_OSC_INT;
    
    DCDC_REG0 = reg0;
    
    IOMUXC_GPR_GPR8  = 0xaaaaaaaa;
    IOMUXC_GPR_GPR12 = 0x0000002a;
    
    __asm volatile ( "dsb \n" );
    __asm volatile ( "isb \n" );
    __asm volatile ( "wfi \n" );
    
    CCM_CLPCR = 0x78;
    
    CCM_CCGR0 = ccgr0;
    CCM_CCGR1 = ccgr1;
    CCM_CCGR2 = ccgr2;
    CCM_CCGR3 = ccgr3;
    CCM_CCGR4 = ccgr4;
    CCM_CCGR5 = ccgr5;
    CCM_CCGR6 = ccgr6;
    
    DCDC_REG1 |= DCDC_REG1_REG_RLOAD_SW;
    
    IOMUXC_GPR_GPR8  = 0x00000000;
    IOMUXC_GPR_GPR12 = 0x00000000;
    
    reg0 = DCDC_REG0 & ~( DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |DCDC_REG0_PWD_OSC_INT );
    
    //reg0 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH;
    
    DCDC_REG0 = reg0;
    
    set_arm_clock( f_cpu );
    
    enable_plls( );
    
    SYST_CSR |= SYST_CSR_TICKINT;
    
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_deepSleep ( void ) {
    SCB_SCR = SCB_SCR_SLEEPDEEP;
    uint32_t f_cpu = F_CPU_ACTUAL;
    SYST_CSR &= ~SYST_CSR_TICKINT;
    IOMUXC_GPR_GPR8  = 0xaaaaaaaa;
    IOMUXC_GPR_GPR12 = 0x0000002a;
    /**************************************************************/
    // Wait mode sequence errata
    IOMUXC_GPR_GPR1 |= IOMUXC_GPR_GPR1_GINT;
    //NVIC_SET_PENDING(IRQ_GPR_IRQ);
    //NVIC_ENABLE_IRQ(IRQ_GPR_IRQ);
    GPC_IMR2 &= ~0x200;
    uint32_t clpcr = CCM_CLPCR | CCM_CLPCR_LPM( 0x01 ) | CCM_CLPCR_ARM_CLK_DIS_ON_LPM | CCM_CLPCR_STBY_COUNT( 0x03 ) | 0x1C | 0x08280000;
    CCM_CLPCR = clpcr;
    GPC_IMR2 |= 0x200;
    //NVIC_DISABLE_IRQ(IRQ_GPR_IRQ);
    //NVIC_CLEAR_PENDING(IRQ_GPR_IRQ);
    /**************************************************************/
    uint32_t ccgr0 = CCM_CCGR0;
    uint32_t ccgr1 = CCM_CCGR1;
    uint32_t ccgr2 = CCM_CCGR2;
    uint32_t ccgr3 = CCM_CCGR3;
    uint32_t ccgr4 = CCM_CCGR4;
    uint32_t ccgr5 = CCM_CCGR5;
    uint32_t ccgr6 = CCM_CCGR6;
    CCM_CCGR0 = 0xC5;
    CCM_CCGR1 = 0 | ( CCM_CCGR1 & ( CCM_CCGR1_GPT1_SERIAL( 3 ) | CCM_CCGR1_GPT1_BUS( 3 ) ) );
    CCM_CCGR2 = 0x3 | ( CCM_CCGR2 & CCM_CCGR2_IOMUXC_SNVS( 3 ) );
    CCM_CCGR3 = 0x10000000 | ( CCM_CCGR3 & ( CCM_CCGR3_ACMP1( 3 ) | CCM_CCGR3_ACMP2( 3 ) | CCM_CCGR3_ACMP3( 3 ) | CCM_CCGR3_ACMP4( 3 ) ) );
    CCM_CCGR4 = 0x300;
    CCM_CCGR5 = 0x50001105;
    // We can enable DCDC when need to config it and close it after configuration
    CCM_CCGR6 = 0x400000;
    
    uint32_t reg0 = DCDC_REG0 & ~( DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |DCDC_REG0_PWD_OSC_INT );
    
    reg0 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK | DCDC_REG0_PWD_OSC_INT;
    
    DCDC_REG0 = reg0;
    
    disable_plls( );
    
    //Disconnected the internal load resistor
    DCDC_REG1 &= ~DCDC_REG1_REG_RLOAD_SW;
    
    //Power Down output range comparator
    DCDC_REG0 |= DCDC_REG0_PWD_CMP_OFFSET;
    
    //set_clock_osc( 24000000 );
    set_arm_clock( 24000000 );
    CCM_ANALOG_PLL_ARM |= CCM_ANALOG_PLL_ARM_BYPASS;
    CCM_ANALOG_PLL_ARM_SET = CCM_ANALOG_PLL_ARM_POWERDOWN;
    
    // Enable weak 2P5 and turn off regular 2P5 */
    PMU_REG_2P5 |= PMU_REG_2P5_ENABLE_WEAK_LINREG;
    PMU_REG_2P5 &= ~PMU_REG_2P5_ENABLE_LINREG;
    // Enable weak 1P1 and turn off regular 1P1 */
    PMU_REG_1P1 |= PMU_REG_1P1_ENABLE_WEAK_LINREG;
    PMU_REG_1P1 &= ~PMU_REG_1P1_ENABLE_LINREG;
    
    __disable_irq();
    set_clock_rc_osc( );
    power_down_subsytem( );
    __enable_irq();
    
    __asm volatile ( "dsb \n" );
    __asm volatile ( "wfi \n" );
    __asm volatile ( "isb \n" );
    
    CCM_CLPCR = 0x78;
    
    CCM_CCGR0 = ccgr0;
    CCM_CCGR1 = ccgr1;
    CCM_CCGR2 = ccgr2;
    CCM_CCGR3 = ccgr3;
    CCM_CCGR4 = ccgr4;
    CCM_CCGR5 = ccgr5;
    CCM_CCGR6 = ccgr6;
    
    DCDC_REG1 |= DCDC_REG1_REG_RLOAD_SW;
    
    IOMUXC_GPR_GPR8  = 0x00000000;
    IOMUXC_GPR_GPR12 = 0x00000000;
    
    // Configure the DCDC_REG0 register.
    reg0 = DCDC_REG0 & ~( DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |DCDC_REG0_PWD_OSC_INT );
    
    //reg0 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH;
    
    DCDC_REG0 = reg0;
    
    USBPHY1_CTRL_CLR = 0xC0000000;
    USBPHY2_CTRL_CLR = 0xC0000000;
    
    // Disconnect vdd_snvs_in and connect vdd_high_in
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS;
    
    // Increase OSC current to normal
    CCM_ANALOG_MISC0_CLR = CCM_ANALOG_MISC0_OSC_I(0x03);
    
    // Clear FET ODRIVE
    PMU_REG_CORE_CLR = PMU_REG_CORE_FET_ODRIVE;
    
    // Switch to XTAL-OSC
    __disable_irq();
    set_clock_xtal_osc();
    __enable_irq();
    
    // Enable regular 2P5 and wait for stable
    PMU_REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG;
    while ((PMU_REG_2P5 & PMU_REG_2P5_OK_VDD2P5) == 0);
    
    // Turn off weak 2P5
    PMU_REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG;
    
    // Enable regular 1P1 and wait for stable
    PMU_REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG;
    while ((PMU_REG_1P1 & PMU_REG_1P1_OK_VDD1P1) == 0);
    
    // Turn off weak 1P1
    PMU_REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG;
    
    enable_plls( );
    
    set_arm_clock( f_cpu );
    
    SYST_CSR |= SYST_CSR_TICKINT;
    
    return wake_source;
}
//----------------------------------------------------------------------------------
int hal_hibernate ( void ) {
    SCB_SCR = SCB_SCR_SLEEPDEEP;
    SYST_CSR &= ~SYST_CSR_TICKINT;
    /**************************************************************/
    // Wait mode sequence errata
    IOMUXC_GPR_GPR1 |= IOMUXC_GPR_GPR1_GINT;
    //NVIC_SET_PENDING(IRQ_GPR_IRQ);
    //NVIC_ENABLE_IRQ(IRQ_GPR_IRQ);
    GPC_IMR2 &= ~0x200;
    uint32_t clpcr;
    clpcr = CCM_CLPCR | ( CCM_CLPCR_LPM( 0x02 ) | CCM_CLPCR_VSTBY | 0x600 | CCM_CLPCR_SBYOS | CCM_CLPCR_ARM_CLK_DIS_ON_LPM | 0x08280000 );
    CCM_CLPCR = clpcr;
    GPC_IMR2 |= 0x200;
    //NVIC_DISABLE_IRQ(IRQ_GPR_IRQ);
    //NVIC_CLEAR_PENDING(IRQ_GPR_IRQ);
    /**************************************************************/
    CCM_CCGR0 = 0x45;
    CCM_CCGR1 = 0 | ( CCM_CCGR1 & ( CCM_CCGR1_GPT1_SERIAL( 2 ) | CCM_CCGR1_GPT1_BUS( 2 ) ) );
    CCM_CCGR2 = 0x1 | ( CCM_CCGR2 & CCM_CCGR2_IOMUXC_SNVS( 2 ) );
    CCM_CCGR3 = 0x10000000 | ( CCM_CCGR3 & ( CCM_CCGR3_ACMP1( 2 ) | CCM_CCGR3_ACMP2( 2 ) | CCM_CCGR3_ACMP3( 2 ) | CCM_CCGR3_ACMP4( 2 ) ) );
    CCM_CCGR4 = 0x110;
    CCM_CCGR5 = 0x1105;
    // We can enable DCDC when need to config it and close it after configuration
    CCM_CCGR6 = 0x400000;
    
    // Turn off FlexRAM1
    PGC_MEGA_CTRL |= PGC_MEGA_CTRL_PCR;
    
    // Turn off FlexRAM0
    GPC_CNTR |= GPC_CNTR_PDRAM0_PGE;
    
#define __VECTOR_TABLE _VectorsRam
    //IOMUXC_GPR_GPR16 = 0x200007;//( uint32_t )( (( uint32_t )__VECTOR_TABLE) | 7 );
    
    SCB_CleanDCache();
    SCB_DisableDCache();
    
    set_clock_osc( 24000000 );
    CCM_ANALOG_PLL_ARM |= CCM_ANALOG_PLL_ARM_BYPASS;
    CCM_ANALOG_PLL_ARM_SET = CCM_ANALOG_PLL_ARM_POWERDOWN;
    
    __disable_irq();
    set_clock_rc_osc( );
    __enable_irq();
    
    //Disconnected the internal load resistor
    DCDC_REG1 &= ~DCDC_REG1_REG_RLOAD_SW;
    
    //Power Down output range comparator
    DCDC_REG0 |= DCDC_REG0_PWD_CMP_OFFSET;
    
    DCDC_REG0 &= ~( DCDC_REG0_PWD_ZCD | DCDC_REG0_PWD_CMP_OFFSET );
    DCDC_REG2 = ( ~0xE00 & DCDC_REG2 ) | DCDC_REG2_LOOPCTRL_EN_RCSCALE1( 0x4 ) | DCDC_REG2_DCM_SET_CTRL;
    // Switch DCDC to use DCDC internal OSC
    // Configure the DCDC_REG0 register.
    uint32_t reg0 = DCDC_REG0 & ~( DCDC_REG0_XTAL_24M_OK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH | DCDC_REG0_SEL_CLK |DCDC_REG0_PWD_OSC_INT );
    reg0 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH;
    DCDC_REG0 = reg0;
    
    // Power down CPU when requested
    PGC_CPU_CTRL = PGC_CPU_CTRL_PCR;
    //PGC_MEGA_CTRL = PGC_CPU_CTRL_PCR;
    //PGC_CPU_PDNSCR = 0xA0A;
    
    PMU_REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE;
    // Disconnect vdd_high_in and connect vdd_snvs_in
    CCM_ANALOG_MISC0_SET = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS;
    // STOP_MODE config, turn off all analog except RTC in stop mode
    CCM_ANALOG_MISC0_CLR = 0xC00;//CCM_ANALOG_MISC0_STOP_MODE_CONFIG( 0x3 );
    //CCM_ANALOG_MISC0_SET = 0x800;
    
    uint32_t gpc_imr1 = GPC_IMR1;
    uint32_t gpc_imr2 = GPC_IMR2;
    uint32_t gpc_imr3 = GPC_IMR3;
    uint32_t gpc_imr4 = GPC_IMR4;
    uint32_t gpc_imr5 = GPC_IMR5;
    
    GPC_IMR1 = 0xFFFFFFFF;
    GPC_IMR2 = 0xFFFFFFFF;
    GPC_IMR3 = 0xFFFFFFFF;
    GPC_IMR4 = 0xFFFFFFFF;
    GPC_IMR5 = 0xFFFFFFFF;
    
    CCM_CCR = ( CCM_CCR & ~0x7E00000 ) | CCM_CCR_REG_BYPASS_COUNT( 0x3 ) | CCM_CCR_OSCNT( 0xAF );
    CCM_CCR |= ( CCM_CCR_RBC_EN | CCM_CCR_COSC_EN  );
    
    for ( int i = 0; i < 12 * 22; i++ ) __asm volatile ( "nop \n" );
    
    GPC_IMR1 = gpc_imr1;
    GPC_IMR2 = gpc_imr2 | ( 0x20000200 );
    GPC_IMR3 = gpc_imr3;
    GPC_IMR4 = gpc_imr4;
    GPC_IMR5 = gpc_imr5;
    
    IOMUXC_GPR_GPR4 = 0x00000010;
    while( ( IOMUXC_GPR_GPR4 & 0x00100000 ) != 0x00100000 );
    IOMUXC_GPR_GPR4 = 0x000036f0;
    IOMUXC_GPR_GPR7 = 0x0000ffff;
    IOMUXC_GPR_GPR8 = 0xfffcffff;
    IOMUXC_GPR_GPR12 = 0x0000000a;
    while( ( IOMUXC_GPR_GPR4 & 0x36f00000 ) != 0x36f00000 );
    while( ( IOMUXC_GPR_GPR7 & 0xffff0000 ) != 0xffff0000 );

    /*IOMUXC_GPR_GPR4 = 0x00000011;
     while ((IOMUXC_GPR_GPR4 & 0x00110000) != 0x00110000){};
     digitalWriteFast( LED_BUILTIN, HIGH );
     IOMUXC_GPR_GPR4 = 0x000036ff;
     IOMUXC_GPR_GPR7 = 0x0000ffff;
     IOMUXC_GPR_GPR8 = 0xfffcffff;
     IOMUXC_GPR_GPR12 = 0x0000000a;
     while ((IOMUXC_GPR_GPR4 & 0x36f90000) != 0x36f90000){};
     while ((IOMUXC_GPR_GPR7 & 0xffff0000) != 0xffff0000){};*/
    //save_context( );
    //while(1);
    
    __asm volatile ( "dsb \n" );
    __asm volatile ( "isb \n" );
    __asm volatile ( "wfi \n" );
    
    // future landing spot for context restore from hibernate reset
    while( 1 );
    
    return wake_source;
}
#endif /* IMXRT1062 */
