/***********************************************************************************
 *  SnoozeUSBSerial.h
 *  Teensy 3.x/LC
 *
 * Purpose: USBSerial Driver
 *
 ***********************************************************************************/
#include "SnoozeUSBSerial.h"
#include "usb_dev.h"

/*******************************************************************************
 *  Sets the usb_configuration, usb_cdc_line_rtsdtr and usb_cdc_line_rtsdtr_millis
 *  to initial state so while(!Serial) works after sleeping. Still buggy...
 *******************************************************************************/
void SnoozeUSBSerial::disableDriver( void ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    if ( mode == RUN_LP ) return;
    //usb_configuration = 0;
    usb_cdc_line_rtsdtr = 0;
    usb_cdc_line_rtsdtr_millis = systick_millis_count;
    if ( mode == VLPW ) {
        SIM_SCGC4 |= SIM_SCGC4_USBOTG;// enable USB clock
        USB0_USBCTRL &= ~USB_USBCTRL_SUSP;// suspended State
        USB0_CTL |= USB_CTL_USBENSOFEN;// enable USB
        while ( ( USB0_CTL & USB_CTL_USBENSOFEN ) != 0x01 );
#if defined(__MK66FX1M0__) && ((F_CPU == 256000000) || (F_CPU == 216000000) || (F_CPU == 180000000))
        SIM_SOPT2 |= SIM_SOPT2_IRC48SEL;
#endif
    }
#endif
}
/*******************************************************************************
 *  Turns off usb clock if using 'sleep'.
 *******************************************************************************/
void SnoozeUSBSerial::enableDriver( void ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    if ( mode == RUN_LP ) return;
    if ( mode == VLPW ) {
        USB0_USBCTRL |= USB_USBCTRL_SUSP;// suspended State
        USB0_CTL &= ~USB_CTL_USBENSOFEN;// disable USB
        while ( ( USB0_CTL & USB_CTL_USBENSOFEN ) != 0x00 );
#if defined(__MK66FX1M0__) && ((F_CPU == 256000000) || (F_CPU == 216000000) || (F_CPU == 180000000))
        SIM_SOPT2 &= ~SIM_SOPT2_IRC48SEL;
#endif
        SIM_SCGC4 &= ~SIM_SCGC4_USBOTG;// disable USB clock
    }
#endif
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::clearIsrFlags( void ) {
    //isr( );
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::isr( void ) {

}
