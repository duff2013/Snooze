/***********************************************************************************
 *  SnoozeUSBSerial.h
 *  Teensy LC
 *
 * Purpose: USBSerial Driver
 *
 **********************************************************************************/
#if defined(__MKL26Z64__)

#include "SnoozeUSBSerial.h"
#include "usb_dev.h"

/*******************************************************************************
 *  Sets the usb_configuration, usb_cdc_line_rtsdtr and usb_cdc_line_rtsdtr_millis
 *  to initial state so while(!Serial) works after sleeping. Still buggy...
 *******************************************************************************/
void SnoozeUSBSerial::disableDriver( uint8_t mode ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    //if ( mode == RUN_LP ) return;
    if (mode == 0) return;
    //usb_configuration = 0;
    usb_cdc_line_rtsdtr = 0;
    usb_cdc_line_rtsdtr_millis = systick_millis_count;
    //if ( mode == VLPW ) {
    if ( mode == 1 ) {
        SIM_SCGC4 |= SIM_SCGC4_USBOTG;// enable USB clock
        USB0_USBCTRL &= ~USB_USBCTRL_SUSP;// suspended State
        USB0_CTL |= USB_CTL_USBENSOFEN;// enable USB
        while ( ( USB0_CTL & USB_CTL_USBENSOFEN ) != 0x01 );
    }
#endif
}
/*******************************************************************************
 *  Turns off usb clock if using 'sleep'.
 *******************************************************************************/
void SnoozeUSBSerial::enableDriver( uint8_t mode ) {
#if F_CPU >= 20000000 && defined(USB_SERIAL)
    //if ( mode == RUN_LP ) return;
    if (mode == 0) return;
    //if ( mode == VLPW ) {
    if ( mode == 1 ) {
        USB0_USBCTRL |= USB_USBCTRL_SUSP;// suspended State
        USB0_CTL &= ~USB_CTL_USBENSOFEN;// disable USB
        while ( ( USB0_CTL & USB_CTL_USBENSOFEN ) != 0x00 );
        SIM_SCGC4 &= ~SIM_SCGC4_USBOTG;// disable USB clock
    }
#endif
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::clearIsrFlags( uint32_t ipsr ) {
    //isr( );
}
/*******************************************************************************
 *  not used
 *******************************************************************************/
void SnoozeUSBSerial::isr( void ) {

}
#endif /* __MKL26Z64__ */
