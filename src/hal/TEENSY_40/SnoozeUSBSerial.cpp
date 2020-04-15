/***********************************************************************************
 *  SnoozeUSBSerial.h
 *  Teensy 4.0
 *
 * Purpose: USBSerial Driver
 *
 **********************************************************************************/
#if defined(__IMXRT1062__)

#include "SnoozeUSBSerial.h"
#include "usb_dev.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern void start_usb_pll(void);
#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *  Sets the usb_configuration, usb_cdc_line_rtsdtr and usb_cdc_line_rtsdtr_millis
 *  to initial state so while(!Serial) works after sleeping. Still buggy...
 *******************************************************************************/
void SnoozeUSBSerial::disableDriver( uint8_t mode ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    if (mode == 0) return;
    //if ( mode == 1 ) {
    if (pll_usb1 & CCM_ANALOG_PLL_USB1_POWER) {
        USBPHY1_CTRL_CLR = USBPHY_CTRL_CLKGATE;
        USBPHY1_PWD_CLR = 0xFFFFFFFF;
        NVIC_ENABLE_IRQ( IRQ_USB1 );
        start_usb_pll( );
        usb_cdc_line_rtsdtr = 0;
        usb_cdc_line_rtsdtr_millis = systick_millis_count;
    }
    //}
#endif
}
/*******************************************************************************
 *  Turns off usb clock if using 'sleep'.
 *******************************************************************************/
void SnoozeUSBSerial::enableDriver( uint8_t mode ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    if ( mode == 0 ) return;
    pll_usb1 = CCM_ANALOG_PLL_USB1;
    if ( pll_usb1 & CCM_ANALOG_PLL_USB1_POWER ) {
        /*NVIC_DISABLE_IRQ( IRQ_USB1 );
         CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS;
         CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER;
         //CCM_ANALOG_PFD_480_SET  = 0x800000;//CCM_ANALOG_PFD_480_PFD2_CLKGATE;
         CCM_ANALOG_PFD_480 |= 0x80808080;
         USBPHY1_PWD  = 0x1E1C00;*/
        
        NVIC_DISABLE_IRQ( IRQ_USB1 );
        CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS;
        CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_EN_USB_CLKS | // disable usb
        CCM_ANALOG_PLL_USB1_ENABLE |      // disable
        //CCM_ANALOG_PLL_USB1_DIV_SELECT |  // use 480 MHz
        CCM_ANALOG_PLL_USB1_POWER;        // power down
        CCM_ANALOG_PFD_480 |= 0x80808080;
        //CCM_ANALOG_PFD_480_SET  = CCM_ANALOG_PFD_480_PFD2_CLKGATE;
        //while ( ( CCM_ANALOG_PLL_USB1 & CCM_ANALOG_PLL_USB1_ENABLE ) );
        //while ( ( CCM_ANALOG_PLL_USB1 & CCM_ANALOG_PLL_USB1_POWER ) );
        //while ( ( CCM_ANALOG_PLL_USB1 & CCM_ANALOG_PLL_USB1_LOCK ) );
        //while ( !( CCM_ANALOG_PLL_USB1 & CCM_ANALOG_PLL_USB1_BYPASS ) );
        //while ( ( CCM_ANALOG_PLL_USB1 & CCM_ANALOG_PLL_USB1_EN_USB_CLKS ) );
        USBPHY1_PWD  =  0x1E1C00;//
        USBPHY2_PWD  = 0x1E1C00;//
        //USBPHY1_CTRL = 0xFFFFFFFF;
        //USBPHY2_CTRL = 0xFFFFFFFF;
        
        USBPHY1_CTRL_CLR = 0x200000;//0xFFFFFFFF;
        USBPHY1_CTRL_SET = 0x40000000;//0xFFFFFFFF;
        USBPHY2_CTRL_CLR = 0x200000;//0xFFFFFFFF;
    }
#endif
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::clearIsrFlags( uint32_t ipsr ) {
    
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::isr( void ) {
    
}
#endif /* __IMXRT1062__ */
